#include "Heuristic.hpp"

class Sync : public Heuristic {
  public:
    Sync(UINT nt, Thread* t, const char* param)
    : Heuristic(nt, t) {}
    
    ~Sync(){}
    
    virtual void choose_threads();
    
};

Heuristic* new_heuristic(UINT nt, Thread* t, const char* param){
    return new Sync(nt, t, param);
}
void delete_heuristic(Heuristic* h){
    delete h;
}
