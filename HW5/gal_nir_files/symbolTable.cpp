#include "symbolTable.hpp"
#include "output.hpp"

using namespace output;

string typeToStr(types t){
    
    switch (t)
    {
        case IntType:
            return "INT";
        case ByteType:
            return "BYTE";
        case BoolType:
            return "BOOL";
        case StringType:
            return "STRING";
        default:
            return "VOID";
    }
}

string TableEntry::getTypeString()
{
    return typeToStr(type);
}

void TableEntry::printType()
{
    printID(id, offset, getTypeString());
}

string TableEntryArray::getTypeString()
{
    return makeArrayType(typeToStr(type), size);
}

TableEntryFunc::TableEntryFunc(string id, types ret_type, const vector<typeStruct>& argsType) : TableEntry(id, 0, ret_type), argsType(argsType){}

string TableEntryFunc::getTypeString()
{
    vector<string> v;
    for (int i = 0; i < argsType.size(); i++)
    {
        if (argsType[i].arr_size == 0)
            v.push_back(typeToStr(argsType[i].type));
        else
            v.push_back(makeArrayType(typeToStr(argsType[i].type), argsType[i].arr_size));
    }
    return makeFunctionType(typeToStr(type), v);
}

ScopeSymbolTable::~ScopeSymbolTable(){}

TableEntry* ScopeSymbolTable::getEntry(string id){
    for (int i = 0; i < entries.size(); i++)
    {
        if(entries[i]->getId() == id){
            return entries[i];
        }
    }
    return NULL;
}

symbolTable::symbolTable() : retType(VoidType), lineno(1)
{
    tables.push_back(ScopeSymbolTable(0, false));
    vector<typeStruct> args;
    args.push_back(StringType);
    tables.back().addFunc("print", VoidType, args);
    args[0] = IntType;
    tables.back().addFunc("printi", VoidType, args);
}

TableEntry *symbolTable::getEntry(string id)
{
    for (int i = 0; i < tables.size(); i++)
    {
        TableEntry* t = tables[i].getEntry(id);
        if(t) return t;
    }
    return NULL;
}
void symbolTable::checkMain()
{
    TableEntryFunc *e = dynamic_cast<TableEntryFunc *>(getEntry("main"));
    if(!e || e->getTypesVec().size() > 0 || e->getType() != VoidType){
        errorMainMissing();
        exit(0);
    }
}