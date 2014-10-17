#include "Heuristic.hpp"
#include "FunctionMarks.hpp"
#include <stdio.h>

class Sync : public Heuristic {
    int* function_level;
    int mode;
    
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
    mode = 1;
    sscanf(param, "%d", &mode);
    
    function_level = new int[nt];
    for (UINT i=0; i<nt; i++){
        function_level[i] = 1;
    }
}

Sync::~Sync(){
    delete [] function_level;
}


void Sync::choose_threads(){
    
    int ref = -1;
    for (UINT i=0; i<num_threads; i++){
        if (!t[i].ignore){
            bool dochange = false;
            switch (mode){
                case 1:
                    dochange = ref == -1 || function_level[i] > function_level[ref] || ( function_level[i] == function_level[ref] && t[i].pc < t[ref].pc );
                    break;
                case 2:
                    dochange = ref == -1 || function_level[i] > function_level[ref] ||
                    ( function_level[i] == function_level[ref] && t[i].sp > t[ref].sp ) ||
                    ( function_level[i] == function_level[ref] && t[i].sp == t[ref].sp && t[i].pc < t[ref].pc );
                    break;
                case 3:
                    dochange = ref == -1 || function_level[i] > function_level[ref] ||
                    ( function_level[i] == function_level[ref] && t[i].sp < t[ref].sp ) ||
                    ( function_level[i] == function_level[ref] && t[i].sp == t[ref].sp && t[i].pc < t[ref].pc );
                    break;
                default:
                    throw Exception("Error in MaxFun-MinPC heuristic: Invalid mode.");
            }
            
            if (dochange){
                ref = i;
            }
        }
    }
    for (UINT i=0; i<num_threads; i++){
        t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
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
    }
    
}
