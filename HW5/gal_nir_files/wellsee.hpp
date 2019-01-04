#ifndef WELLSEE
#define WELLSEE

#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>
#include <set>
#include <iostream>
#include <sstream>
#include "symbolTable.hpp"
#include "bp.hpp"

using std::string;
using std::vector;
using std::set;
using std::stringstream;

const string TERMINATE = "Terminate";
const string TERMINATE_OUT_OF_BOUNDS = "TerminateArr";
const string TERMINATE_DIVISION_ZERO = "TerminateZero";

int emit(const string& s){
    return CodeBuffer::instance().emit(s);
}

void emitData(const std::string& dataLine){
    CodeBuffer::instance().emitData(dataLine);
}
void bpatch(const std::vector<int> &address_list, const std::string &loc){
    CodeBuffer::instance().bpatch(address_list, loc);
}

std::string genLabel(){
    return CodeBuffer::instance().genLabel();
}

std::vector<int> makelist(int litem){
    return CodeBuffer::makelist(litem);
}

std::vector<int> merge(const std::vector<int> &l1,const std::vector<int> &l2){
    return CodeBuffer::merge(l1, l2);
}

string tempToStr(tempType temp){
    assert(temp >= 0 && temp <= 15);
    switch(temp){
        case 0: return "$t0";
        case 1: return "$t1";
        case 2: return "$t2";
        case 3: return "$t3";
        case 4: return "$t4";
        case 5: return "$t5";
        case 6: return "$t6";
        case 7: return "$t7";
//        case 8: return "$t8"; // voltile
//        case 9: return "$t9"; // saved for initializations
        case 8: return "$s0";
        case 9: return "$s1";
        case 10: return "$s2";
        case 11: return "$s3";
        case 12: return "$s4";
        case 13: return "$s5";
        case 14: return "$s6";
        case 15: return "$s7";
        default: break;
    }
    return "";
}

class tempStack{
    vector<tempType> temp_stack;
    set<tempType> temp_used;
public:
    tempStack() {
        for(tempType i = 0; i <= 15; i++){
            temp_stack.push_back(i);
        }
    }
    ~tempStack(){}

    static tempStack& instance(){
        static tempStack ts;
        return ts;
    }

    tempType getTemp(){
        tempType t = temp_stack.back();
        temp_stack.pop_back();
        temp_used.insert(t);
        return t;
    }
    void tempDone(tempType v){
        if(v == NO_TEMP) return;
        assert(temp_used.count(v) == 1);
        temp_stack.push_back(v);
        temp_used.erase(v);
    }
    const set<tempType>& getUsed() const {
        return temp_used;
    }
};

tempType getTemp(){
    return tempStack::instance().getTemp();
}
void tempDone(tempType v){
    tempStack::instance().tempDone(v);
}
const set<tempType> &getUsed(){
    return tempStack::instance().getUsed();
}

void init_text_area(){
    emit(".globl     main");
    emit("main:");
    emit("subu $sp, $sp, 4");
    emit("sw $ra, ($sp)");
    emit("jal __main");
    emit(TERMINATE + ": li $v0, 10");
    emit("syscall");
    emit("__print:");
    emit("lw $a0, 0($sp)");
    emit("li $v0, 4");
    emit("syscall");
    emit("jr $ra");
    emit("__printi:");
    emit("lw $a0, 0($sp)");
    emit("li $v0, 1");
    emit("syscall");
    emit("jr $ra");
    emitData("OutOfBounds: .asciiz \"Error index out of bounds\\n\"");
    emitData("DivideInZero: .asciiz \"Error division by zero\\n\"");
    emit(TERMINATE_OUT_OF_BOUNDS + ": la $a0, OutOfBounds");
    emit("li $v0, 4");
    emit("syscall");
    emit("j " + TERMINATE);
    emit(TERMINATE_DIVISION_ZERO + ": la $a0, DivideInZero");
    emit("li $v0, 4");
    emit("syscall");
    emit("j " + TERMINATE);
}

void checkArrayBoundaries(int size, tempType offset)
{
    emit("blt " + tempToStr(offset) + ", 0, " + TERMINATE_OUT_OF_BOUNDS);
    stringstream ss;
    ss << "bge " << tempToStr(offset) << ", " << size << ", " << TERMINATE_OUT_OF_BOUNDS;
    emit(ss.str());
}

void checkNotDividingByZero(tempType t2)
{
    emit("beq " + tempToStr(t2) + ", 0, " + TERMINATE_DIVISION_ZERO);
}

void createVar(){
    emit("subu $sp, $sp, 4");
    emit("sw $0, ($sp)");
}

void createArray(int size){
    stringstream s;
    s << "subu $sp, $sp, " << 4*size;
    emit(s.str());
    for(int i = 0; i < size; i++){
        s.clear();
        s.str("");
        s << "sw $0, " << i * 4 << "($sp)";
        emit(s.str());
    }
}

void createArgsTemp(tempType t) {
    emit("subu $sp, $sp, 4");
    emit("sw "+ tempToStr(t) + ", ($sp)");
}

void createVarInit(tempType source){
    emit("subu $sp, $sp, 4");
    emit("sw " + tempToStr(source) + ", ($sp)");
}

void createArgsArray(types type, int offset_source, int size){
    stringstream s;
    s << "subu $sp, $sp, " << size*4;
    emit(s.str());
    for(int i = 0; i < size ; i++){
        s.str("");;
        s << "lw $t9, " << i * 4 - offset_source << ("($fp)");
        emit(s.str());
        s.str("");
        s << "sw $t9, " << i * 4 << "($sp)";
        emit(s.str());
    }
}

void accessVar(typeStruct ts, tempType target){
    assert(ts.arr_size == 0);
    stringstream s;
    s << "lw " << tempToStr(target) << ", " << -ts.offset << "($fp)";
    emit(s.str());
}

void accessArray(typeStruct ts, tempType offset, tempType target){
    assert(ts.arr_size != 0);
    checkArrayBoundaries(ts.arr_size, offset);
    emit("sll $t9, " + tempToStr(offset) + ", 2"); //multiply by 4
    emit("add $t9, $t9, $fp");
    stringstream s;
    s << "lw " << tempToStr(target) << ", " << -ts.offset << "($t9)";
    emit(s.str());
}

void addFunction(string name){
    name = "__" + name + ":";
    emit(name);
    emit("move $fp, $sp");
}

void endFunction(){
    emit("move $sp, $fp");
    emit("jr $ra");
}

void returnFunction(types type, tempType t){
    emit("move $v0, " + tempToStr(t));
    endFunction();
}

void callFunction(const string &func_name){
    emit("jal __" + func_name);
}

void releaseArgs(int s){
    stringstream ss;
    ss << "addu $sp, $sp, " << s;
    emit(ss.str());
}

void backupBeforeFunction(){
    stringstream ss;
    ss << "subu $sp, $sp, " << (getUsed().size() + 2) * 4;
    emit(ss.str());
    emit("sw $ra, ($sp)");
    emit("sw $fp, 4($sp)");
    int t = 8;
    for(set<tempType>::iterator it = getUsed().begin(); it != getUsed().end(); it++){
        ss.clear();
        ss.str("");
        ss << "sw " << tempToStr(*it) << ", " << t << "($sp)";
        emit(ss.str());
        t += 4;
    }
}

void restoreAfterFunction(){
    stringstream ss;
    emit("lw $ra, ($sp)");
    emit("lw $fp, 4($sp)");
    int t = 8;
    for (set<tempType>::iterator it = getUsed().begin(); it != getUsed().end(); it++)
    {
        ss.clear();
        ss.str("");
        ss << "lw " << tempToStr(*it) << ", " << t << "($sp)";
        emit(ss.str());
        t += 4;
    }
    ss.clear();
    ss.str("");
    ss << "addu $sp, $sp, " << (getUsed().size() + 2) * 4;
    emit(ss.str());
}

void createString(const string& s, tempType t){
    static int counter = 0;
    stringstream ss;
    ss << "string_label" << counter++;
    string label = ss.str();
    ss << ": .asciiz " << s;
    emitData(ss.str());
    emit("la " + tempToStr(t) + ", " + label);
}

void returnToTemp(tempType t){
    emit("move " + tempToStr(t) + ", $v0");
}

void loadNumToTemp(const string& s, tempType t){
    emit("li " +  tempToStr(t) + ", " + s);
}

void truncation(const string& str)
{
    emit("and " + str + ", 0xff");
}

void truncation(tempType t){
    truncation(tempToStr(t));
}

void loadToVar(typeStruct ts, tempType source)
{
    assert(ts.arr_size == 0);
    stringstream s;
    s << "sw " << tempToStr(source) << ", " << -ts.offset << "($fp)";
    emit(s.str());
}
void loadToVarArray(typeStruct ts, int offset_source){
    stringstream s;
    for (int i = 0; i < ts.arr_size; i++)
    {
        s.str("");
        ;
        s << "lw $t9, " << i * 4 - offset_source << ("($fp)");
        emit(s.str());
        s.str("");
        s << "sw $t9, " << i * 4 - ts.offset << "($fp)";
        emit(s.str());
    }
}

void loadToArray(typeStruct ts, tempType offset_of_target, tempType source)
{
    assert(ts.arr_size != 0);
    checkArrayBoundaries(ts.arr_size, offset_of_target);
    emit("sll $t9, " + tempToStr(offset_of_target) + ", 2"); //multiply by 4
    emit("add $t9, $t9, $fp");
    stringstream s;
    s << "sw " << tempToStr(source) << ", " << -ts.offset << "($t9)";
    emit(s.str());
}

void arithmetics(types type, const string& op_str, tempType target, tempType t1, tempType t2){
    string u = " ";
    // if(type == ByteType){
    //     u = "u ";
    // }
    string op;
    if(op_str == "+"){
        op = "addu";
    } else if(op_str == "-"){
        op = "subu";
    } else if(op_str == "*"){
        op = "mul";
    } else {
        op = "div";
        checkNotDividingByZero(t2);
    }
    emit(op + u + tempToStr(target) + ", " + tempToStr(t1) + ", " + tempToStr(t2));
    if(type == ByteType){
        truncation(target);
    }
}

void logicalRelops(const string& rop_str, tempType t1, tempType t2, vector<int>& truelist, vector<int>& falselist){
    string rop = "";
    if(rop_str == "=="){
        rop = "beq ";
    } else if(rop_str == "!="){
        rop = "bne ";
    } else if(rop_str == "<"){
        rop = "blt ";
    } else if(rop_str == ">"){
        rop = "bgt ";
    } else if(rop_str == "<="){
        rop = "ble ";
    } else {
        rop = "bge ";
    }
    truelist = makelist(emit(rop + tempToStr(t1) + ", " + tempToStr(t2) + ", " ));
    falselist = makelist(emit("j "));
}

void returnToBoolExp(vector<int> &truelist, vector<int> &falselist){
    truelist = makelist(emit("bne $v0, 0, "));
    falselist = makelist(emit("j "));
}

void initBool(vector<int> &truelist, vector<int> &falselist){
    bpatch(truelist, genLabel());
    emit("li $t9, 1");
    vector<int> tempV = makelist(emit("j "));
    bpatch(falselist, genLabel());
    emit("li $t9, 0");
    bpatch(tempV, genLabel());
    emit("subu $sp, $sp, 4");
    emit("sw $t9, ($sp)");
}

void initBoolArgs(const string& ret_label , tempType t, vector<int> &truelist, vector<int> &falselist){
    vector<int> tempV = makelist(emit("j "));    
    bpatch(truelist, genLabel());
    emit("li " + tempToStr(t) + ", 1");
    emit("j " + ret_label);
    bpatch(falselist, genLabel());
    emit("li " + tempToStr(t) + ", 0");
    emit("j " + ret_label);
    bpatch(tempV, genLabel());
    emit("subu $sp, $sp, 4");
    emit("sw " + tempToStr(t) + ", ($sp)");
}

void assignBool(typeStruct ts, vector<int> &truelist, vector<int> &falselist){
    assert(ts.arr_size == 0);
    bpatch(truelist, genLabel());
    emit("li $t9, 1");
    vector<int> tempV = makelist(emit("j "));
    bpatch(falselist, genLabel());
    emit("li $t9, 0");
    bpatch(tempV, genLabel());
    stringstream ss;
    ss << "sw $t9, " << -ts.offset << "($fp)";
    emit(ss.str()); 
}

void assignBoolArray(typeStruct ts, tempType offset, vector<int> &truelist, vector<int> &falselist)
{
    assert(ts.arr_size != 0);
    checkArrayBoundaries(ts.arr_size, offset);
    bpatch(truelist, genLabel());
    emit("li $t9, 1");
    vector<int> tempV = makelist(emit("j "));
    bpatch(falselist, genLabel());
    emit("li $t9, 0");
    bpatch(tempV, genLabel());
    emit("sll $t8, " + tempToStr(offset) + ", 2"); //multiply by 4
    emit("add $t8, $t8, $fp");
    stringstream s;
    s << "sw $t9, " << -ts.offset << "($t8)";
    emit(s.str());
}

void returnBool(vector<int> &truelist, vector<int> &falselist)
{
    bpatch(truelist, genLabel());
    emit("li $v0, 1");
    endFunction();
    bpatch(falselist, genLabel());
    emit("li $v0, 0");
    endFunction();
}

void accessBool(typeStruct ts, vector<int>& truelist, vector<int>& falselist){
    assert(ts.arr_size == 0);
    stringstream s;
    s << "lw $t9, " << -ts.offset << "($fp)";
    emit(s.str());
    truelist = makelist(emit("bne $t9, 0, "));
    falselist = makelist(emit("j "));
}

void accessBoolArray(typeStruct ts, tempType offset, vector<int> &truelist, vector<int> &falselist){
    assert(ts.arr_size != 0);
    checkArrayBoundaries(ts.arr_size, offset);
    emit("sll $t9, " + tempToStr(offset) + ", 2"); //multiply by 4
    emit("add $t9, $t9, $fp");
    stringstream s;
    s << "lw $t9, " << -ts.offset << "($t9)";
    emit(s.str());
    truelist = makelist(emit("bne $t9, 0, "));
    falselist = makelist(emit("j "));
}

void fixStack(int fix){
    stringstream ss;
    ss << "addu $sp, $sp, " << fix*4;
    emit(ss.str());
}

void boolToTemp(tempType target, vector<int> &truelist, vector<int> &falselist){
    bpatch(truelist, genLabel());
    emit("li " + tempToStr(target) + " 1");
    vector<int> tempV = makelist(emit("j "));
    bpatch(falselist, genLabel());
    emit("li " + tempToStr(target) + " 0");
    bpatch(tempV, genLabel());
    truelist.clear();
    falselist.clear();
}

#endif // WELLSEE
