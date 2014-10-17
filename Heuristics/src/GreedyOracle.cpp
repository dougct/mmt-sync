#include "Heuristic.hpp"
#include "Warp.hpp"
#include <deque>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <vector>
//#define VERBOSE

#ifdef VERBOSE
#define DUMP(x) std::cerr << #x " = " << (x) << " "
#define DUMP_HEX(x) std::cerr << #x " = " << std::hex << (x) << std::dec << " "
#define INFO(x) std::cerr << x;
#define ENDLINE std::cerr << std::endl
#else
#define DUMP(x)
#define DUMP_HEX(x)
#define INFO(x)
#define ENDLINE
#endif

struct InstructionLocation
{
    uint64_t sequence_number;
    UINT tid;
    
    InstructionLocation(uint64_t seq, UINT tid) :
        sequence_number(seq), tid(tid) {}
};

class Sync : public Heuristic {
  public:
	typedef Warp::Bitset Bitset;
    Sync(UINT nt, Thread* t, const char* param);
    
    ~Sync(){}
    
    virtual void choose_threads();
    virtual void register_instruction(Instruction const & insn, 
        UINT tid, uint64_t sequence);
  private:
    uint64_t sequence_numbers[MAX_THREADS];
    typedef std::deque<uint64_t> SequenceQueue;
    std::vector<std::unordered_map<ADDRINT, SequenceQueue> > timestamps;
    std::vector<SequenceQueue> barriers;
  
    void unregister_instruction(ADDRINT pc, UINT tid);
    uint64_t shortest_distance(ADDRINT pc, unsigned int tid, unsigned int tid_orig);
    static bool lookup(SequenceQueue const & ahead_queue, uint64_t val);
    
    uint64_t fake_horizon;
    double beta;
};

Heuristic* new_heuristic(UINT nt, Thread* t, const char* param) {
    return new Sync(nt, t, param);
}
void delete_heuristic(Heuristic* h) {
    delete h;
}

Sync::Sync(UINT nt, Thread* t, const char* param)
    : Heuristic(nt, t), timestamps(nt), barriers(nt)
{
    for(unsigned int i = 0; i != nt; ++i) {
        sequence_numbers[i] = 0;
    }
    if(sscanf(param, "%lu %lf", &fake_horizon, &beta) < 2) {
        fake_horizon = 1000;
        beta = 0.0;
    }
    DUMP(fake_horizon); DUMP(beta); ENDLINE;
}

bool Sync::lookup(Sync::SequenceQueue const & ahead_queue, uint64_t val)
{
    for(auto it = ahead_queue.begin(); it != ahead_queue.end(); ++it) {
        if(*it == val) {
            return true;
        }
        else if(*it > val) {
            return false;   // Passed target in sorted queue
        }
    }
    return false;
}

uint64_t Sync::shortest_distance(ADDRINT pc, unsigned int tid, unsigned int tid_orig)
{
    // When will be the next time thread tid encounters instruction at pc?
    SequenceQueue & queue = timestamps[tid][pc];
    if(queue.empty()) {  // Never
        return -1;
    }

    assert(queue.front() >= sequence_numbers[tid]);
    
    // Will we have to pass through a barrier to reach this instruction?
    if(!barriers[tid].empty() && barriers[tid].front() <= queue.front()) {
        INFO("Barrier "); DUMP_HEX(pc); DUMP(tid_orig); DUMP(tid); DUMP_HEX(t[tid].pc); DUMP(queue.front()); DUMP(sequence_numbers[tid]); DUMP(barriers[tid].front()); ENDLINE;
        return -1;  // Don't make scheduling decision across barriers
    }
    
    // Compute relative distance from current progress of thread tid
    uint64_t diff = queue.front() - sequence_numbers[tid];
    DUMP_HEX(pc); DUMP(tid_orig); DUMP(tid); DUMP_HEX(t[tid].pc); DUMP(queue.front()); DUMP(sequence_numbers[tid]); DUMP(diff); ENDLINE;
    if(diff > fake_horizon) {
        return -1;    // Ignore matches beyond fake_horizon
    }
    
    // Check length of sequence
    unsigned int lookahead = (unsigned int)(beta * diff);
    if(lookahead >= t[tid_orig].instruction_queue.size()) {
        return -1;  // Too far, ignore
    }
    if(lookahead > 0) {
        ADDRINT ahead_pc = t[tid_orig].instruction_queue[lookahead].pc;
        SequenceQueue & ahead_queue = timestamps[tid][ahead_pc];
        DUMP(lookahead+1); DUMP(ahead_pc); if(!ahead_queue.empty()) DUMP(ahead_queue.front()-queue.front()); ENDLINE;            
        if(!lookup(ahead_queue, lookahead + 1 + queue.front())) {
        //if(ahead_queue.empty() || ahead_queue.front() - queue.front() <= lookahead + 1) {
            // Why off-by-one?
            // Accept earlier match, unless ahead_queue.front() < queue.front()
            return -1;  // Sequence too short: ignore
        }
    }
    return diff;
}

void Sync::choose_threads()
{
	static int rrcounter = 0;
    Bitset noignore;
    for(unsigned int i = 0; i != num_threads; ++i) {
        noignore[i] = !t[i].ignore;
    }
    Warp warps[MAX_THREADS];
    unsigned int nwarps = SplitWarpByPC(warps, noignore, t, num_threads);
    // For each warp,
    //  Lookup in future of threads not in warp
    //  If PC found
    //   Set priority of warp to distance to fist occurence of PC
    //  Else
    //   Set priority to max
    
    uint64_t priorities[MAX_THREADS];
    for(unsigned int i = 0; i != nwarps; ++i) {
        Warp & warp1 = warps[i];
        uint64_t min_distance = -1;
        unsigned int ref = ffs(warp1.mask);
        for(unsigned int j = 0; j != nwarps; ++j) {
            if(i != j) {
                Warp & warp2 = warps[j];
                int tid2 = ffs(warp2.mask);
                uint64_t distance = shortest_distance(warp1.pc, tid2, ref);
                if(distance < min_distance) {
                    min_distance = distance;
                }
            }
        }
        priorities[i] = min_distance;
    }
    
    // Select warps with max priority (round-robin)
	int selectedWarp = -1;
	uint64_t max_priority = 0;
    for(unsigned int i = 0; i != nwarps; ++i)
    {
        unsigned int ii = (i + rrcounter) % nwarps;
        if(priorities[ii] > max_priority) {
            max_priority = priorities[ii];
            selectedWarp = ii;
        }
    }
    rrcounter = (rrcounter + 1) % nwarps;

    assert(selectedWarp >= 0);
    Warp const & w = warps[selectedWarp];
    
	// Update is_active
	for (UINT i=0; i<num_threads; i++){
		if(w.mask[i]) {
			assert(!t[i].ignore);
		}
	    t[i].is_active = w.mask[i];

        // Unregister scheduled instructions
	    if(t[i].is_active) {
	        // Or should be called by Simulation?
	        unregister_instruction(t[i].pc, i);
	    }
	}
}

void Sync::register_instruction(Instruction const & insn,
    UINT tid, uint64_t sequence)
{
    timestamps[tid][insn.pc].push_back(sequence);
    if(insn.barrier) {
        INFO("Registered barrier "); DUMP(tid); DUMP(sequence); ENDLINE;
        barriers[tid].push_back(sequence);
    }
}

void Sync::unregister_instruction(ADDRINT pc,
    UINT tid)
{
    sequence_numbers[tid] = timestamps[tid][pc].front();
    timestamps[tid][pc].pop_front();
    if(!barriers[tid].empty() && barriers[tid].front() <= sequence_numbers[tid]) // Barrier passed, can drop it
    {
        INFO("Unregistered barrier "); DUMP(tid); DUMP(barriers[tid].front()); ENDLINE;
        barriers[tid].pop_front();
    }
}
