#ifndef __STACKVAL__
#define __STACKVAL__

#include <string>
#include <map>
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
	map<string,types> all_types;
	StackType(types type = types_Undefined, string id = "") : type(type), str(str), lineno(1) {
		//all_types[id] = types_Undefined;
	}
};

#define YYSTYPE StackType
#endif