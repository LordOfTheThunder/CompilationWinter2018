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
    output::endScope();

    for (vector<TableEntry*>::iterator it = this->entries.begin(); it != this->entries.end(); ++it){
        StructEntry * res = dynamic_cast<StructEntry*>(*it);
        if (!res){
            (*it)->print();
        }
    }
    for (vector<TableEntry*>::iterator it = this->entries.begin(); it != this->entries.end(); ++it){
        StructEntry * res = dynamic_cast<StructEntry*>(*it);
        if (res){
            (*it)->print();
        }
    }

    clearVectorOfPointers(this->entries);
}

symbolTable::symbolTable() : lineno(0), mainExists(false) {
    this->global = new Scope(0, false, true); // The last parameter indicates a global scope
    this->scopes.push_back(global);
    vector<varPair> print_args;
    print_args.push_back(varPair(typeToString(types_String), string("")));
    vector<varPair> printi_args;
    printi_args.push_back(varPair(typeToString(types_Int), string("")));

    this->addFunction(typeToString(types_Void), string("print"), print_args, this->lineno, false);
    this->addFunction(typeToString(types_Void), string("printi"), printi_args, this->lineno, false);
}

void symbolTable::addFunction(string retval, string id, vector<varPair> formals, int lineno, bool addScope) {
    if (DEBUGGING){
        cout << "add function " << id << ", args: " << formals.size() << endl;
        for (vector<varPair>::iterator it = formals.begin(); it!=formals.end(); ++it){
            cout << "argument " << (*it).type << " " << (*it).id << endl;
        }
    }
    int argOffset = 0;
    this->lineno = lineno;
    if (this->existsId(id)){
        output::errorDef(this->lineno, id);
        exit(0);
    }

    if ((id.compare("main") == 0) &&
            (formals.size() == 0) &&
            (retval.compare(typeToString(types_Void)) == 0)){
        this->mainExists = true;
    }

    this->scopes.back()->addEntry(new FunctionEntry(formals, id, retval));

    if (addScope){
        Scope * funcScope = newScope(false, true);

        //    Adding arguments to the function's scope
        for (vector<varPair>::iterator it = formals.begin(); it != formals.end(); ++it) {
            argOffset -= getVariableSize((*it).type);
            this->addFunctionArgument((*it).type, (*it).id, argOffset, (*it).lineno);
        }
    }
}

void symbolTable::addFunctionArgument(string& type, string& id, int offset, int lineno){
    if (DEBUGGING) cout << "add arg: " << type << " " << id << endl;
    this->lineno = lineno;

    if (this->existsId(id)){
        // TODOBOM: handle existing identifier
        output::errorDef(this->lineno, id);
        exit(0);
    }

    if (!isPrimitive(type)){
        if (!this->getStruct(type)){
            output::errorUndefStruct(this->lineno, type);
            exit(0);
        }
    }

    VariableEntry * res = new VariableEntry(type, id, offset);
    this->scopes.back()->addEntry(res);
}

int symbolTable::countSize(vector<varPair>& types){
    int size = 0;
    for (vector<varPair>::iterator it = types.begin(); it != types.end(); ++it){
        int var_size = getVariableSize((*it).type);
        if (var_size == -1) return -1;
        size += var_size;
    }
    return size;
}

int symbolTable::getVariableSize(string& type){
    if (isPrimitive(type)){
        return 1;
    }
    else if (getStruct(type)){
        return getStruct(type)->size();
    }
    else {
        return -1;
    }
}

void symbolTable::addWhile(){
    newScope(true);
}

void symbolTable::addIf(){
    newScope(false);
}

void symbolTable::addElse(){
    newScope(false);
}

void symbolTable::addScope(){
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

bool symbolTable::existsFunction(string& id, vector<varPair>& formals, string& retval){
    FunctionEntry comp(formals, id, retval);
    FunctionEntry * current = this->getFunction(id);

    if (current){
        return (comp == *current);
    }
    return false;
}

void symbolTable::callFunction(string& id, vector<varPair>& args, int lineno){
    if (DEBUGGING) cout << "called function: " << id << endl;
    this->lineno = lineno;
    FunctionEntry * res = getFunction(id);

    if (!res){
        output::errorUndefFunc(this->lineno, id);
        exit(0);
    }
    if (!res->matchArgs(args)){
        output::errorPrototypeMismatch(this->lineno, id, res->getArgs());
        exit(0);
    }
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
    assert(!this->scopes.empty());
    for (vector<Scope*>::iterator it = scopes.begin(); it != scopes.end(); ++it){
        StructEntry * res = dynamic_cast<StructEntry*>((*it)->getEntry(id));
        if (res){
            return (res);
        }
    }
    return NULL;
}

int symbolTable::getOffset(){
    assert(!this->scopes.empty());
    return this->scopes.back()->getOffset();
}

void symbolTable::popScope(){
    if (this->scopes.empty()){
        return;
    }
    // TODO: clean code - remove unnecessary code
//    Scope * current = scopes.back();
//
//    if (!current->isGlobal()){
//        this->scopes.pop_back();
//        scopes.back()->updateOffset(current->getOffset());
//        delete current;
//        return;
//    }
//
//    delete current;
//    this->scopes.pop_back();

    Scope * current = scopes.back();
    delete current;
    this->scopes.pop_back();
}

void symbolTable::isBreakAllowed(int lineno){
    assert(!this->scopes.empty());
    this->lineno = lineno;

    for (vector<Scope*>::reverse_iterator it = scopes.rbegin(); it != scopes.rend(); ++it){
        if ((*it)->isWhile()){
            return;
        }
    }
    output::errorUnexpectedBreak(this->lineno);
    exit(0);
}

void symbolTable::isContinueAllowed(int lineno){
    assert(!this->scopes.empty());
    this->lineno = lineno;

    for (vector<Scope*>::reverse_iterator it = scopes.rbegin(); it != scopes.rend(); ++it){
        if ((*it)->isWhile()){
            return;
        }
    }
    output::errorUnexpectedContinue(this->lineno);
    exit(0);
}

void symbolTable::existsMain(){
    if (!this->mainExists){
        output::errorMainMissing();
        exit(0);
    }
}

void symbolTable::addStruct(string& id, vector<varPair>& members, int lineno){
    if (DEBUGGING) cout << "add struct " << id << endl;
    this->lineno = lineno;
    if (this->existsId(id)){
        // TODOBOM: handle existing identifier
        output::errorDef(this->lineno, id);
        exit(0);
    }

    vector<string> tmp;

    for (vector<varPair>::iterator it = members.begin(); it != members.end(); ++it){
        // Check if already defined
        for (vector<string>::iterator in_it = tmp.begin(); in_it != tmp.end(); ++in_it) {
            if ((*in_it).compare((*it).id) == 0) {
                output::errorDef((*it).lineno, (*it).id);
                exit(0);
            }
        }
        tmp.push_back((*it).id);
    }

    this->scopes.back()->addEntry(new StructEntry(members, id, this->getOffset()));
}

void symbolTable::addVariable(string type, string id, int lineno){
    if (DEBUGGING) cout << "add var: " << type << " " << id << endl;
    this->lineno = lineno;

    if (this->existsId(id)){
        // TODOBOM: handle existing identifier
        output::errorDef(this->lineno, id);
        exit(0);
    }

    if (!isPrimitive(type)){
        if (!this->getStruct(type)){
            output::errorUndefStruct(this->lineno, type);
            exit(0);
        }
    }

    VariableEntry * res = new VariableEntry(type, id, this->getOffset());
    this->scopes.back()->addEntry(res);

    // Increment offset
    int size = 0;
    if (isPrimitive(type)){

        size = 1;
    }
    else if (getStruct(type)){

        size = getStruct(type)->size();
    }
    else {
        // Unreachable code - type should be either primitive or struct id
        assert(false);
    }

    this->scopes.back()->incrementOffset(size);
}

void symbolTable::addVariable(varPair v, int lineno){
    this->addVariable(v.type, v.id, lineno);
}

void symbolTable::checkReturn(string& type, int lineno){
//    Get the function declaration
//    The last entry in the global scope should be a function entry
    FunctionEntry * funcDecl= dynamic_cast<FunctionEntry*>(global->getLastEntry());
    assert(funcDecl != NULL);
    if (!funcDecl->matchType(type)){
        output::errorMismatch(lineno);
        exit(0);
    }

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
        exit(0);
    }
}