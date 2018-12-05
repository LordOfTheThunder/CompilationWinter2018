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
    vector<StructMember> members;

public:
    StructEntry(vector<StructMember> members, string id, int offset);
};

class FunctionEntry : public TableEntry{
private:
    vector<Formal> formals;
    string id;
    types ret;

public:
    FunctionEntry(vector<Formal> formals, string id, types ret);
    types getType(){return this->ret;}
    string getId(return this->id;)
};

class Scope{
private:
    vector<TableEntry *> entries;
    int offset
    bool isWhile;

public:
    Scope(int offset_, bool isWhile_) : offset(offset_), isWhile(isWhile_){};
    void addEntry(TableEntry * ent);
    void removeEntry();
    bool containsId(string id);
    TableEntry& getEntry(string id);
    int getOffset();
    bool isWhile(){return this->isWhile;}
};

class symbolTable{
private:
    vector<Scope> scopes;
    int line;
    bool mainExists;

    void newScope(bool isWhile){
        this->scopes.push_back(new Scope(this->getOffset(), isWhile));
    }

public:
    symbolTable();
//    The following functions are scope-related functions (i.e. create a new scope)
    void addFunction(types retval, string id, vector<types> formals, bool addScope);
    void addWhile();
    void addIf();
    void addElse();
    void addScope();

//    The following functions aren't scope-related (i.e. doesn't create a new scope)
    void addStruct(string id, vector<StructMember> members); // TODO
    void addVariable(types type, string id); // TODO

//    Existence checkers and validation
    bool validateId(string& id); // TODO
    bool existsVariable(string& id) // TODO
    bool existsFunction(string& id); // TODO
    bool existsFunction(string& id); // TODO
    bool existsStruct(string& id); // TODO
    bool existsMain(); // TODO
    bool isBreakAllowed(); // TODO

//    Getters
    int getOffset(); // TODO
    FunctionEntry& getFunction(string& id); // TODO
    VariableEntry& getVariable(string& id); // TODO
    StructEntry& getStruct(string& id); // TODO
};