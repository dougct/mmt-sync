//STRUCTURED CONTROL FLOW GRAPH HEURISTIC

#include "Heuristic.hpp"

class Sync : public Heuristic {
    UINT* stack_level;
    
  public:
    Sync(UINT nt, Thread* t, const char* param);
    ~Sync();
    
    void choose_threads();
};

Heuristic* new_heuristic(UINT nt, Thread* t, const char* param){
    return new Sync(nt, t, param);
}
void delete_heuristic(Heuristic* h){
    delete h;
}


Sync::Sync(UINT nt, Thread* t, const char* param)
: Heuristic(nt, t)
{
    stack_level = new UINT[num_threads];
    for (UINT i=0; i<num_threads; i++){
        stack_level[i] = 0;
    }
}
Sync::~Sync(){
    delete [] stack_level;
}

void Sync::choose_threads(){
    //Not implemented
    
}


/*
The heuristic requires programs with structured control flow graph, but continue and break statements with any depth is allowed.

Code requiriments:
*No indirect branch
*All loops must be while(true){...}
**while(cond) become while(true){ if(!cond)break; ...}
**do-while(cond) become while(true){ ... if(!cond)break;} or while(true){ ... if(cond)continue; break;}
*the end of a iteration is continue (unconditional) or if-continue (conditional)

Instructions:
*if
*if-else (the jump in the end of if tells to execute else)
*return from anywhere (Decreases the nesting depth and return)
*break n; where n is the nesting loop depth (Unconditional. Ends the loop, decreases the nesting depth and jumps to the block after the loop)
*continue n; where n is the nesting loop depth (Unconditional. Ends the loop, decreases the nesting depth and jumps to the beginning of the loop)
*if-continue - Conditional. No intermediate basic block
*if-break - Conditional. No intermediate basic block
*begin-while pragma (Increases the nesting loop depth. This is the beginning of each iteration (and the destiny of continue))
*block-with-break pragma - (The exit of the block is a break or return, then the execution of the block will be delayed until all threads finish the loop)
*reconvergence pragma - (It is in the end of if(without else) and else, the next instruction is the reconvergence point)
*Alternative instruction: if-continue-else-break_on_fallthrough - It improves do-while statements.

This code is more generic:
-The heuristic will continue easy and without problems.
-The control-structure is almost high-level language.
-The code expansion will be smaller than transformation without nested break/continue.
-No stack is used, each thread has its nesting counter.
-But the loop never ends as a fall-through of a branch without another specific instruction.
-All the loop depth data of branch/jump instructions depends on static values in instructions.

Pragmas about the next instruction
-bit: begin while
-bit: block ends with break/return
-bit: reconvergence of if or if-else
They are special instructions, but may be changed to be told by 2 reserved bits in instruction code. (Functions that starts with loop would have NOP or useless instruction in the beginning)

Second version:
-Improvement for recursive functions (or detected the same PC with different SP)

Third version:
-Improvement for function calls in nested loops
Synchronize func() in:
if (cond){
    ...
    func();
    ...
} else {
    ...
    func();
    ...
}


Fourth version:
-Indirect branch support if it is a simple switch statement.


Conversion of if:
if (a && b){...}

if (a){
    if(b){...}
}


if (a || b){...}

bool aux = false;
if (a){
    aux = true;
}
else {
    if(b){
        aux = true;
    }
}
if (aux){...}
*/

