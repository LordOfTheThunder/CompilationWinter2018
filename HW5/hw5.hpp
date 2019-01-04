#ifndef __HW5__
#define __HW5__

#include "StackStructs.h"
#include "bp.hpp"
#include <cassert>
#include <algorithm>
#include <set>
#include <sstream>


#define DEBUG
#define emit(s) CodeBuffer::instance().emit(s)

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

enum arithmetic_op {
    add_op,
    sub_op,
    mul_op,
    div_op
};

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
            cout << "Allocated register " << register_type_to_str(res) << endl;
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
            cout << "Freed register " << register_type_to_str(reg) << endl;
        #endif
    }
};

RegisterAllocation *reg_alloc;

void allocateVar(register_type reg) {
    emit("subu $sp, $sp, 4");
    stringstream s;
    s << "sw " << register_type_to_str(reg) << ", ($sp)";
    emit(s.str());
}

void assignToVar(register_type to_assign) {
	register_type reg = reg_alloc->allocateRegister();
	// MAX TODO: do assignment code
	reg_alloc->freeRegister(reg);
}

void arithmetic_op_between_regs(register_type first, register_type second, arithmetic_op op) {
    string op_str = op_to_string(op);
}

#endif