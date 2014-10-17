#include "Heuristic.hpp"
#include "Warp.hpp"
#include <map>
#include <bitset>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>

//#define VERBOSE
//#define DUMPTRACE
//#define FASTPATH

using std::hex;
using std::dec;

struct InstructionState
{
	typedef std::bitset<MAX_THREADS> Bitset;
	Bitset lastWarp;
	int progress;
	unsigned int missCounter;
	int origin;

	InstructionState() :
		progress(0), missCounter(0), origin(-1) {
		lastWarp.set();
	}
	InstructionState(Bitset w, int p, unsigned int c, int o) :
		lastWarp(w), progress(p), missCounter(c), origin(o) {}
};

struct WarpState
{
	ADDRINT lastDivPC;
	int progress;
	int target;
	int origin;
	
	WarpState() :
		lastDivPC(0), progress(-1), target(-1), origin(-1) {}
};

enum { MAX_ORIGINS = 2 };

struct DivergencePoint
{
	int reconvDistance[MAX_ORIGINS];
	
	DivergencePoint() {
		for(unsigned int i = 0; i != MAX_ORIGINS; ++i) reconvDistance[i] = -1;
	}
};

class Sync : public Heuristic {
	typedef InstructionState::Bitset Bitset;
	enum SubHeuristic { PolRDMinSPPC=0, PolRoundRobin=1 };
	std::map<ADDRINT, InstructionState> insnStates;	// Indexed by PC
	std::vector<WarpState> warpStates;	// Indexed by warp mask (yes, it gets big for >16 threads)
	std::map<ADDRINT, DivergencePoint> divergencePoints;	// Indexed by PC
	Warp lastWarp;					// To detect divergence/reconvergence
	int ref;
	SubHeuristic subheuristic;
	bool greedy;
	unsigned int unbalance_limit;

	static unsigned int const hysteresis = 0;
	static unsigned int const ignore_threshold = 8;
	static bool const decrement_counter = false;
	static bool const inherit_target = true;
	static bool const ignore_phantoms = true;
	static bool const compensate_overshoot = true;
	
	// Mutually exclusive
	static bool const fix_passedrecv = false;	// Trust active thread
	static bool const fix_otherwarp_target = false;	// Trust divergence miss detection

	void Divergence(Warp const & originalWarp, Warp warps[], unsigned int nWarps);
	void Reconvergence(ADDRINT pc, Bitset mask1, Bitset mask2);
	void MissedReconvergence(InstructionState & is, Warp const & warp1);
	void Schedule();
	void UpdateStates(Warp const & warp);
	int RDMinSPPC(Warp & warp, Warp const allwarps[], unsigned int nwarps);
	int RoundRobin(Warp & warp, Warp const allwarps[], unsigned int nwarps);
	
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


Sync::Sync(UINT nt, Thread* t, const char* param) :
	Heuristic(nt, t),
	lastWarp((1ull << nt) - 1, 0),	// All set
	ref(-1),
	subheuristic(PolRDMinSPPC)
{
	unsigned int mygreedy;
    sscanf(param, "%u %u %d", (unsigned int *)&subheuristic, &mygreedy, &unbalance_limit);
    greedy = mygreedy;
	warpStates.resize(1 << nt);
}


void Sync::choose_threads()
{
#ifdef FASTPATH
	// Fast path: inside a basic block?
	if(ref != -1 && !t[ref].basic_block && !t[ref].ignore) {
		for(UINT i=0; i<num_threads; i++) {
			t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
		}
		// Count all instructions for now. May work with counting BB only too.
	    ++warpStates[lastWarp.mask.to_ulong()].progress;
	    lastWarp.pc = t[ref].pc;
		return;
	}
#endif
	
	assert(lastWarp.mask.any());
	
	// Diverge
	//  different PCs in threads of lastWarp?
	Warp newWarps[MAX_THREADS];
	unsigned int nNewWarps = SplitWarpByPC(newWarps, lastWarp.mask, t, num_threads);
	assert(nNewWarps > 0);
	
	// Reconverge
	//  same PC in thread not in newWarps?
	for(unsigned int j = 0; j != nNewWarps; ++j) {
		ADDRINT pc = newWarps[j].pc;
		Bitset mask = newWarps[j].mask;
		Bitset newmask(0);
		// Look for other threads with same PC
		for(unsigned int i = 0; i != num_threads; ++i) {
			if(!mask[i] && !t[i].ignore && t[i].pc == pc) {
				newmask.set(i);
			}
		}
		if(newmask.any()) {
			Reconvergence(pc, mask, newmask);
			// Merge with current warp
			newWarps[j].mask |= newmask;
		}
	}
	
	// Handle divergence
	if(nNewWarps > 1) {
		Divergence(lastWarp, newWarps, nNewWarps);
	}
	
	// Select next warp
	Schedule();
    
    UpdateStates(lastWarp);
}

void Sync::UpdateStates(Warp const & warp)
{
	WarpState & ws = warpStates[warp.mask.to_ulong()];
    // Update progress
    int progress = ++ws.progress;
	// Update instruction state
	InstructionState & is = insnStates[warp.pc];
  
	if(ws.target != -1 && ws.target - ws.progress < 0) {
#ifdef VERBOSE
		std::cerr << "Passed expected reconvergence point at " << ws.progress << " / " << ws.target << std::endl;
#endif
		// Repare optimistic prediction
		if(fix_passedrecv) { ws.target = ws.progress; }
	}
    
#ifdef DUMPTRACE
    for (UINT i=0; i<num_threads; i++){
    	std::cerr << is.lastWarp[i];
    }
    std::cerr << ", " << is.missCounter;
	std::cerr << std::endl;
#endif

	if((is.lastWarp & warp.mask).none()) {
		++is.missCounter;
		MissedReconvergence(is, warp);	// Side effects : replace mask
	}
	else if(decrement_counter && is.missCounter > 0 && is.lastWarp == warp.mask) {
		--is.missCounter;
	}

	is.lastWarp = warp.mask;
	is.progress = progress;
	is.origin = ws.origin;
}

int Sync::RDMinSPPC(Warp & warp, Warp const allwarps[], unsigned int nwarps)
{
	// Consider in order
	// - progress == -1
	// - min(missCounter)
	// - min(SP)
	// - min(PC)
	unsigned int min_prio = -1;	// UINT_MAX
	int minprogress = 0;
	ADDRINT minsp = -1;	// ADDRINT_MAX
	ADDRINT minpc = -1;
	int bestwarp = -1;
	for(unsigned int i = 0; i != nwarps; ++i)
	{
		Warp const & warp = allwarps[i];
		WarpState const & ws = warpStates[warp.mask.to_ulong()];
		if(minprogress >= 0 || ws.progress == -1) {
			minprogress = ws.progress;
			InstructionState const & e = insnStates[warp.pc];
			if (e.missCounter <= min_prio + hysteresis || min_prio >= ignore_threshold) {
				min_prio = std::min(min_prio, e.missCounter);
				ADDRINT sp = t[ffs(warp.mask)].sp;
				if(sp <= minsp) {
					minsp = sp;
					if(warp.pc < minpc) {
						minpc = warp.pc;
						bestwarp = i;
					}
				}
			}
		}
	}
	warp = allwarps[bestwarp];
	return ffs(warp.mask);
}

int Sync::RoundRobin(Warp & warp, Warp const allwarps[], unsigned int nwarps)
{
	static int rrcounter = -1;
	rrcounter = (rrcounter + 1) % nwarps;
	// Prefer warps whose target == -1
	unsigned int wid = rrcounter;
	
	// Can we find a better one?
    for(unsigned int ii = 0; ii != nwarps; ++ii) {
    	unsigned int i = (ii + rrcounter) % nwarps;
    	WarpState & ws = warpStates[allwarps[i].mask.to_ulong()];
		if(ws.target == -1) {
			wid = i;
			break;
		}
	}
	warp = allwarps[wid];
	rrcounter = wid;
	return ffs(warp.mask);
}

void Sync::Schedule()
{
	// Sets ref, lastWarp as a side effect
	// Select next warp
	// Re-form all warps
	Bitset noignore;
	for(unsigned int i = 0; i != num_threads; ++i) {
		noignore[i] = !t[i].ignore;
	}

	Warp warps[MAX_THREADS];
	unsigned int nwarps = SplitWarpByPC(warps, noignore, t, num_threads);

	// Attempt to find warp with work to do
	// - Greedy policy: round-robin between eligible warps
	// - Max-slack policy: warp with latest deadline first
	static int rrcounter = 0;
	int maxslack = 0;
	int selectedWarp = -1;
    for(unsigned int i = 0; i != nwarps; ++i) {
    	unsigned int ii = greedy ? (i + rrcounter) % nwarps : i;
    	WarpState & ws = warpStates[warps[ii].mask.to_ulong()];
		if(ws.target != -1 && ws.target - ws.progress > maxslack) {
    		maxslack = ws.target - ws.progress;
    		selectedWarp = ii;
			if(greedy) break;
    	}
    }
	
	if(greedy) {
		rrcounter = (rrcounter + 1) % nwarps;
	}

    if(selectedWarp != -1) {
    	Warp & w = warps[selectedWarp];
#ifdef VERBOSE
		WarpState const & ws = warpStates[w.mask.to_ulong()];
		std::cerr << "Scheduled warp w/ progress " << ws.progress << " / " << ws.target << std::endl;
#endif
		lastWarp = w;
    	// Get ID of first thread of warp
    	ref = ffs(w.mask);
    	// Update is_active
		for (UINT i=0; i<num_threads; i++){
			if(w.mask[i]) {
				assert(!t[i].ignore);
			}
		    t[i].is_active = w.mask[i];
		}
    } else {
    	// None found. Fall back to sub-heuristic
    	switch(subheuristic) {
    	case PolRDMinSPPC:
			ref = RDMinSPPC(lastWarp, warps, nwarps);
			break;
		case PolRoundRobin:
			ref = RoundRobin(lastWarp, warps, nwarps);
			break;
		default:
			assert(false);
		}
		
		// Update is_active flags
		for (UINT i=0; i<num_threads; i++){
			if(lastWarp.mask[i]) {
				assert(!t[i].ignore);
			}
			t[i].is_active = lastWarp.mask[i];
		}
		
#ifdef VERBOSE
		//std::cerr << "Fallback: scheduled thread " << ref << ", " << lastWarp.mask << std::endl;
#endif
		
    }
}

void Sync::MissedReconvergence(InstructionState & is, Warp const & warp1)
{
	Bitset mask1 = warp1.mask;
	Bitset mask2 = is.lastWarp;
	WarpState & ws1 = warpStates[mask1.to_ulong()];
	WarpState & ws_parent = warpStates[(mask1 | mask2).to_ulong()];
	WarpState & ws2 = warpStates[mask2.to_ulong()];
	int origin2 = is.origin;

#ifdef VERBOSE
	std::cerr << "Missed reconvergence at " << std::hex << warp1.pc << " from " << ws_parent.lastDivPC << std::dec << std::endl;
	std::cerr << " between " << mask1 << " " << ws1.progress << " / " << ws1.target
		<< " edge " << ws1.origin << " div " << hex << ws1.lastDivPC << dec << std::endl;
	std::cerr << " and " << mask2 << " " << is.progress << " edge " << origin2 << std::endl;
	std::cerr << " Other warp now at " << ws2.progress << " / " << ws2.target
		<< " edge " << ws2.origin << std::endl;
#endif


	if(ws2.progress < is.progress) {
		if(ignore_phantoms) {
#ifdef VERBOSE
			std::cerr << " Other warp is behind. Ignoring." << std::endl;
#endif
			return;
		}
		else if(ws2.target != -1 && ws2.target <= ws2.progress && ws2.origin == origin2) {
#ifdef VERBOSE
			std::cerr << " Other warp is stuck behind. Adjusting target to let it catch up." << std::endl;
#endif
			ws2.target = is.progress;
			return;
		}
		else if(ws1.target == -1)
		{
#ifdef VERBOSE
			std::cerr << " Other warp is behind. Waiting." << std::endl;
#endif
			ws1.target = ws1.progress;
			return;
		}
	}
	else if(std::abs(ws2.progress - ws1.progress) > unbalance_limit) {
#ifdef VERBOSE
			std::cerr << " Unbalance too high. Ignoring." << std::endl;
#endif
		return;
	}

	if(origin2 != !ws1.origin) {
#ifdef VERBOSE
		std::cerr << " Same/mismatched origins " << ws1.origin << " and " << origin2 << ". Ignoring." << std::endl;
#endif
		return;
	}
	
	if(ws_parent.lastDivPC == 0) {
#ifdef VERBOSE
		std::cerr << " Unable to find divergence point for mask " << (mask1 | mask2) << ". Ignoring." << std::endl;
#endif
	}
	else
	{
	
		// Selectively update divergence point data
		// Ignore subsequent missed reconvergence events
		DivergencePoint & divPt = divergencePoints[ws_parent.lastDivPC];
		// Try to minimize distance on path 2 (which overshooted)
		if(divPt.reconvDistance[origin2] == -1 || divPt.reconvDistance[origin2] > is.progress) {
#ifdef VERBOSE
			std::cerr << " Updating distance of divergence point @" << std::hex << ws_parent.lastDivPC << std::dec
				<< " from " << divPt.reconvDistance[origin2] << " to " << is.progress << std::endl;
#endif
			divPt.reconvDistance[origin2] = is.progress;
		}
#ifdef VERBOSE
		else {
			std::cerr << " Already fixed. Ignoring" << std::endl;
		}
#endif
	}

	if(ws2.target == -1 || (fix_otherwarp_target && ws2.target > is.progress)) {
		if(ws2.origin == origin2) {
#ifdef VERBOSE
			std::cerr << " Updating target of other warp from " << ws2.target << " to " << is.progress << std::endl;
#endif
			ws2.target = is.progress;
		}
#ifdef VERBOSE
		else {
			std::cerr << " Other warp on different path. Not updating." << std::endl;
		}
#endif
	}
}

void Sync::Reconvergence(ADDRINT pc, Sync::Bitset mask1, Sync::Bitset mask2)
{
	// Write back observed distances to divergence point state
	// We handle one instruction at a time -> no more than 2 warps can converge
	// (e.g. no 3-way reconvergence)
	// Hence we only need to consider 2 warp states
	WarpState & ws1 = warpStates[mask1.to_ulong()];
	WarpState & ws2 = warpStates[mask2.to_ulong()];
	WarpState & ws_parent = warpStates[(mask1 | mask2).to_ulong()];
	//assert(ws_parent.lastDivPC != 0);	// May trigger on unstructured CF!

#ifdef VERBOSE
	std::cerr << "Reconvergence at " << std::hex << pc << std::dec << std::endl;
	std::cerr << " between " << mask1 << " progress=" << ws1.progress << " target=" << ws1.target << " origin=" << ws1.origin << std::endl;
	std::cerr << " and " << mask2 << " progress=" << ws2.progress << " target=" << ws2.target << " origin=" << ws2.origin << std::endl;
#endif
	
	if(ws_parent.lastDivPC == 0) {
#ifdef VERBOSE
		std::cerr << " WARNING: Unable to find divergence point for mask " << (mask1 | mask2) << " (opportunistic reconvergence or unstructured control flow)." << std::endl;
#endif
	}
	else {
		// Update divergence point data
		// No benefit in marking recv points we already got right!
		// Just set upper bound on unknown or pessimistic path lengths
		DivergencePoint & divPt = divergencePoints[ws_parent.lastDivPC];
		if(ws1.origin < MAX_ORIGINS
			&& (divPt.reconvDistance[ws1.origin] == -1 || divPt.reconvDistance[ws1.origin] > ws1.progress)) {
			int newtarget = ws1.progress;
			int overshoot = ws2.progress - ws2.target;
			if(compensate_overshoot && ws2.target != -1 && overshoot > 0) {
				newtarget = ws1.progress - overshoot;
			}
#ifdef VERBOSE
			std::cerr << " Updating distance of divergence point @" << std::hex << ws_parent.lastDivPC << std::dec
				<< " edge " << ws1.origin << " from " << divPt.reconvDistance[ws1.origin] << " to " << newtarget << std::endl;
#endif
			divPt.reconvDistance[ws1.origin] = newtarget;
		}

		if(ws2.origin < MAX_ORIGINS
			&& (divPt.reconvDistance[ws2.origin] == -1 || divPt.reconvDistance[ws2.origin] > ws2.progress)) {
#ifdef VERBOSE
			std::cerr << " Updating distance of divergence point @" << std::hex << ws_parent.lastDivPC << std::dec
				<< " edge " << ws2.origin<< " from " << divPt.reconvDistance[ws2.origin] << " to " << ws2.progress << std::endl;
#endif
			divPt.reconvDistance[ws2.origin] = ws2.progress;
		}
	}

	// Recombine
	ws_parent.progress += std::max(ws1.progress, ws2.progress);	// Or sum?
	// ws_parent.target = std::max(ws_parent.target, std::max(ws1.target, ws2.target));		// May be -1
	// ws_parent.lastDivPC, .origin, .target stay as before
	
	// Invalidate inconsistent data
	ws1.lastDivPC = 0; ws1.progress = -1; ws1.target = -1; ws1.origin = -1;
	ws2.lastDivPC = 0; ws2.progress = -1; ws2.target = -1; ws2.origin = -1;
#ifdef VERBOSE
	std::cerr << " Revived warp: " << ws_parent.progress << " / " << ws_parent.target << " from " << ws_parent.origin << std::endl;
#endif
}

void Sync::Divergence(Warp const & originalWarp, Warp warps[], unsigned int nWarps)
{
	// Remember divergence point
	warpStates[originalWarp.mask.to_ulong()].lastDivPC = originalWarp.pc;
	DivergencePoint divPt = divergencePoints[originalWarp.pc];
	WarpState const & myws = warpStates[originalWarp.mask.to_ulong()];
	
#ifdef VERBOSE
	std::cerr << "Divergence at " << std::hex << originalWarp.pc << std::dec << std::endl;
#endif
	
	// Init new warps
	for(unsigned int i = 0; i != nWarps; ++i) {
		WarpState & ws = warpStates[warps[i].mask.to_ulong()];
		ws.progress = 0;
		ws.target = i < MAX_ORIGINS ? divPt.reconvDistance[i] : -1;
		if(inherit_target && ws.target == -1 && myws.target != -1) {
			ws.target = std::max(myws.target - myws.progress, -1);	// In the worst case, should reconverge at the parent recv pt
			// If parent is lost (passed expected recv), reset target to unknown (-1)
		}
		ws.origin = i;
#ifdef VERBOSE
		std::cerr << " origin " << i << ": " << std::hex << warps[i].pc << std::dec << " " << warps[i].mask << " target=" << ws.target << std::endl;
#endif
	}
}

