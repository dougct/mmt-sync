#include "Heuristic.hpp"
#include "FunctionMarks.hpp"

#include <list>

struct Struct{
    bool current;
    bool function;
};

class Sync : public Heuristic {
    ADDRINT* last_pc;
    std::list<Struct*> current;
    bool branch;
    
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
    last_pc = new ADDRINT[num_threads];
    Struct* st = new Struct[num_threads];
    for (UINT i=0; i<num_threads; i++){
        last_pc[i] = 0;
        st[i].current = true;
        st[i].function = true;
    }
    
    current.push_back( st );
    branch = false;
}
Sync::~Sync(){
    delete [] last_pc;
    delete [] current.back();
    current.pop_back();
}

void Sync::choose_threads(){
    
    Struct* st = current.back();
    
    //Check if there was backward branch
    if (branch){
        for (UINT i=0; i<num_threads; i++){
            if (!t[i].ignore && t[i].is_active && st[i].function){
                if ( st[i].current ){
                    st[i].current = t[i].pc >= last_pc[i];
                }
            }
        }
    }
    
    
    //Smallest pc in current
    int ref = -1;
    do {
       
        for (UINT i=0; i<num_threads; i++){
            if (!t[i].ignore && st[i].function && st[i].current ){
                if ( ref < 0 || t[i].pc < t[ref].pc ){
                    ref = i;
                }
            }
        }
       
        //current is empty
        if (ref == -1){
            for (UINT i=0; i<num_threads; i++){
                st[i].current = true;
            }
            ref = -2;
            continue;
        }
        else if (ref == -2){
            throw Exception("Error detected by heuristic code: No thread was chosen to execute.");
        }
        else {
            break;
        }
    }while(true);
    
    
    //Chosen threads
    for (UINT i=0; i<num_threads; i++){
        t[i].is_active = !t[i].ignore && st[i].function && t[i].pc == t[ref].pc;
        if (t[i].is_active){
            last_pc[i] = t[i].pc;
            st[i].current = true;
        }
    }
    
    
    branch = (t[ref].cf_flags & InstructionFlags::IsBranch) != 0;
    
    //If a thread found the beginning or the end of a function
    if ( is_function_call( &t[ref] ) ){
        Struct* nst = new Struct[num_threads];
        for (UINT i=0; i<num_threads; i++){
            nst[i].current = true;
            nst[i].function = t[i].is_active;
        }
        current.push_back( nst );
    }
    if ( is_function_ret( &t[ref] ) ){
        UINT count = 0;
        for (UINT i=0; i<num_threads; i++){
            if (t[i].is_active){
                st[i].function = false;
            }
            if (st[i].function){
                count++;
            }
        }
        if (count == 0){
            delete [] current.back();
            current.pop_back();
            if (current.size() == 0){
                throw Exception("Error in heuristic: Irregular call/return pattern.");
            }
        }
    }
    
}
