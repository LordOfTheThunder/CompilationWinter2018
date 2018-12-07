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