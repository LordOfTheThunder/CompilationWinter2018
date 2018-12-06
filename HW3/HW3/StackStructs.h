#ifndef __STACKVAL__
#define __STACKVAL__

#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

enum types {
	types_Int,
	types_Byte,
	types_Bool,
	types_Void,
	types_String
};

struct StackType {
	// For error reports
	int lineno;
	// Information in the stack
	string str;
	types type;
	StackType(types type = types_Void, string str = "") : type(type), str(str), lineno(1) {

	}
};

#define YYSTYPE StackType
#endif