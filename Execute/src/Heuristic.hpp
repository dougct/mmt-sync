#ifndef __HEADER_HEURISTIC__
#define __HEADER_HEURISTIC__

#include "Types.hpp"
#include "Thread.hpp"
#include "InstructionFlags.hpp"

class Heuristic {
  protected:
    UINT num_threads;
    Thread* t;
    
  public:
    Heuristic(UINT nt, Thread* threads)
    : num_threads(nt), t(threads)
    {}
    virtual ~Heuristic(){}
    
    virtual void choose_threads() = 0;
    virtual void register_instruction(Instruction const & insn, UINT tid, uint64_t sequence) {}  // Default: just ignore
    
};

extern "C" Heuristic* new_heuristic(UINT nt, Thread* t, const char* param);
extern "C" void delete_heuristic(Heuristic* h);

#endif
