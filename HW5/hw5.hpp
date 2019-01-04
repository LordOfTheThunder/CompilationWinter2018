#include <cassert>
#include "StackStructs.h"

#define DEBUG

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

class RegisterAllocation {
    vector<register_type> available;
    vector<register_type> used;
public:
    RegisterAllocation() {
        for (int i = t0; i < t9; ++i) {
            available.push_back(i);
        }
    }

    register_type allocateRegister() {
        assert(!available.empty());
        register_type res = available[0];
        used.push_back(res);
        available.erase(available.begin());
        #ifdef DEBUG
            cout << "Allocated register " << register_type_to_str(res) << endl;
        #endif
        return res;
    }

    void freeRegister(register_type reg) {
        if (used.find(reg) != used.end()) {
            return;
        }
        assert(used.count(reg) == 1);
        used.erase(used.find(reg));
        available.push_back(reg);
        #ifdef DEBUG
            cout << "Freed register " << register_type_to_str(reg) << endl;
        #endif
    }
}