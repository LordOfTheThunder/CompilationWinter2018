#ifndef __STACKVAL__
#define __STACKVAL__

#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

enum types {
	types_Int,
	types_Byte,
	types_Bool,
	types_Void,
	types_String,
	types_Undefined
};

class varPair{
public:
    string type;
    string id;

    varPair(){}
    varPair(string type, string id) : type(type), id(id) {}

    bool operator==(const varPair & rhs) const {
        return ((this->type.compare(rhs.type)) == 0);
    }

    bool operator!=(const varPair & rhs) const {
        return (!(rhs == *this));
    }
};

static bool isPrimitive(string& type){
    // TODO: implement
    return ((type.compare(string("INT")) == 0) ||
            (type.compare(string("BYTE")) == 0) ||
            (type.compare(string("BOOL")) == 0) ||
            (type.compare(string("VOID")) == 0) ||
            (type.compare(string("STRING")) == 0));
}

static string typeToString(types type){
	if (type == types_Int) return string("INT");
	if (type == types_Byte) return string("BYTE");
	if (type == types_Bool) return string("BOOL");
	if (type == types_Void) return string("VOID");
	if (type == types_String) return string("STRING");
	if (type == types_Undefined) return string("undefined");
	else return string("None");
}

static types stringToType(string type){
	if (type == "INT") return types_Int;
	if (type == "BYTE") return types_Byte;
	if (type == "BOOL") return types_Bool;
	if (type == "VOID") return types_Void;
	if (type == "STRING") return types_String;
	return types_Undefined;
}

struct StackType {
	// For error reports
	int lineno;
	// Information in the stack
	string str;
	types type;
	vector<varPair> func_info;
	types ret_type;
	StackType(types type = types_Undefined, string str = "") : type(type), str(str), lineno(1), func_info() {
		ret_type = types_Undefined;
	}
};

#define YYSTYPE StackType
#endif