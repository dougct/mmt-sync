#ifndef WARP_HPP_INCLUDED
#define WARP_HPP_INCLUDED

#include <bitset>

enum { MAX_THREADS = 16 };

struct Warp
{
	typedef std::bitset<MAX_THREADS> Bitset;
	Bitset mask;
	ADDRINT pc;
	
	Warp() : mask(0), pc(0) {}
	Warp(Bitset m, ADDRINT pc) : mask(m), pc(pc) {}
};

// Returns number of subwarps created
// Fill warps[] with subwarps sorted by PC
unsigned int SplitWarpByPC(Warp warps[], Warp::Bitset subset, Thread* t, unsigned int num_threads)
{
	unsigned int nsubwarps = 0;
	
	for(; subset.any(); ++nsubwarps)
	{
		// Compute min(PC)
		ADDRINT minPC = -1;
		for(unsigned int i = 0; i != num_threads; ++i) {
			if(subset[i]) {
				minPC = std::min(minPC, t[i].pc);
			}
		}
		
		warps[nsubwarps].pc = minPC;
		// Select threads with min PC and move them from subset to warps[nsubwarps]
		for(unsigned int i = 0; i != num_threads; ++i) {
			if(subset[i] && t[i].pc == minPC) {
				warps[nsubwarps].mask.set(i);
				subset.reset(i);
			}
		}
	}
	return nsubwarps;
}

int ffs(std::bitset<MAX_THREADS> m)
{
	if(m.none()) return -1;
	int r;
	for(r = 0; !m[r]; ++r) {}
	return r;
}



#endif
