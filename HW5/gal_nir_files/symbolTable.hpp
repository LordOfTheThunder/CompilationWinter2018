#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include "attributes.h"
#include "output.hpp"
#include <string>
#include <vector>
#include <cassert>
using std::string;
using std::vector;
using namespace output;


const int WORDSIZE = 4;

string typeToStr(types t);

class TableEntry
{
  private:
    string id;
    int offset;

  protected:
    types type;

  public:
    TableEntry(string id, int offset, types type) : id(id), offset(offset), type(type) {}
    virtual ~TableEntry() {}

    virtual string getTypeString();

    types getType(){
        return type;
    }

    const string& getId(){
        return id;
    }

    int getOffset() const {
        return WORDSIZE * (offset + 1);
    }

    void printType();

};

class TableEntryArray : public TableEntry
{
  private:
    int size;

  public:
    TableEntryArray(string id, int offset, types type, int size) : TableEntry(id, offset, type), size(size){}
    ~TableEntryArray() {}
    string getTypeString();
    int getSize() { return size; }
};

class TableEntryFunc : public TableEntry
{
  private:
    vector<typeStruct> argsType;

  public:
    TableEntryFunc(string id, types ret_type, const vector<typeStruct>& argsType);
    ~TableEntryFunc() {}
    string getTypeString();
    const vector<typeStruct>& getTypesVec() {
        return argsType;
    }
};

class ScopeSymbolTable
{
  private:
    vector<TableEntry *> entries;
    int offset;
    int argsOffset;
    bool inLoop;

  public:
    ScopeSymbolTable(int initial_offset, bool inLoop) : offset(initial_offset), argsOffset(0), inLoop(inLoop) {}
    ~ScopeSymbolTable();
    
    void endTable(){
        //endScope();
        for (int i = 0 ; i < entries.size() ; i++ )
        {
            //entries[i]->printType();
            delete entries[i];
        }
        //entries.clear();
    }
    
    void addVar(string id, types type)
    {
        entries.push_back(new TableEntry(id, offset++, type));
    }
    void addArray(string id, types type, int size)
    {
        offset += size;
        entries.push_back(new TableEntryArray(id, offset-1, type, size));
    }
    void addFunc(string id, types return_type, const vector<typeStruct>& argsType)
    {
        entries.push_back(new TableEntryFunc(id, return_type, argsType));
    }

    void addVarArgs(string id, types type)
    {
        entries.push_back(new TableEntry(id, --argsOffset, type));
    }
    void addArrayArgs(string id, types type, int size)
    {
        entries.push_back(new TableEntryArray(id, argsOffset-1, type, size));
        argsOffset -= size;
    }

    TableEntry* getEntry(string id);

    int getOffset()
    {
        return offset;
    }

    bool isInLoop() { return inLoop; }
};

class symbolTable
{
  private:
    vector<ScopeSymbolTable> tables;

    TableEntry *getEntry(string id);

    void idIsAvailabe(const string& id ) {
        if(getEntry(id)){
            errorDef(lineno, id);
            exit(0);
        }
    }

  public:
    int lineno;
    types retType;
    
    symbolTable();
    ~symbolTable() {}

    typeStruct getVar(const string& id){
        TableEntry* e = getEntry(id);
        if (!e || dynamic_cast<TableEntryFunc *>(e))
        {
            errorUndef(lineno, id);
            exit(0);
        }
        typeStruct ts(e->getType());
        ts.offset = e->getOffset();
        if(dynamic_cast<TableEntryArray*>(e)){
            ts.arr_size = dynamic_cast<TableEntryArray *>(e)->getSize();
        }
        return ts;
    }

    TableEntryFunc* getFunc(const string& id){
        TableEntryFunc *e = dynamic_cast<TableEntryFunc *>(getEntry(id));
        if (!e)
        {
            errorUndefFunc(lineno, id);
            exit(0);
        }
        return e;
    }

    void addVar(string id, typeStruct type)
    {
        idIsAvailabe(id);
        if(type.arr_size == 0)
            tables.back().addVar(id, type.type);
        else
            tables.back().addArray(id, type.type, type.arr_size);
    }
    
    void addFunc(string id, types return_type, const vector<typeStruct>& argsType)
    {
        idIsAvailabe(id);
        tables.back().addFunc(id, return_type, argsType);
    }

    void addVarArgs(string id, typeStruct type)
    {
        idIsAvailabe(id);
        if(type.arr_size == 0)
            tables.back().addVarArgs(id, type.type);
        else
            tables.back().addArrayArgs(id, type.type, type.arr_size);
    }

    void addTable(bool isLoop){
        tables.push_back(ScopeSymbolTable(
            tables.back().getOffset(),
            isLoop
            ));
    }

    void removeTable(){
        assert(!tables.empty());
        tables.back().endTable();
        tables.pop_back();
    }

    bool isInLoop() {
        for(vector<ScopeSymbolTable>::reverse_iterator it = tables.rbegin(); it != tables.rend() ; it++){
            if(it->isInLoop()){
                return true;
            }
        }
        return false;
    }

    void checkMain();

    int getOffsetDelta(){
        assert(!tables.empty());        
        if(tables.size() == 1){
            return tables.back().getOffset();
        } else {
            return tables.back().getOffset() - tables[tables.size() - 2].getOffset();
        }
    }

    int getOffsetDeltaLoop(){
        assert(!tables.empty());
        for(vector<ScopeSymbolTable>::reverse_iterator it = tables.rbegin(); it != tables.rend() ; it++){
            if(it->isInLoop()){
                it++;
                return tables.back().getOffset() - it->getOffset();
            }
        }
        assert(false);
        return 0; // should never happen
    }

};

#endif // !SYMBOL_TABLE
