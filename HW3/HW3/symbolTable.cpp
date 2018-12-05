#include "symbolTable.hpp"
#include "StackStructs.h"

symbolTable::symbolTable() : line(1), mainExists(false) {
    newScope(false);
    vector<types> print_args;
    print_args.push_back(types_String);
    vector<types> printi_args;
    printi_args.push_back(types_Int);
    this->addFunction(types.types_Void, "print", print_args)
    this->addFunction(types.types_Void, "printi", printi_args)
}

void symbolTable::addFunction(types retval, string& id, vector<types> formals, bool addScope) {
    validateId(id);

    if ((id.compare("main") == 0) &&
            (formals.size() == 0) &&
            (retval == types_Void)){
        this->mainExists = true;
    }

    if (addScope)
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