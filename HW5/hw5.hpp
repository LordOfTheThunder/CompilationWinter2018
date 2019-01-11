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
string boolImmediateToString(string imm_value);

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
        case v0:
        return "$v0";
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

    register_type reg = st.reg;
    emit("subu $sp, $sp, 4");
    if (st.type == types_Undefined) {
        // allocate var without copying data
        emit("sw $0, ($sp)");
        return;
    }

    stringstream s;

    if (reg == no_reg) {
        reg = loadImmediateToRegister(st.str);
    }
    s << "sw " << register_type_to_str(reg) << ", ($sp)";

    emit(s.str());
    // After allocating variable from register, free the register
    reg_alloc->freeRegister(reg);
}

void assignToStruct(string var_name) {
    #ifdef DEBUG_API
        cout << "-API- Running assignToStruct" << endl;
    #endif
    stringstream s;
    VariableEntry* var_entry = tables->getVariable(var_name);
    assert(var_entry != NULL);

    StructEntry* st_type = tables->getStruct(var_entry->getType());
    assert(st_type != NULL);

    int size = st_type->size();
    // otherwise we copy data from one struct to another struct
    int offset = size * 4 - 4;
    int orig_offset = var_entry->getWordOffset() - size * 4;
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

void logicalRelops(const string& rop_str, register_type t1, register_type t2, vector<int>& true_list, vector<int>& false_list){
    string rop = "";
    if(rop_str == "=="){
        rop = "beq ";
    } else if(rop_str == "!="){
        rop = "bne ";
    } else if(rop_str == "<"){
        rop = "blt ";
    } else if(rop_str == ">"){
        rop = "bgt ";
    } else if(rop_str == "<="){
        rop = "ble ";
    } else {
        rop = "bge ";
    }
    true_list = MAKE_LIST(emit(rop + register_type_to_str(t1) + ", " + register_type_to_str(t2) + ", " ));
    false_list = MAKE_LIST(emit("j "));
}

void allocateStruct(StructEntry* st_type, string var_name, bool assign = false) {
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

    assignToStruct(var_name);
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
    s << "li " << register_type_to_str(reg) << ", " << boolImmediateToString(num);
    emit(s.str());
    return reg;
}

register_type loadStructMemberToRegister(string struct_name, string member) {
    stringstream s;
    register_type reg = reg_alloc->allocateRegister();
    int offset = tables->getStructMemberWordOffset(struct_name, member);
    s << "lw " << register_type_to_str(reg) << ", " << -offset << "($fp)";
    emit(s.str());
    return reg;
}

void checkDivisionByZero(StackType st) {
    stringstream s;
    if (st.reg == no_reg) {
        st.reg = loadImmediateToRegister(st.str);
    }
    s << "beq " << register_type_to_str(st.reg) << ", 0, TerminateZero";
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

void callReturnFunc() {
    emit("move $sp, $fp");
    emit("jr $ra");
}

int getOffsetFromStructVar(string var_name) {
    string struct_type = var_name.substr(0, var_name.find("."));
    string member = var_name.substr(var_name.find(".") + 1, var_name.size() - var_name.find("."));
    return tables->getStructMemberWordOffset(struct_type, member);
}

void addRegisterToFunc(register_type reg) {
    emit("subu $sp, $sp, 4");
    emit("sw " + register_type_to_str(reg) + ", ($sp)");
    reg_alloc->freeRegister(reg);
}

void addVarToFunc(string var_name) {
    VariableEntry* var_entry = tables->getVariable(var_name);
    int offset;
    if (var_entry == NULL) {
        // We probably have a variable of a struct
        offset = getOffsetFromStructVar(var_name);
    } else {
        offset = var_entry->getWordOffset();
    }
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

void returnValueFromFunction(StackType st) {
    if (st.reg != no_reg) {
        // we have result stored in register
        //emit("move $v0, " + register_type_to_str(st.reg));
        stringstream s;
        emit("subu $sp, $sp, 4");
        s << "sw " << register_type_to_str(st.reg) << ", ($sp)";
        emit(s.str());
        reg_alloc->freeRegister(st.reg);
        return;
    }
    string name = st.str;
    if (isImmediate(name)) {
        // we return immediate
        // If number : returns the number as string
        // If bool, returns 0 or 1
        //emit("li $v0, " + boolImmediateToString(name));
        register_type reg = reg_alloc->allocateRegister();
        stringstream s;
        s << "li " << register_type_to_str(reg) << ", " << boolImmediateToString(name);
        emit(s.str());
        s.clear();
        s.str(string());
        emit("subu $sp, $sp, 4");
        s << "sw " << register_type_to_str(reg) << ", ($sp)";
        emit(s.str());
        reg_alloc->freeRegister(reg);
        return;
    }
    // We return a value.
    stringstream s;
    VariableEntry* var_entry = tables->getVariable(name);
    assert(var_entry != NULL);
    int offset = var_entry->getWordOffset();
    register_type reg = reg_alloc->allocateRegister();
    s << "lw " << register_type_to_str(reg) << -offset << "($fp)";
    emit(s.str());
    s.clear();
    s.str(string());
    emit("subu $sp, $sp, 4");
    s << "sw " << register_type_to_str(st.reg) << ", ($sp)";
    emit(s.str());
    //s << "lw $v0, " << -offset << "($fp)";
    reg_alloc->freeRegister(reg);
}

void addImmediateToFunc(string imm_value) {
    emit("subu $sp, $sp, 4");
    register_type reg = reg_alloc->allocateRegister();
    stringstream s;
    imm_value = boolImmediateToString(imm_value);
    if (imm_value[imm_value.size() - 1] == 'b') {
        imm_value = imm_value.substr(0, imm_value.size() - 1);
    }
    s << "li " << register_type_to_str(reg) << ", " << boolImmediateToString(imm_value) << endl;
    s << "sw " << register_type_to_str(reg) << ", " << "($sp)";
    emit(s.str());
    reg_alloc->freeRegister(reg);
}

void addStructTypeToFunc(string var_name) {
    VariableEntry* var_entry = tables->getVariable(var_name);
    assert(var_entry != NULL);
    string type = var_entry->getType();
    StructEntry* struct_entry = tables->getStruct(type);
    assert(struct_entry != NULL);
    int offset = var_entry->getWordOffset();

    register_type reg = reg_alloc->allocateRegister();
    stringstream s;
    s << "subu $sp, $sp, " << 4 * struct_entry->size();
    emit(s.str());
    s.clear();
    s.str(std::string());
    for (int i = 1; i <= struct_entry->size(); ++i) {
        int curr_write_loc = 4 * (struct_entry->size() - i);
        s << "lw " << register_type_to_str(reg) << ", " << -offset << "($fp)" << endl;
        s << "sw " << register_type_to_str(reg) << ", " << curr_write_loc << "($sp)";
        if (i < struct_entry->size()) {
            s << endl;
        }
        offset += 4;
    }
    emit(s.str());
    reg_alloc->freeRegister(reg);
}

void returnFromFunc(StackType st) {
    emit("lw $ra, ($sp)");
    emit("lw $fp, 4($sp)");
    emit("addu $sp, $sp, 8");
}

register_type createString(string str) {
    register_type reg = reg_alloc->allocateRegister();
    static int counter = 0;
    stringstream s;
    s << "string_label" << counter++;
    string str_label = s.str();
    s << ": .asciiz " << str;
    emitData(s.str());
    emit("la " + register_type_to_str(reg) + ", " + str_label);
    return reg;
}

register_type callFunction(string func_name, StackType st = StackType()) {
    #ifdef DEBUG_API
        cout << "-API- Running callFunction" << endl;
    #endif

    vector<varPair> params = st.func_info;
    emit("subu $sp, $sp, 8");
    emit("sw $ra, ($sp)");
    emit("sw $fp, 4($sp)");

    if (func_name == "print") {
        // Special case for print function
        register_type reg = createString(st.str);
        st.reg = reg;
        allocateVar(st);
        reg_alloc->freeRegister(reg);
        emit("jal __" + func_name);
        emit("addu $sp, $sp, 4");
        returnFromFunc(st);
        return no_reg;
    }

    int size = 0;
    for (int i = params.size() - 1; i >= 0; --i) {
        VariableEntry* var_entry = tables->getVariable(params[i].id);
        if (params[i].reg != no_reg) {
            // We have some kind of binop/relop result saved in register
            addRegisterToFunc(params[i].reg);
            size += 4;
        } else if (isStructType(params[i].type)) {
            // we have a struct type
            addStructTypeToFunc(params[i].id);
            StructEntry* struct_entry = tables->getStruct(params[i].type);
            size += 4 * struct_entry->size();
        } else if (isImmediate(params[i].id)) {
            // we have an integer, a byte or a boolean as immediate
            addImmediateToFunc(params[i].id);
            size += 4;
        } else if (var_entry != NULL) {
            // we have variable type
            addVarToFunc(params[i].id);
            size += 4;
        } else {
            // we have return value from function
        }
    }
    emit("jal __" + func_name);

    register_type reg = no_reg;
    FunctionEntry* func_entry = tables->getFunction(func_name);
    assert(func_entry != NULL);
    if (func_entry->getType() != typeToString(types_Void)) {
        reg = reg_alloc->allocateRegister();
        stringstream s;
        s << "lw " << register_type_to_str(reg) << ", ($sp)";
        emit(s.str());
        emit("move $sp, $fp");
    }

    stringstream s;
    s << "addu $sp, $sp, " << size;
    emit(s.str());
    returnFromFunc(st);
    return reg;
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
    emit("TerminateZero: la $a0, DivisionByZero");
    emit("li $v0, 4");
    emit("syscall");
    emit("j ExitCode");
}

#endif