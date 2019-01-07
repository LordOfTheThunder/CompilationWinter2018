#ifndef __HW5__
#define __HW5__

#include "StackStructs.h"
#include "bp.hpp"
#include "symbolTable.hpp"
#include <cassert>
#include <algorithm>
#include <set>
#include <sstream>


//#define DEBUG
//#define DEBUG_API
#define emit(s) CodeBuffer::instance().emit(s)
#define emitData(s) CodeBuffer::instance().emitData(s)
#define print_code_buffer CodeBuffer::instance().printCodeBuffer()
#define print_data_buffer CodeBuffer::instance().printDataBuffer()
symbolTable* tables = NULL;
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

void allocateVar(StackType st = StackType()) {
    #ifdef DEBUG_API
        cout << "-API- Running allocateVar" << endl;
    #endif

    emit("subu $sp, $sp, 4");
    if (st.type == types_Undefined) {
        // allocate var without copying data
        emit("sw $0, ($sp)");
        return;
    }

    register_type reg = st.reg;
    stringstream s;

    if (reg == no_reg) {
        reg = loadImmediateToRegister(st.str);
    }
    s << "sw " << register_type_to_str(reg) << ", ($sp)";

    emit(s.str());
    // After allocating variable from register, free the register
    reg_alloc->freeRegister(reg);
}

void assignToStruct(StructEntry* st_type) {
    #ifdef DEBUG_API
        cout << "-API- Running assignToStruct" << endl;
    #endif
    stringstream s;
    int size = st_type->size();
    // otherwise we copy data from one struct to another struct
    int offset = size * 4 - 4;
    int orig_offset = st_type->getWordOffset();
    register_type reg = reg_alloc->allocateRegister();
    for (int i = 0; i < size; ++i) {
        s << "lw " << register_type_to_str(reg) << ", " << -orig_offset << "($fp)" << endl;
        s << "sw " << register_type_to_str(reg) << ", " << offset << "($sp)";
        if (i < size - 1) {
            s << endl;
        }
        offset -= 4;
        orig_offset += 4;
    }
    reg_alloc->freeRegister(reg);
    emit(s.str());
}

void allocateStruct(StructEntry* st_type, bool assign = false) {
    #ifdef DEBUG_API
        cout << "-API- Running allocateStruct" << endl;
    #endif

    int size = st_type->size();
    stringstream s;
    s << "subu $sp, $sp, " << size * 4;
    emit(s.str());
    s.clear();
    s.str(std::string());
    if (!assign) {
        // allocate struct without copying data
        for (int i = 0; i < size; ++i) {
            int curr_offset = -i * 4;
            s << "sw $0, " << curr_offset << "($sp)";
            if (i < size - 1) {
                s << endl;
            }
        }
        emit(s.str());
        return;
    }

    assignToStruct(st_type);
}

void assignToVar(int offset, StackType st) {
    stringstream s;
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
    if (s1.type == types_Byte && s3.type == types_Byte) {
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

void defFunction(string func_name) {
    emit("__" + func_name + ":");
    emit("move $fp, $sp");
}

bool isImmediate(string name) {
    if (name == "true" || name == "false") {
        return true;
    }
    if (name[0] <= '9' && name[0] >= '0') {
        return true;
    }
    return false;
}

void addVarToFunc(string var_name) {
    VariableEntry* var_entry = tables->getVariable(var_name);
    int offset = var_entry->getWordOffset();
    emit("subu $sp, $sp, 4");
    register_type reg = reg_alloc->allocateRegister();
    stringstream s;
    s << "lw " << register_type_to_str(reg) << ", " << -offset << "($fp)" << endl;
    s << "sw " << register_type_to_str(reg) << ", " << "($sp)";
    emit(s.str());
    reg_alloc->freeRegister(reg);
}

string boolImmediateToString(string imm_value) {
    // get right output in case imm_value is bool
    if (imm_value == "true") {
        return "1";
    }
    else if (imm_value == "false") {
        return "0";
    }
    return imm_value;
}

void returnValueFromFunction(string name) {
    if (isImmediate(name)) {
        // we return immediate
        // If number : returns the number as string
        // If bool, returns 0 or 1
        emit("li $v0, " + boolImmediateToString(name));
        return;
    }
    // We return a value.
    stringstream s;
    VariableEntry* var_entry = tables->getVariable(name);
    assert(var_entry != NULL);
    int offset = var_entry->getWordOffset();
    s << "lw $v0, " << -offset << "($fp)";
    emit(s.str());
}

void addImmediateToFunc(string imm_value) {
    emit("subu $sp, $sp, 4");
    register_type reg = reg_alloc->allocateRegister();
    stringstream s;
    imm_value = boolImmediateToString(imm_value);
    if (imm_value[imm_value.size() - 1] == 'b') {
        imm_value = imm_value.substr(0, imm_value.size() - 1);
    }
    s << "li " << register_type_to_str(reg) << ", " << imm_value << endl;
    s << "sw " << register_type_to_str(reg) << ", " << "($sp)";
    emit(s.str());
    reg_alloc->freeRegister(reg);
}

void addStructTypeToFunc(string var_name) {
    VariableEntry* var_entry = tables->getVariable(var_name);
    string type = var_entry->getType();
    StructEntry* struct_entry = tables->getStruct(type);
    int offset = var_entry->getWordOffset();

    register_type reg = reg_alloc->allocateRegister();
    stringstream s;
    emit("subu $sp, $sp, " + 4 * struct_entry->size());
    for (int i = 1; i <= struct_entry->size(); ++i) {
        int curr_write_loc = 4 * (struct_entry->size() - i);
        s << "lw " << register_type_to_str(reg) << ", " << -offset << "($fp)" << endl;
        s << "sw " << register_type_to_str(reg) << ", " << -curr_write_loc << "($sp)";
        if (i < struct_entry->size()) {
            s << endl;
        }
        offset += 4 * i;
    }
    emit(s.str());
    reg_alloc->freeRegister(reg);
}

int calculateParamSize(StackType st) {
    int size = 0;
    vector<varPair> params = st.func_info;
    for (int i = 0; i < params.size(); ++i) {
        if (st.struct_type != "") {
            // we have a struct type
            VariableEntry* var_entry = tables->getVariable(params[i].id);
            string type = var_entry->getType();
            StructEntry* struct_entry = tables->getStruct(type);
            size += 4 * struct_entry->size();
        } else {
            size += 4;
        }
    }
    return size;
}

void returnFromFunc(StackType st) {
    int size = calculateParamSize(st);
    if (size > 0) {
        stringstream s;
        s << "addu $sp, $sp, " << size;
        emit(s.str());
    }
    emit("lw $ra, ($sp)");
    emit("lw $fp, 4($sp)");
    emit("addu $sp, $sp, 8");
    emit("move $sp, $fp");
}

void callFunction(string func_name, StackType st = StackType()) {
    #ifdef DEBUG_API
        cout << "-API- Running callFunction" << endl;
    #endif
    vector<varPair> params = st.func_info;
    emit("subu $sp, $sp, 8");
    emit("sw $ra, ($sp)");
    emit("sw $fp, -4($sp)");
    for (int i = 0; i < params.size(); ++i) {
        if (st.struct_type != "") {
            // we have a struct type
        } else if (isImmediate(params[i].id)) {
            // we have an integer, a byte or a boolean as immediate
            addImmediateToFunc(params[i].id);
        } else {
            // we have variable type
            addVarToFunc(params[i].id);
        }
    }
    emit("jal __" + func_name);
    returnFromFunc(st);
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
    emitData("DivisionByZero: .asciiz \"Error division by zero\\n\"");
    emit("j ExitCode");
}

#endif