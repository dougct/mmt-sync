#include "PostDom.hpp"

void Sync::execute_sub(){
    
    BlocoPragma& current = stack.front();
    
    ref = -1;
    for (UINT i=0; i<num_threads; i++){
        if ( !t[i].ignore && current.threads[i] ){
            if ( ref == -1 || function_level[i] > function_level[ref] || (function_level[i] == function_level[ref] && t[i].pc < t[ref].pc) ){
                ref = i;
            }
        }
    }
    
}
