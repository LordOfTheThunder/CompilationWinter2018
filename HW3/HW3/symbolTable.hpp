#pragma once
#include <string>
#include <vector>
#include "StackStructs.h"

class StructMember{
private:
    types type;
    string id;

public:
    StructMember(types type, string& id) : type(type), id(id) {}
    types getType(){return this->type;}
    string getId(){return this->id;}
};

class Formal{
private:
    types type;
    string id;

public:
    Formal(types type, string& id) : type(type), id(id) {}
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
    types type;
    string id;

public:
    VariableEntry(types type, string& id, int offset) : TableEntry(id, offset), type(type){}

};

class StructEntry : public TableEntry{
private:
    int size;
    string id;
    vector<types> members;

public:
    StructEntry(vector<types> members, string id, int offset) : TableEntry(id, offset), members(members) {}

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
};

class FunctionEntry : public TableEntry{
private:
    vector<types> formals;
    string id;
    types ret;

public:
    FunctionEntry(vector<types> formals, string id, types ret, int offset) : TableEntry(id, offset), formals(formals), ret(ret) {}
    types getType(){return this->ret;}
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
};

class Scope{
private:
    vector<TableEntry *> entries;
    int offset;
    bool isWhile_;
    bool isGlobal_;

public:
    Scope(){}
    Scope(int offset, bool isWhile_, bool isGlobal_ = false) : offset(offset), isWhile_(isWhile_), isGlobal_(isGlobal_){}
    void addEntry(TableEntry * ent);
    void removeEntry(){} // TODO
    bool existsId(string& id);
    VariableEntry * getVariable(string& id){} // TODO
    bool existsVariable(string& id){} // TODO
    TableEntry * getEntry(string& id){} // TODO
    int getOffset() {return this->offset;}
    bool isWhile(){return this->isWhile_;}
    bool isGlobal(){return this->isGlobal_;}
};

class symbolTable{
private:
    vector<Scope *> scopes;
    Scope * global;
    int line;
    bool mainExists;

    void newScope(bool isWhile){
        this->scopes.push_back(new Scope(this->getOffset(), isWhile));
    }

public:
    symbolTable();
//    The following functions are scope-related functions (i.e. create a new scope)
    void addFunction(types retval, string id, vector<types> formals);
    void addWhile(int line);
    void addIf();
    void addElse();
    void addScope();

//    The following functions aren't scope-related (i.e. doesn't create a new scope)
    void addStruct(string& id, vector<types>& members);
    void addVariable(types type, string id);

//    Existence checkers and validation
    bool existsId(string& id);
    bool existsVariable(string& id);
    bool existsFunction(string& id, vector<types> formals, types retval);
    bool existsStruct(string& id, vector<types>& members);
    bool existsMain();
    bool isBreakAllowed();

//    Getters
    int getOffset();
    FunctionEntry * getFunction(string& id);
    VariableEntry * getVariable(string& id);
    StructEntry * getStruct(string& id);
    int getLine(){return this->line;}

//    Setters
    void setLine(int line){this->line = line;}
};