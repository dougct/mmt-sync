#include "SimpleHeuristic.hpp"

void Sync::choose_threads()
{
    // Round robin among active threads
    static int rrcounter = 0;
    unsigned int ref = -1;
    for (UINT ii=0; ii<num_threads; ii++){
        unsigned int i = (ii + rrcounter) % num_threads;
        if (!t[i].ignore){
            ref = i;
            break;
        }
    }
    for (UINT i=0; i<num_threads; i++){
        //t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
        t[i].is_active = (i == ref);    // No fetch combining
    }
    // Next
    rrcounter = (ref + 1) % num_threads;
}
