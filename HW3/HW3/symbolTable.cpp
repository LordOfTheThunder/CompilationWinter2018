#include "symbolTable.hpp"
#include "StackStructs.h"
#include <assert.h>
#include "output.hpp"

symbolTable::symbolTable() : line(1), mainExists(false) {
    this->global = new Scope(this->line, false, true); // The last parameter indicates a global scope
    this->scopes.push_back(global);
    vector<types> print_args;
    print_args.push_back(types_String);
    vector<types> printi_args;
    printi_args.push_back(types_Int);
    this->addFunction(types_Void, string("print"), print_args);
    this->addFunction(types_Void, string("printi"), printi_args);
}

void symbolTable::addFunction(types retval, string id, vector<types> formals) {
    if (this->existsId(id)){
        // TODOBOM: handle this
        output::errorDef(this->lineno, id);
        exit(0)
    }

    if ((id.compare("main") == 0) &&
            (formals.size() == 0) &&
            (retval == types_Void)){
        this->mainExists = true;
    }

    if (!this->scopes.back()->isGlobal()){
        // TODO: handle declaration not in global scope
    }

    this->scopes.back()->addEntry(new FunctionEntry(formals, id, retval, this->getOffset()));
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
    for (vector<Scope*>::iterator it = scopes.begin(); it != scopes.end(); ++it){
        if ((*it)->existsId(id)){
            return true;
        }
    }
    return false;
}

bool symbolTable::existsVariable(string& id){
    for (vector<Scope*>::iterator it = scopes.begin(); it != scopes.end(); ++it){
        if ((*it)->existsVariable(id)){
            return true;
        }
    }
    return false;
}

bool symbolTable::existsFunction(string& id, vector<types> formals, types retval){
    FunctionEntry comp(formals, id, retval, 0);
    FunctionEntry * current = this->getFunction(id);

    if (current){
        return (comp == *current);
    }
    return false;
}

bool symbolTable::existsStruct(string& id, vector<types>& members){
    StructEntry comp(members, id, 0);
    StructEntry * current = this->getStruct(id);

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
    for (vector<Scope*>::iterator it = scopes.begin(); it != scopes.end(); ++it){
        if ((*it)->existsVariable(id)){
            return ((*it)->getVariable(id));
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
    return this->scopes.back()->getOffset();
}

bool symbolTable::isBreakAllowed(){
    assert(!this->scopes.empty());
    return this->scopes.back()->isWhile();
}

bool symbolTable::existsMain(){
    return this->mainExists;
}

void symbolTable::addStruct(string& id, vector<types>& members){
    if (this->existsId(id)){
        // TODOBOM: handle existing identifier
        output::errorDef(this->lineno, id);
        exit(0)
    }
    if (!this->scopes.back()->isGlobal()){
        // TODO: handle declaration not in global scope
    }

    this->scopes.back()->addEntry(new StructEntry(members, id, this->getOffset()));
}

void symbolTable::addVariable(types type, string id){
    if (this->existsId(id)){
        // TODOBOM: handle existing identifier
        output::errorDef(this->lineno, id);
        exit(0)
    }

    VariableEntry * res = new VariableEntry(type, id, this->getOffset());
    this->scopes.back()->addEntry(res);
}

void Scope::addEntry(TableEntry * ent){
    this->entries.push_back(ent);
}

bool Scope::existsId(string& id){
    for (vector<TableEntry*>::iterator it = this->entries.begin(); it != this->entries.end(); ++it){
        if ((*it)->getId().compare(id) == 0){
            return true;
        }
    }
    return false;
}