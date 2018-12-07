#pragma once
#include <string>
#include <vector>
#include "StackStructs.h"

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

class TableEntry{
private:
    string id;
    int offset;

public:
    TableEntry(string& id, int offset) : id(id), offset(offset){}
    string& getId(){return this->id;}
    virtual ~TableEntry() {}

};

class VariableEntry : public TableEntry{
private:
    string type;
    string id;

public:
    VariableEntry(string type, string& id, int offset) : TableEntry(id, offset), type(type){}
    string getType() const { return this->type; }
};

class StructEntry : public TableEntry{
private:
    int size;
    string id;
    vector<varPair> members;

public:
    StructEntry(vector<varPair> members, string id, int offset) : TableEntry(id, offset), members(members) {}

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
};

class FunctionEntry : public TableEntry{
private:
    vector<varPair> formals;
    string id;
    string ret;

public:
    FunctionEntry(vector<varPair> formals, string& id, string& ret, int offset) : TableEntry(id, offset), formals(formals), ret(ret) {}
    string getType(){return this->ret;}
    string& getId(){return this->id;}

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
    void removeEntry(){} // TODO
    bool existsId(string& id);
    VariableEntry * getVariable(string& id);
    bool existsVariable(string& id);
    TableEntry * getEntry(string& id);
    int getOffset() {return this->offset;}
    bool isWhile(){return this->isWhile_;}
    bool isGlobal(){return this->isGlobal_;}
};

class symbolTable{
private:
    vector<Scope *> scopes;
    Scope * global;
    int lineno;
    bool mainExists;

    Scope * newScope(bool isWhile){
        this->scopes.push_back(new Scope(this->getOffset(), isWhile));
        return this->scopes.back();
    }

public:
    symbolTable();
    ~symbolTable();
//    The following functions are scope-related functions (i.e. create a new scope)
    void addFunction(string retval, string id, vector<varPair> formals, bool addScope = true);
    void addWhile(int lineno);
    void addIf(int lineno);
    void addElse(int lineno);
    void addScope(int lineno);
    void popScope();

//    The following functions aren't scope-related (i.e. doesn't create a new scope)
    void addStruct(string& id, vector<varPair>& members);
    void addVariable(string type, string id);
    void addVariable(varPair v);

//    Existence checkers and validation
    bool existsId(string& id);
    bool existsVariable(string& id);
    bool existsFunction(string& id, vector<varPair> formals, string& retval);
    bool existsStruct(string& id, vector<varPair>& members);
    void existsMain();
    void isBreakAllowed();
    void isContinueAllowed();
    void validateByte(string& value);

//    Getters
    int getOffset();
    FunctionEntry * getFunction(string& id);
    VariableEntry * getVariable(string& id);
    StructEntry * getStruct(string& id);
    int getLine(){return this->lineno;}

//    Setters
    void setLine(int lineno){this->lineno = lineno;}


};