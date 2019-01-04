#ifndef __ATTRIBUTES_H
#define __ATTRIBUTES_H

#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>

typedef int tempType;
const tempType NO_TEMP = -1;
#ifndef NULL
#define NULL 0
#endif // !NULL

using std::string;
using std::vector;


enum types
{
	IntType,
	ByteType,
	BoolType,
	VoidType,
	StringType
};

struct typeStruct
{
	types type;
	int arr_size;
	int offset;
	typeStruct(types t = VoidType, int size = 0, int offset = 0) : type(t), arr_size(size), offset(offset) {}
	bool operator==(const typeStruct& rhs) const{
		return type == rhs.type && arr_size == rhs.arr_size;
	}
	bool operator!=(const typeStruct& rhs) const {
		return !(*this == rhs);
	}
};

struct Stype{
private:
	vector<int> l1;
  	vector<int> l2;
public:
	types type;
	string str;
	vector<string> vec_ids;
	vector<typeStruct> vec_types;
	int arr_size;
	int lineno;
	int offset;
	tempType temp;
	
	Stype(string str = "", types type = VoidType) : type(type), str(str), vec_ids(),  vec_types(), arr_size(0), lineno(1), offset(0), temp(NO_TEMP) {}
	typeStruct getTypeStruct(){
		return typeStruct(type, arr_size, offset);
	}

	vector<int>& getNextlist(){
		return l1;
	}
	vector<int>& getBreaklist(){
		return l2;
	}
	vector<int>& getTruelist(){
		assert(type == BoolType);
		return l1;
	}
	vector<int>& getFalselist(){
		assert(type == BoolType);
		return l2;
	}
	
};




#define YYSTYPE Stype	// Tell Bison to use Stype as the stack type

#endif
