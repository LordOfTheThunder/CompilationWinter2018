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

static string typeToString(types type){
	if (type == types_Int) return string("int");
	if (type == types_Byte) return string("byte");
	if (type == types_Bool) return string("bool");
	if (type == types_Void) return string("void");
	if (type == types_String) return string("string");
	if (type == types_Undefined) return string("undefined");
	else return string("None");
}

struct StackType {
	// For error reports
	int lineno;
	// Information in the stack
	string str;
	types type;
	vector<types> func_types;
	StackType(types type = types_Undefined, string str = "") : type(type), str(str), lineno(1), func_types() {
	}
};

#define YYSTYPE StackType
#endif