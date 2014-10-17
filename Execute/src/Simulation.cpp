#include "Heuristic.hpp"
#include "Thread.hpp"
#include "DataAnalyser.hpp"
#include "Simulation.hpp"

#include <stdio.h>

//#define DUMPTRACE 42
#define DUMPSTREAM stderr

Simulation::Simulation(UINT nt, UINT nf, const char *arqvs[], HeuristicData* h) :
    horizon_sequence(nt)
{
    num_threads = nt;
    threads = new Thread[num_threads];
    for (UINT i=0; i<num_threads; i++){
        threads[i].load(arqvs[i]);
    }
    
    analyser.init(threads, num_threads, nf);
    
    heur_data = h;
    
    heuristic = heur_data->new_heuristic(num_threads, threads, h->parameter);
    
    finished = 0;
    waiting_in_barrier= 0;
}

Simulation::~Simulation(){
    heur_data->delete_heuristic(heuristic);
    delete [] threads;
}


// Returns number of finished threads
UINT Simulation::read_instructions(){
    
    refill();
    for (UINT i=0; i<num_threads; i++){
        if( !threads[i].finished && threads[i].is_active ){
            
            // Read next instruction from instruction queue
            if(threads[i].instruction_queue.empty()) {
                threads[i].finished = true;
                finished++;
            }
            else {
                Instruction const & insn = threads[i].instruction_queue.front();
                threads[i].fetch(insn);
                threads[i].instruction_queue.pop_front();
                
                if (threads[i].barrier){
                    waiting_in_barrier++;
                    if (waiting_in_barrier == num_threads){
                        waiting_in_barrier = 0;
                        for (UINT k=0; k<num_threads; k++){
                            threads[k].barrier = false;
                            threads[k].ignore = threads[k].finished;
                        }
                    }
                }
            }
            
            threads[i].ignore = threads[i].finished || threads[i].barrier;
        }
    }
    return finished;
}

void Simulation::refill()
{
    for (UINT i=0; i<num_threads; i++){
        if(threads[i].not_eof) {
            Instruction insn;
            while(threads[i].instruction_queue.size() < readahead_horizon
                && (threads[i].not_eof = insn.read(threads[i].thread_arq)))
            {
                threads[i].instruction_queue.push_back(insn);
                heuristic->register_instruction(insn, i,
                    horizon_sequence[i]++);
            }
        }
    }
}

void Simulation::check_valid()
{
    //Check if the result of the heuristic is valid
    int first = -1;
    for (UINT i=0; i<num_threads; i++){
        if ( threads[i].is_active ){
            if (first == -1){
                first = i;
                if (threads[first].mem_quant > 4){
                    throw Exception("Error in the input file. Wrong number of memory addresses.");
                }
            }
            else if ( threads[first].pc != threads[i].pc ){
                throw Exception("Error in the heuristic: simultaneous execution of threads with different instructions.");
            }
            else if (
                threads[first].mem_quant != threads[i].mem_quant ||
                threads[first].reg_size != threads[i].reg_size
            ){
                throw Exception("Error in the input file. It may be problem in the instruction printer.");
            }
        }
    }
    if (first == -1){
        throw Exception("Error in heuristic: No thread was chosen to execute.");
    }
    
#ifdef DUMPTRACE
    // (Print pc and tids of the threads)
    fprintf(DUMPSTREAM, "%.16lx, %ld, ", threads[first].pc, threads[first].sp);
    for(UINT i = 0; i != num_threads; ++i)
    {
        fputc(threads[i].is_active ? '1' : '0', DUMPSTREAM);
    }
    for(UINT i = 0; i != num_threads; ++i)
    {
        fprintf(DUMPSTREAM, ", %lld", analyser.instructions[i]);
    }
    fprintf(DUMPSTREAM, ", ");
    for(UINT i = 0; i != num_threads; ++i)
    {
        fputc(threads[i].ignore ? '1' : '0', DUMPSTREAM);
    }
    
    fputc('\n', DUMPSTREAM);
#endif
}

void Simulation::simulation(){
    
    finished = 0;
    waiting_in_barrier = 0;
    
    // Get the next instruction of the active threads
    while (read_instructions() != num_threads) {
        
        //The heuristic
        heuristic->choose_threads();
        
        check_valid();
        
        //Analysis
        analyser.analyse();
    }
    analyser.check_result();
}

