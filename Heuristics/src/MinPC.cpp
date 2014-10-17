#include "SimpleHeuristic.hpp"

void Sync::choose_threads(){
    
    //Choose the smallest PC
    int ref = -1;
    for (UINT i=0; i<num_threads; i++){
        if (!t[i].ignore){
            if ( ref == -1 || t[i].pc < t[ref].pc ){
                ref = i;
            }
        }
    }
    for (UINT i=0; i<num_threads; i++){
        t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
    }
    
}
