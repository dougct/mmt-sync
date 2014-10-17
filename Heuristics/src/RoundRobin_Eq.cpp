#include "Heuristic.hpp"
#include <bitset>

class Sync : public Heuristic {
    std::bitset<64> was_executed;
    UINT rrcounter;
    
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
    if (nt > 64){
        throw Exception("This heuristic does not support more than 64 threads due to smaller allocated space.");
    }
    was_executed.reset();
    rrcounter = 0;
}

Sync::~Sync(){}


void Sync::choose_threads(){
    int ref = -1;
    for (UINT ii=0; ii<num_threads; ii++){
        unsigned int i = (ii + rrcounter) % num_threads;
        if ( !t[i].ignore && !was_executed.test(i) ){
            ref = i;
            break;
        }
    }
    if (ref == -1){
        was_executed.reset();
        for (UINT ii=0; ii<num_threads; ii++){
            unsigned int i = (ii + rrcounter) % num_threads;
            if (!t[i].ignore){
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
