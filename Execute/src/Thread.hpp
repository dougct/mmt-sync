#ifndef __HEADER_THREAD__
#define __HEADER_THREAD__

#include "Types.hpp"
#include <stdio.h>
#include <deque>

struct Instruction {
    ADDRINT pc;
    ADDRINT sp;
#ifdef COMPILER_SUPPORT
    UINT bb_priority;
    UINT pragma_end_list[256];
    UINT pragma_begin_list;
    unsigned char pragma_end_counter;
#endif
    unsigned char reg_size;
    unsigned char cf_flags;
    unsigned char mem_quant;
    
    ADDRINT mem_address[4];
    unsigned char reg_data[256]; //It will not be larger than 256
    bool barrier;
    bool basic_block;
#ifdef COMPILER_SUPPORT
    bool pragma_fun_call;
    bool pragma_fun_ret;
#endif
    
    bool read(FILE * file);
    Instruction();
    ~Instruction();
};


class Thread{
  public:
    FILE* thread_arq;
    std::deque<Instruction> instruction_queue;
    bool not_eof;
    bool barrier;
    bool finished;
    
    //Interface for heuristics
    bool is_active;
    bool ignore; //If it has finished or it is in a barrier
    
    ADDRINT pc;
    ADDRINT sp;
    
    bool basic_block; //Hardware basic block
    unsigned char cf_flags;
    
    unsigned char mem_quant;
    unsigned char reg_size;
    
    ADDRINT mem_address[4];
    unsigned char reg_data[256];
    
    #ifdef COMPILER_SUPPORT
    UINT bb_priority;
    bool pragma_fun_call;
    bool pragma_fun_ret;
    UINT pragma_begin_list;
    unsigned char pragma_end_counter;
    UINT pragma_end_list[256];
    #endif
    
    
  public:
    void load(const char* file);
    void fetch(Instruction const & insn);
    
    Thread();
};


#endif
