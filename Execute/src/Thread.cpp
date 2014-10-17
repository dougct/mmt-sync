#include "Thread.hpp"
#include <stdio.h>
#include <algorithm>


static void readf(FILE* arq, void* ptr, UINT size){
    unsigned char* pos = (unsigned char*) ptr;
    for (UINT i=0; i<size; i++){
        pos[i] = fgetc(arq);
    }
}
static bool teste_eof(FILE* arq){
    bool end = feof(arq);
    return end;
}


Instruction::Instruction()
{

}

Instruction::~Instruction()
{

}

bool Instruction::read(FILE * file)
{
    unsigned char mqflags;
    readf(file, &pc, sizeof(ADDRINT));
    readf(file, &sp, sizeof(ADDRINT));
    readf(file, &mqflags, sizeof(char));
    readf(file, &reg_size, sizeof(char));
    readf(file, &cf_flags, sizeof(char));
#ifdef COMPILER_SUPPORT
    readf(file, &bb_priority, sizeof(UINT));
#endif
    
    if (teste_eof(file) ){
        fclose(file);
        return false;
    }

    barrier = (mqflags & 0x80) != 0; //128
    basic_block = (mqflags & 0x40) != 0; //64
#ifdef COMPILER_SUPPORT
    pragma_fun_call = (mqflags & 0x20) != 0; //32
    pragma_fun_ret = (mqflags & 0x10) != 0; //16
    bool pdom_pragmas = (mqflags & 0x08) != 0; //8
    if (pdom_pragmas){
        readf(file, &pragma_begin_list, sizeof(UINT));
        
        readf(file, &pragma_end_counter, sizeof(char));
        for (UINT k=0; k<pragma_end_counter; k++){
            readf(file, &pragma_end_list[k], sizeof(UINT));
        }
    }
    else {
        pragma_begin_list = 0;
        pragma_end_counter = 0;
    }
#endif
    mem_quant = mqflags & 0x07;  // Lowest 3 bits
    for(UINT j=0; j<mem_quant; j++){
        readf(file, &mem_address[j], sizeof(ADDRINT));
    }
    
    if (reg_size > 0){
        readf(file, reg_data, reg_size);
    }
    return true;
}


void Thread::load(const char* file){
    thread_arq = fopen(file, "r");
    
    if (thread_arq == NULL){
        char* msg = new char[200];
        sprintf(msg, "Invalid input file (with the thread): %s.", file);
        throw Exception(msg);
    }
    
    is_active = true;
    finished = false;
    
    ignore = false;
    
    basic_block = false;
    cf_flags = 0;
    
    #ifdef COMPILER_SUPPORT
    pragma_fun_call = false;
    pragma_fun_ret = false;
    pragma_begin_list = 0;
    pragma_end_counter = 0;
    #endif
    
}

void Thread::fetch(Instruction const & insn)
{
    pc = insn.pc;
    sp = insn.sp;
    reg_size = insn.reg_size;
    barrier = insn.barrier;
    basic_block = insn.basic_block;
    cf_flags = insn.cf_flags;
#ifdef COMPILER_SUPPORT
    bb_priority = insn.bb_priority;
    pragma_fun_call = insn.pragma_fun_call;
    pragma_fun_ret = insn.pragma_fun_ret;
    pragma_begin_list = insn.pragma_begin_list;
    pragma_end_counter = insn.pragma_end_counter;
    if(pragma_end_counter > 0) {
        std::copy(insn.pragma_end_list, insn.pragma_end_list + pragma_end_counter, pragma_end_list);
    }
#endif
    mem_quant = insn.mem_quant;
    std::copy(insn.mem_address, insn.mem_address + mem_quant, mem_address);
    reg_size = insn.reg_size;
    std::copy(insn.reg_data, insn.reg_data + reg_size, reg_data);
}

Thread::Thread() :
    not_eof(true)
{
}

