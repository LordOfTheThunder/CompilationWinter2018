#pragma once
#include <string>
#include <vector>
#include "StackStructs.h"
#include "output.hpp"
#include "bp.hpp"

#define DEBUGGING 0
#define emit(s) CodeBuffer::instance().emit(s)

class TableEntry{

protected:
    string id;
    int offset;

public:
    TableEntry(string& id, int offset) : id(id), offset(offset) {}
    string& getId(){return this->id;}
    int getOffset(){return this->offset;}
    virtual void print() {}
    virtual ~TableEntry() {}
};

class VariableEntry : public TableEntry{
private:
    string type;

public:
    VariableEntry(string& type, string& id, int offset) : TableEntry(id, offset), type(type) {}
    string& getType(){return type;}

    void print(){
        if (isPrimitive(type)){
            output::printID(id, offset, type);
        } else{
            output::printID(id, offset, "struct " + type);
        }
    }
};

class StructEntry : public TableEntry{
private:
    vector<varPair> members;
    vector<string> types;
    vector<string> names;

public:
    StructEntry(vector<varPair> members, string id, int offset) : TableEntry(id, offset), members(members)
    {
        for (vector<varPair>::iterator it = members.begin(); it != members.end(); ++it){
            types.push_back((*it).type);
            names.push_back((*it).id);
        }
    }

    int size(){return members.size();}

    void print() {
        output::printStructType(id, types, names);
    }

    bool operator==(const StructEntry& rhs) const {
        if (rhs.id.compare(this->id) != 0){
            return false;
        }
        if (rhs.members.size() != this->members.size()){
            return false;
        }
        for (int i = 0; i < rhs.members.size(); i++){
            if (rhs.members[i] != this->members[i])
                return false;
        }

        return true;
    }

    bool operator!=(const StructEntry& rhs) const {
        return (!(rhs == *this));
    }

    bool varExists(string& id) {
        for (vector<varPair>::iterator it = members.begin(); it != members.end(); ++it){
            if (it->id == id) {
                return true;
            }
        }
        return false;
    }
    string getTypeOfVar(string& id) {
        if (!varExists(id)) {
            return string("None");
        }
        for (vector<varPair>::iterator it = members.begin(); it != members.end(); ++it){
            if (it->id == id) {
                return it->type;
            }
        }
    }
};

class FunctionEntry : public TableEntry{
private:
    vector<varPair> formals;
    vector<string> args;
    string type;

public:
    FunctionEntry(vector<varPair> formals, string& id, string& type) : TableEntry(id, 0), formals(formals), type(type)
    {
        for (vector<varPair>::iterator it = formals.begin(); it != formals.end(); ++it){
            if (isPrimitive((*it).type)){
                args.push_back((*it).type);
            } else {
                args.push_back("struct " + (*it).type);
            }
        }

        if (DEBUGGING) cout << id << " " << offset << endl;
    }
    string getType(){return this->type;}
    string& getId(){return this->id;}

    vector<string>& getArgs() {return args;}
    void print(){
        output::printID(id, offset, output::makeFunctionType(type, args));
    }

    bool matchArgs(vector<varPair>& rhs){
        if (rhs.size() != args.size()){
            return false;
        }

        for (int i = 0; i < args.size(); i++){
//            if (formals[i] != rhs[i]){
//                if ((formals[i].type.compare(typeToString(types_Int)) == 0) &&
//                    (rhs[i].type.compare(typeToString(types_Byte)) == 0)){
//                     Allowing byte to int casting
//                    continue;
//                }
//                return false;
//            }
            if (!typesEqual(formals[i].type, rhs[i].type)){
                return false;
            }
        }
        return true;
    }
    bool matchType(string& type){
        return typesEqual(this->type, type);
    }
    bool operator==(const FunctionEntry& rhs) const {
        if (rhs.id.compare(this->id) != 0){
            return false;
        }
        if (rhs.formals.size() != this->formals.size()){
            return false;
        }
        for (int i = 0; i < rhs.formals.size(); i++){
            if (rhs.formals[i] != this->formals[i])
                return false;
        }

        return true;
    }

    bool operator!=(const FunctionEntry& rhs) const {
        return (!(rhs == *this));
    }
};

class Scope{
private:
    vector<TableEntry *> entries;
    int offset;
    bool isWhile_;
    bool isGlobal_;

public:
    Scope(){}
    ~Scope();
    Scope(int offset, bool isWhile_, bool isGlobal_ = false) : offset(offset), isWhile_(isWhile_), isGlobal_(isGlobal_){}
    void addEntry(TableEntry * ent);
    bool existsId(string& id);
    VariableEntry * getVariable(string& id);
    bool existsVariable(string& id);
    TableEntry * getEntry(string& id);
    int getOffset() {return this->offset;}
    void incrementOffset(int delta){offset += delta;}
    bool isWhile(){return this->isWhile_;}
    bool isGlobal(){return this->isGlobal_;}
    void updateOffset(int newOffset){this->offset = newOffset;}
    TableEntry * getLastEntry() {
        if (entries.empty()){
            return NULL;
        }
        return entries.back();
    }
};

class symbolTable{
private:
    vector<Scope *> scopes;
    Scope * global;
    int lineno;
    bool mainExists;

    Scope * newScope(bool isWhile, bool initOffset = false){
        if (initOffset){
            if (DEBUGGING) cout << "adding scope with offset " << 0 << endl;
            this->scopes.push_back(new Scope(0, isWhile));
        } else {
            if (DEBUGGING) cout << "adding scope with offset " << getOffset() << endl;
            this->scopes.push_back(new Scope(this->getOffset(), isWhile));
        }
        return this->scopes.back();
    }

public:
    symbolTable();
    ~symbolTable();
//    The following functions are scope-related functions (i.e. create a new scope)
    void addFunction(string retval, string id, vector<varPair> formals, int lineno = 0, bool addScope = true);
    void addWhile();
    void addIf();
    void addElse();
    void addScope();
    void popScope();

//    The following functions aren't scope-related (i.e. doesn't create a new scope)
    void addStruct(string& id, vector<varPair>& members, int lineno);
    void addVariable(string type, string id, int lineno);
    void addFunctionArgument(string& type, string& id, int offset, int lineno);
    void addVariable(varPair v, int lineno);

//    Existence checkers and validation
    bool existsId(string& id);
    bool existsVariable(string& id);
    bool existsFunction(string& id, vector<varPair>& formals, string& retval);
    bool existsStruct(string& id, vector<varPair>& members);
    void existsMain();
    void isBreakAllowed(int lineno);
    void isContinueAllowed(int lineno);
    void validateByte(string& value);
    void checkReturn(string type, int lineno);

//    Getters
    void getFreedom(){
        cout << "Freedom is for those Willing to Defend it" << endl;
    }
    int getOffset();
    TableEntry * getEntry(string& id);
    FunctionEntry * getFunction(string& id);
    VariableEntry * getVariable(string& id);
    StructEntry * getStruct(string& id);
    int getLine(){return this->lineno;}
    int countSize(vector<varPair>& types);
    int getVariableSize(string& type);

//    Strange API utils
    void callFunction(string& id, vector<varPair>& args, int lineno);

//    Setters
    void setLine(int lineno){this->lineno = lineno;}


};