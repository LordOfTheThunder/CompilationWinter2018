#pragma once
#include <string>
#include <vector>
#include "StackStructs.h"

class StructMember{
private:
    types type;
    string id;

public:
    StructMember(type_, id_) : type(type_), id(id_) {}
    types getType(){
        return this->type
    }
    string getId(){
        return this->id;
    }


};

class Formal{
private:
    types type;
    string id;

public:
    Formal(type_, id_) : type(type_), id(id_) {}
};

class TableEntry{
private:
    string id;
    int offset;

public:
    TableEntry(string id, int offset) : id(id), offset(offset){}
    string getId(return this->id;)
};

class VariableEntry : public TableEntry{
private:
    types type;
    string id;
};

class StructEntry : public TableEntry{
private:
    int size;
    string id
    vector<types> members;

public:
    StructEntry(vector<types> members, string id);

    bool operator==(const StructEntry& rhs) const {
        if (rhs.id.compare(this->id) != 0){
            return false;
        }
        if (rhs.members.size() != this->members.size()){
            return false;
        }
        for (int i = 0; i < rhs.formals.size(); i++){
            if (rhs.formals[i] != this->formals[i])
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
    FunctionEntry(vector<Formal> types, string id, types ret);
    types getType(){return this->ret;}
    string getId(return this->id;)

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
    int offset
    bool isWhile;
    bool isGlobal;

public:
    Scope(int offset, bool isWhile, bool isGlobal = false) : offset(offset), isWhile(isWhile), isGlobal(isGlobal){}; // TODO
    void addEntry(TableEntry * ent); // TODO
    void removeEntry(); // TODO
    bool existsId(string& id); // TODO
    VariableEntry& getVariable(string& id) // TODO
    bool existsVariable(string& id); // TODO
    TableEntry * getEntry(string& id); // TODO
    int getOffset(); // TODO
    bool isWhile(){return this->isWhile;} // TODO
    bool isGlobal(){return this->isGlobal;}
};

class symbolTable{
private:
    vector<Scope> scopes;
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
    void addWhile();
    void addIf();
    void addElse();
    void addScope();

//    The following functions aren't scope-related (i.e. doesn't create a new scope)
    void addStruct(string& id, vector<types>& members);
    void addVariable(types type, string id); // TODO

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
};