#include "Heuristic.hpp"
#include <map>
#include <bitset>
#include <iostream>
#include <cstdlib>
#include <cstdio>

//#define DUMPTRACE 42
//#define FASTPATH

struct ReconvergenceEntry
{
    typedef std::bitset<64> Bitset;
	Bitset mask;
	unsigned int counter;
	
	ReconvergenceEntry() :
		counter(0) {
		mask.set();	// Initialize to all set
	}
	ReconvergenceEntry(Bitset m, unsigned int c) :
		mask(m), counter(c) {}
};

class Sync : public Heuristic {
    typedef ReconvergenceEntry::Bitset Bitset;
    
    std::map<ADDRINT, ReconvergenceEntry> bit_table;
    int ref;
    ADDRINT lastsp;
    
    //Parameters
    unsigned int hysteresis;
    unsigned int ignore_threshold;    // threshold = 1 -> Boolean
    bool decrement_counter;
    
  public:
    Sync(UINT nt, Thread* t, const char* param);
    ~Sync(){}
    
    void choose_threads();
};

Heuristic* new_heuristic(UINT nt, Thread* t, const char* param){
    return new Sync(nt, t, param);
}
void delete_heuristic(Heuristic* h){
    delete h;
}


Sync::Sync(UINT nt, Thread* t, const char* param)
: Heuristic(nt, t), lastsp(0)
{
    if (nt > 64){
        throw Exception("This heuristic does not support more than 64 threads due to smaller allocated space.");
    }
    
    char dec_c[10];
    sscanf(param, "%u %u %s", &hysteresis, &ignore_threshold, dec_c );
    decrement_counter = dec_c[0] == 'y' && dec_c[1] == 'e' && dec_c[2] == 's';
    
    ref = -1;
}

void Sync::choose_threads(){
    
    //-Each PC has a bit mask of active threads and a reconvergence bit.
    //-Before executing an instruction:
    //--Choose with preference the threads where the reconvergence bit of the PC is 0.
    //--Compare using AND the bit mask of active threads of the current execution of the instruction with the bit mask of the previous execution, that is stored in the table. If it is 0, then the reconvergence bit of this PC is set to 1.
    //--Update the bit mask of active threads for the chosen PC.
    
#ifdef FASTPATH
    // Need to recompute next PC if SP decreases (min-SP-PC-specific)
    if ( ref != -1 && !t[ref].basic_block && !t[ref].ignore && t[ref].sp >= lastsp){
        for (UINT i=0; i<num_threads; i++){
            t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
        }
        return;
    }
#endif
    
    // Give priority to threads that have the lowest counter (reversed priority)
    unsigned int cnt_reconvergence[num_threads];
    unsigned int min_prio = -1;	// UINT_MAX
    for (UINT i=0; i<num_threads; i++){
        if (!t[i].ignore){
            
			ReconvergenceEntry const & e = bit_table[t[i].pc];
            cnt_reconvergence[i] = e.counter;
            
            min_prio = std::min(min_prio, cnt_reconvergence[i]);
        }
    }
    
    
    //SUB-HEURISTIC
    // Choose min(SP:PC) among threads with lowest counter value
    ref = -1;
    for (UINT i=0; i<num_threads; i++){
        if (!t[i].ignore){
            if (cnt_reconvergence[i] <= min_prio + hysteresis || min_prio >= ignore_threshold) {
                if ( ref == -1 || t[i].sp < t[ref].sp || ( t[i].sp == t[ref].sp && t[i].pc < t[ref].pc ) ){
                    ref = i;
                }
            }
        }
    }
    
    for (UINT i=0; i<num_threads; i++){
        t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
    }
 
    //Comparing the current mask with the previous mask
	ReconvergenceEntry & re = bit_table[t[ref].pc];
    
#if DUMPTRACE
    for (UINT i=0; i<num_threads; i++){
    	std::cerr << re.mask[i];
    }
    std::cerr << ", " << re.counter << ", " << min_prio;
	std::cerr << std::endl;
#endif
    
    bool complement = true;
    bool equals = true;
    for (UINT i=0; i<num_threads; i++){
        if (t[i].is_active && re.mask[i]){
            complement = false;
        }
        else if(t[i].is_active ^ re.mask[i]) {
        	equals = false;
        }
    }
    re.counter += complement;	// Inc when complementary masks,
    if(decrement_counter && re.counter && equals) --re.counter;   // Dec when equal masks
    
    //Atualizando tabela
    for (UINT i=0; i<num_threads; i++){
        re.mask.set(i, t[i].is_active);
    }
    lastsp = t[ref].sp;
    
}
