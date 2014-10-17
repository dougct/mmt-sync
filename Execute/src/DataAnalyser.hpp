#ifndef __HEADER_DATA_ANALYSER__
#define __HEADER_DATA_ANALYSER__

#include "Types.hpp"
#include "Thread.hpp"
#include "StatisticCounter.hpp"

#include <bitset>

class DataAnalyser{
  public:
    static const UINT NUM_BITS = 8*sizeof(void*); 
    typedef LONG Values[NUM_BITS];
    
    Thread* threads;
    UINT num_threads;
    UINT num_fetches;
    
    UINT fetch_counter;
    
    LONG* instructions; //counter
    
    //Memory accesses (for each number of active threads)
    LONG* number_of_accesses;
    LONG* uniform_accesses;
    LONG* affine_accesses;
    LONG* scattered_accesses;
    Values* mem_affine_distance; //Common distance between the address
    Values* mem_scattered_interval; //Interval where all scattered addresses are in
    Values intervalo_comum_embaralhado;
    
    //Executing
    LONG* executing_pertime; //Number of times when each number of active threads happens
    
    LONG* execute_identical; //Number of execute-identical instructions
    
    //Counters
    LONG resulting_size; //Number of instructions fetched
    LONG total_instructions; //Number of instructions executed
    LONG amount_divergence;
    
    //New statistics
    typedef std::bitset<64> Bitset; //helper
    Bitset current_cycle; //helper
    LONG number_cycles;
    
    LONG* inactive_length; //helper
    bool* was_active; //helper
    StatisticCounter* divergence_length;
    
    StatisticCounter divergence_depth;
    
    long long first_thread; //helper
    long long other_thread; //helper
    StatisticCounter div_unbalance;
    StatisticCounter div_differece;
    
    LONG* alone_thread;
    
  public:
    DataAnalyser();
    ~DataAnalyser();
    
    void init(Thread* thrds, UINT nt, UINT nf);
    
    void analyse();
    
    void check_result();
    void print();
    
  private:
    struct CommonVar{
        int first;
        UINT running;
    };
    
    void memory_analysis(CommonVar* cv);
    void registers_analysis(CommonVar* cv);
    void divergence_statistics(CommonVar* cv);
    
};

#endif
