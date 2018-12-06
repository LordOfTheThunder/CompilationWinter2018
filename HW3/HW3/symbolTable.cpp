#include "symbolTable.hpp"
#include "StackStructs.h"

symbolTable::symbolTable() : line(1), mainExists(false) {
    this->global = new Scope(false);
    this->scopes.push_back(global);
    vector<types> print_args;
    print_args.push_back(types_String);
    vector<types> printi_args;
    printi_args.push_back(types_Int);
    this->addFunction(types.types_Void, "print", print_args)
    this->addFunction(types.types_Void, "printi", printi_args)
}

void symbolTable::addFunction(types retval, string& id, vector<types> formals) {
    existsId(id);

    if ((id.compare("main") == 0) &&
            (formals.size() == 0) &&
            (retval == types_Void)){
        this->mainExists = true;
    }

    this->scopes.back().addEntry(new FunctionEntry(formals, id, retval))
    newScope(false);
}

void symbolTable::addWhile(int line){
    this->line = line;
    newScope(true);
}

void symbolTable::addIf(){
    this->line = line;
    newScope(false);
}

void symbolTable::addElse(){
    this->line = line;
    newScope(false);
}

void symbolTable::addScope(){
    this->line = line;
    newScope(false);
}

bool symbolTable::existsId(string& id){
    for (vector<Scope>::iterator it = scopes.begin(); it != scopes.end(); ++it){
        if ((*it).existsId(id)){
            return true;
        }
    }
    return false;
}

bool symbolTable::existsVariable(string& id){
    for (vector<Scope>::iterator it = scopes.begin(); it != scopes.end(); ++it){
        if ((*it).existsVariable(id)){
            return true;
        }
    }
    return false;
}

bool symbolTable::existsFunction(string& id, vector<types> formals, types retval){
    FunctionEntry comp(formals, id, retval);
    FunctionEntry * current = this->getFunction(id);

    if (current){
        return (comp == *current);
    }
    return false;
}

bool symbolTable::existsStruct(string& id, vector<types>& members){
    StructEntry comp(members, id);
    FunctionEntry * current = this->getStruct(id);

    if (current){
        return (comp == *current);
    }
    return false;
}

FunctionEntry * symbolTable::getFunction(string& id){
    if (!this->global){
        return NULL;
    }
    FunctionEntry * res = dynamic_cast<FunctionEntry*>(this->global->getEntry(id));
    return res;
}

VariableEntry * symbolTable::getVariable(string& id){
    assert(!this->scopes.empty());
    for (vector<Scope>::iterator it = scopes.begin(); it != scopes.end(); ++it){
        if ((*it).existsVariable(id)){
            return ((*it).getVariable(id));
        }
    }
    return NULL;
}

StructEntry * symbolTable::getStruct(string& id){
    if (!this->global){
        return NULL;
    }
    StructEntry * res = dynamic_cast<StructEntry*>(this->global->getEntry(id));
    return res;
}

int symbolTable::getOffset(){
    assert(!this->scopes.empty());
    return this->scopes.back().getOffset();
}

bool symbolTable::isBreakAllowed(){
    assert(!this->scopes.empty());
    return this->scopes.back().isWhile();
}

bool symbolTable::existsMain(){
    return this->mainExists;
}