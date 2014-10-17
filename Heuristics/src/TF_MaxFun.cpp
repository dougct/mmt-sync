#include "Heuristic.hpp"
#include "FunctionMarks.hpp"
#include <stdio.h>

class Sync : public Heuristic {
    int* function_level;
    int ref;
    bool returned;
    
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
    returned = false;
    ref = -1;
    function_level = new int[nt];
    for (UINT i=0; i<nt; i++){
        function_level[i] = 1;
    }
}

Sync::~Sync(){
    delete [] function_level;
}


void Sync::choose_threads(){
    
    if (returned){
        for (UINT i=0; i<num_threads; i++){
            if (t[i].is_active || t[i].pc == t[ref].pc){
                //Largest number. The basic block must continue after return.
                if (t[i].bb_priority == 0){
                    t[i].bb_priority = 0x7FFFFFFF;
                }
            }
        }
        returned = false;
        ref = -1; //Choose again
    }
    
    //Check if a new basic block was found. Choose threads again
    bool new_bb = ref == -1 || t[ref].ignore || t[ref].bb_priority > 0;
    bool rechoose = false;
    
    if (!new_bb){
        //Check basic blocks that LLVM creates when it generates X6 code, for instance:
        //    cmpb    $1, %bl
        //    jne .LBB0_10
        //    jmp .LBB0_9
        //  .LBB0_11:
        
        for (UINT i=0; i<num_threads; i++){ 
            if (t[i].is_active && t[i].pc != t[ref].pc){
                rechoose = true;
            }
        }
        if (rechoose){
            for (UINT i=0; i<num_threads; i++){ 
                if (t[i].is_active && !t[ref].ignore){
                    if (t[i].bb_priority == 0){
                        t[i].bb_priority = 0x7FFFFFFF;
                    }
                }
            }
        }
    }
    
    //Choose threads
    if (new_bb || rechoose){
        ref = -1;
        for (UINT i=0; i<num_threads; i++){
            if (!t[i].ignore){
                if (ref == -1 || function_level[i] > function_level[ref] ||
                    ( function_level[i] == function_level[ref] && t[i].bb_priority > t[ref].bb_priority) ||
                    ( function_level[i] == function_level[ref] && t[i].bb_priority == t[ref].bb_priority && t[i].pc < t[ref].pc)
                ){
                    //MinSP can also be used in the place of max function_level
                    ref = i;
                }
            }
        }
        for (UINT i=0; i<num_threads; i++){
            t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
        }
    }
    
    //If this instruction calls a function
    if ( is_function_call( &t[ref] ) ){
        for (UINT i=0; i<num_threads; i++){
            if (t[i].is_active){
                function_level[i]++;
            }
        }
    }
    //If this instruction returns from a function
    if ( is_function_ret( &t[ref] ) ){
        for (UINT i=0; i<num_threads; i++){
            if (t[i].is_active){
                function_level[i]--;
            }
        }
        returned = true;
    }
    
}

