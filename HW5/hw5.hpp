#ifndef __HW5__
#define __HW5__

#include "StackStructs.h"
#include "bp.hpp"
#include "symbolTable.hpp"
#include <cassert>
#include <algorithm>
#include <set>
#include <sstream>


#define DEBUG
#define DEBUG_API
#define emit(s) CodeBuffer::instance().emit(s)
#define emitData(s) CodeBuffer::instance().emitData(s)
#define print_code_buffer CodeBuffer::instance().printCodeBuffer()

register_type loadImmediateToRegister(string num);

string register_type_to_str(register_type type) {
    switch (type) {
        case t0:
        return "$t0";
        case t1:
        return "$t1";
        case t2:
        return "$t2";
        case t3:
        return "$t3";
        case t4:
        return "$t4";
        case t5:
        return "$t5";
        case t6:
        return "$t6";
        case t7:
        return "$t7";
        case s0:
        return "$s0";
        case s1:
        return "$s1";
        case s2:
        return "$s2";
        case s3:
        return "$s3";
        case s4:
        return "$s4";
        case s5:
        return "$s5";
        case s6:
        return "$s6";
        case s7:
        return "$s7";
        case t8:
        return "$t8";
        case t9:
        return "$t9";
        default:
        return "";
    }
    return "";
}

string op_to_string(arithmetic_op op) {
    switch(op) {
        case add_op:
        return "addu";
        case sub_op:
        return "subu";
        case mul_op:
        return "mul";
        case div_op:
        return "div";
    }
}

class RegisterAllocation {
    vector<register_type> available;
    set<register_type> used;
public:
    RegisterAllocation() {
        for (int i = t0; i < t9; ++i) {
            available.push_back((register_type)i);
        }
    }

    register_type allocateRegister() {
        assert(!available.empty());
        register_type res = available[0];
        used.insert(res);
        available.erase(available.begin());
        #ifdef DEBUG
            cout << "-I- Allocated register " << register_type_to_str(res) << endl;
        #endif
        return res;
    }

    void freeRegister(register_type reg) {
        if (std::find(used.begin(), used.end(), reg) == used.end()) {
            return;
        }
        assert(used.count(reg) == 1);
        used.erase(std::find(used.begin(), used.end(), reg));
        available.push_back(reg);
        #ifdef DEBUG
            cout << "-I- Freed register " << register_type_to_str(reg) << endl;
        #endif
    }
};

RegisterAllocation *reg_alloc;

void allocateVar(register_type reg = no_reg) {
    #ifdef DEBUG_API
        cout << "-API- Running allocateVar" << endl;
    #endif
    emit("subu $sp, $sp, 4");
    stringstream s;
    if (reg == no_reg) {
        s << "sw " << "$0" << ", ($sp)";
    } else {
        s << "sw " << register_type_to_str(reg) << ", ($sp)";
    }

    emit(s.str());
    // After allocating variable from register, free the register
    reg_alloc->freeRegister(reg);
}

void assignToVar(VariableEntry* var_entry, StackType st) {
    stringstream s;
    int offset = var_entry->getWordOffset();
    register_type reg = st.reg;
    if (reg == no_reg) {
        reg = loadImmediateToRegister(st.str);
    }
    s << "sw " << register_type_to_str(reg) << ", " << -offset << "($fp)";
    emit(s.str());
    reg_alloc->freeRegister(reg);
}

register_type arithmetic_op(StackType s1, StackType s3, arithmetic_op op) {
    #ifdef DEBUG_API
        cout << "-API- Running arithmetic_op" << endl;
    #endif

    register_type reg1 = s1.reg;
	register_type reg3 = s3.reg;
	string reg1_str = s1.str;
	string reg3_str = s3.str;

	// MAX TODO : check for struct types later too....

	if (reg1 == no_reg) {
		reg1 = loadImmediateToRegister(reg1_str);
	}
	if (reg3 == no_reg) {
		reg3 = loadImmediateToRegister(reg3_str);
	}

    bool truncate_result = false;
    if (s1.type == types_Byte || s3.type == types_Byte) {
		truncate_result = true;
	}

    string op_str = op_to_string(op);
    stringstream s;
    s << op_str << " " << register_type_to_str(reg1) << ", " << register_type_to_str(reg1) << ", " << register_type_to_str(reg3);
    if (truncate_result) {
        s << endl << "and " << register_type_to_str(reg1) << ", " << "255";
    }
    emit(s.str());
    reg_alloc->freeRegister(reg3);

    return reg1;
}

register_type loadToRegister(VariableEntry* var_entry) {
    #ifdef DEBUG_API
        cout << "-API- Running loadToRegister" << endl;
    #endif
    register_type res = reg_alloc->allocateRegister();
    stringstream s;
    int offset = var_entry->getWordOffset();
    s << "lw " << register_type_to_str(res) << ", " << -offset << "($fp)";
    emit(s.str());

    return res;
}

register_type loadImmediateToRegister(string num) {
    stringstream s;
    register_type reg = reg_alloc->allocateRegister();
    s << "li " << register_type_to_str(reg) << ", " << num;
    emit(s.str());
    return reg;
}

void checkDivisionByZero(register_type reg) {
    stringstream s;
    s << "beq " << register_type_to_str(reg) << ", 0, DivisionByZero";
    emit(s.str());
}

void init_text() {
    emit(".globl     main");
    emit(".ent     main");
    emit("main:");
    emit("subu $sp, $sp, 4");
    emit("sw $ra, ($sp)");
    emit("jal __main");
    emit("ExitCode: li $v0, 10");
    emit("syscall");
    emit("__print:");
    emit("lw $a0, 0($sp)");
    emit("li $v0, 4");
    emit("syscall");
    emit("jr $ra");
    emit("__printi:");
    emit("lw $a0, 0($sp)");
    emit("li $v0, 1");
    emit("syscall");
    emit("jr $ra");
    emit("DivisionByZero: .asciiz \"Error division by zero\\n\"");
    emit("j ExitCode");
}

#endif