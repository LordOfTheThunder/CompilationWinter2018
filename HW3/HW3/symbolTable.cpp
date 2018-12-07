#include "symbolTable.hpp"
#include "StackStructs.h"
#include <assert.h>
#include "output.hpp"
#include <stdlib.h>

template <class T>
void clearVectorOfPointers(vector<T>& v){
    for (typename vector<T>::iterator it = v.begin(); it != v.end(); ++it){
        delete (*it);
    }

    v.clear();
}

symbolTable::~symbolTable() {
    clearVectorOfPointers(this->scopes);
}

Scope::~Scope(){
    clearVectorOfPointers(this->entries);
}

symbolTable::symbolTable() : lineno(1), mainExists(false) {
    this->global = new Scope(this->lineno, false, true); // The last parameter indicates a global scope
    this->scopes.push_back(global);
    vector<varPair> print_args;
    print_args.push_back(varPair(typeToString(types_String), string("")));
    vector<varPair> printi_args;
    printi_args.push_back(varPair(typeToString(types_Int), string("")));
    this->addFunction(typeToString(types_Void), string("print"), print_args, false);
    this->addFunction(typeToString(types_Void), string("printi"), printi_args, false);
}

void symbolTable::addFunction(string retval, string id, vector<varPair> formals, bool addScope) {
    if (this->existsId(id)){
        // TODOBOM: handle this
        output::errorDef(this->lineno, id);
        exit(0);
    }

    if ((id.compare("main") == 0) &&
            (formals.size() == 0) &&
            (retval.compare(typeToString(types_Void)) == 0)){
        this->mainExists = true;
    }

    if (!this->scopes.back()->isGlobal()){
        // TODO: handle declaration not in global scope
    }
    this->scopes.back()->addEntry(new FunctionEntry(formals, id, retval, this->getOffset()));
    if (addScope){
        Scope * funcScope = newScope(false);
        //    Adding arguments to the function's scope
        for (vector<varPair>::iterator it = formals.begin(); it != formals.end(); ++it) {
            this->addVariable(*it);
        }
    }
}

void symbolTable::addWhile(int lineno){
    this->lineno = lineno;
    newScope(true);
}

void symbolTable::addIf(int lineno){
    this->lineno = lineno;
    newScope(false);
}

void symbolTable::addElse(int lineno){
    this->lineno = lineno;
    newScope(false);
}

void symbolTable::addScope(int lineno){
    this->lineno = lineno;
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

bool symbolTable::existsFunction(string& id, vector<varPair> formals, string& retval){
    FunctionEntry comp(formals, id, retval, 0);
    FunctionEntry * current = this->getFunction(id);

    if (current){
        return (comp == *current);
    }
    return false;
}

bool symbolTable::existsStruct(string& id, vector<varPair>& members){
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

void symbolTable::popScope(){
    if (this->scopes.empty()){
        return;
    }

    delete this->scopes.back();
    this->scopes.pop_back();
}

void symbolTable::isBreakAllowed(){
    assert(!this->scopes.empty());
    if (!(this->scopes.back()->isWhile())){
        output::errorUnexpectedBreak(this->lineno);
    }
}

void symbolTable::isContinueAllowed(){
    assert(!this->scopes.empty());
    if (!(this->scopes.back()->isWhile())){
        output::errorUnexpectedContinue(this->lineno);
    }
}

void symbolTable::existsMain(){
    if (!this->mainExists){
        output::errorMainMissing();
    }
}

void symbolTable::addStruct(string& id, vector<varPair>& members){
    if (this->existsId(id)){
        // TODOBOM: handle existing identifier
        output::errorDef(this->lineno, id);
        exit(0);
    }

    this->scopes.back()->addEntry(new StructEntry(members, id, this->getOffset()));
}

void symbolTable::addVariable(string type, string id){
    if (this->existsId(id)){
        // TODOBOM: handle existing identifier
        output::errorDef(this->lineno, id);
        exit(0);
    }

    VariableEntry * res = new VariableEntry(type, id, this->getOffset());
    this->scopes.back()->addEntry(res);
}

void symbolTable::addVariable(varPair v){
    this->addVariable(v.type, v.id);
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

VariableEntry * Scope::getVariable(string& id){
    VariableEntry * res = dynamic_cast<VariableEntry*>(this->getEntry(id));
    return res;
}

bool Scope::existsVariable(string& id){
    VariableEntry * res = this->getVariable(id);
    return (res != NULL);

}

TableEntry * Scope::getEntry(string& id){
    for (vector<TableEntry*>::iterator it = this->entries.begin(); it != this->entries.end(); ++it){
        if ((*it)->getId().compare(id) == 0){
            return (*it);
        }
    }

    return NULL;
}

void symbolTable::validateByte(string& value){
    int value_int = atoi(value.c_str());
    if (value_int > 255 || value_int < 0){
        output::errorByteTooLarge(this->lineno, string(value));
    }
}