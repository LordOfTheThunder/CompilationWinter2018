
#ifndef __SYMBOLTABLE__
#define __SYMBOLTABLE__

#include <string>
#include <vector>
#include "StackStructs.h"
using namespace std;

class StructMember{
private:
    types type;
    string id;

public:
    StructMember(types type_, string id_) : type(type_), id(id_) {}
    types getType() const {
        return this->type;
    }
    string getId() const {
        return this->id;
    }


};

class Formal{
private:
    types type;
    string id;

public:
    Formal(types type_, string id_) : type(type_), id(id_) {}
};

class TableEntry{
private:
    string id;
    int offset;

public:
    TableEntry(string id, int offset) : id(id), offset(offset){}
    string getId() const {
        return this->id;
    }
};

class VariableEntry : public TableEntry{
private:
    types type;
    string id;

public:
    VariableEntry(types type, string& id) : type(type), id(id){}
};

class StructEntry : public TableEntry{
private:
    int size;
    string id;
    vector<types> members;
    vector<types> formals;

public:
    StructEntry(vector<types> members, string id);

    bool operator==(const StructEntry& rhs) const {
        if (rhs.getId().compare(this->id) != 0){
            return false;
        }
        if (rhs.members.size() != this->members.size()){
            return false;
        }

        for (int i = 0; i < rhs.formals.size(); ++i) {
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
    FunctionEntry(vector<Formal> formals, string id, types ret);
    types getType() const  {
        return this->ret;
    }
    string getId() const {
        return this->id;
    }
>>>>>>> Stashed changes

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
    bool is_while;
    bool is_global;

public:
    Scope(bool isWhile_, int offset_ = 0) : offset(offset_), is_while(isWhile_){}; // TODO
    void addEntry(TableEntry * ent); // TODO
    void removeEntry(); // TODO
    bool existsId(string& id) const; // TODO
    VariableEntry& getVariable(string& id); // TODO
    bool existsVariable(string& id); // TODO
    TableEntry * getEntry(string& id); // TODO
    int getOffset() const; // TODO
    bool isWhile() const {
        return this->is_while;
    } // TODO
    bool isGlobal() const {
        return this->is_global;
    }
};

class symbolTable{
private:
    vector<Scope> scopes;
    Scope * global;
    bool mainExists;

    void newScope(bool isWhile){
        this->scopes.push_back(new Scope(this->getOffset(), isWhile));
    }

public:
    int line;
    symbolTable();
//    The following functions are scope-related functions (i.e. create a new scope)
    void addFunction(types retval, string id, vector<types> formals);
    void addWhile();
    void addIf();
    void addElse();
    void addScope();

//    The following functions aren't scope-related (i.e. doesn't create a new scope)
    void addStruct(string& id, vector<types>& members);
    void addVariable(types type, string& id);
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

#endif
