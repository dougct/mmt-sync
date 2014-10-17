#ifndef __HEADER_SIMULATION__
#define __HEADER_SIMULATION__

#include "DataAnalyser.hpp"
#include <vector>

class Thread;
class Heuristic;

class Simulation{
  public:
    struct HeuristicData{
        Heuristic* (*new_heuristic)(UINT, Thread*, const char*);
        void (*delete_heuristic)(Heuristic*);
        const char* parameter;
    };
    
    static unsigned int const readahead_horizon = 1000;
    
  private:
    UINT num_threads;
    Thread* threads;
    DataAnalyser analyser;
    Heuristic* heuristic;
    HeuristicData* heur_data;
    std::vector<uint64_t> horizon_sequence;

    UINT finished; //Number of finished threads
    UINT waiting_in_barrier;
    
    UINT read_instructions();
    void check_valid();
    void read_instruction(Instruction & insn, FILE * file) const;
    void refill();
    
  public:
    Simulation(UINT num_threads, UINT num_fetches, const char** arqvs, HeuristicData* h);
    ~Simulation();
    
    void simulation();
    DataAnalyser* get_analyser(){
        return &analyser;
    }
};

#endif
