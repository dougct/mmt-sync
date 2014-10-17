#include "PostDom.hpp"
#include <bitset>

void Sync::execute_sub(){
    
    BlocoPragma& current = stack.front();
    
    static int rrcounter = 0;
    static std::bitset<64> was_executed(0);
    
    ref = -1;
    for (UINT ii=0; ii<num_threads; ii++){
        unsigned int i = (ii + rrcounter) % num_threads;
        if ( !t[i].ignore && current.threads[i] && !was_executed.test(i) ){
            ref = i;
            break;
        }
    }
    if (ref == -1){
        was_executed.reset();
        for (UINT ii=0; ii<num_threads; ii++){
            unsigned int i = (ii + rrcounter) % num_threads;
            if (!t[i].ignore && current.threads[i]){
                ref = i;
                break;
            }
        }
    }
    
    for (UINT i=0; i<num_threads; i++){
        t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
        if (t[i].is_active){
            was_executed.set(i);
        }
    }
    
    // Next
    rrcounter = (ref + 1) % num_threads;
    
}
