#include "DataAnalyser.hpp"

#include <stdio.h>
#include <algorithm>
#include <string.h>

static UINT first_bit(LONG x){
    UINT b = 0;
    while (x != 0){
        x >>= 1;
        b++;
    }
    return b;
}

DataAnalyser::DataAnalyser(){
    memset(this, 0, sizeof(DataAnalyser));
}
DataAnalyser::~DataAnalyser(){
    delete [] instructions;
    
    delete [] number_of_accesses;
    delete [] uniform_accesses;
    delete [] affine_accesses;
    delete [] scattered_accesses;
    
    delete [] mem_affine_distance;
    delete [] mem_scattered_interval;
    
    delete [] executing_pertime;
    delete [] execute_identical;
    
    delete [] inactive_length;
    delete [] divergence_length;
    delete [] was_active;
    delete [] alone_thread;
}

void DataAnalyser::init(Thread* thrds, UINT nt, UINT nf){
    threads = thrds;
    num_threads = nt;
    num_fetches = nf;
    fetch_counter = 0;
    instructions = new LONG[num_threads];
    std::fill(instructions, instructions+num_threads, 0);
    
    number_of_accesses = new LONG[num_threads];
    uniform_accesses = new LONG[num_threads];
    affine_accesses = new LONG[num_threads];
    scattered_accesses = new LONG[num_threads];
    
    mem_affine_distance = new Values[num_threads];
    mem_scattered_interval = new Values[num_threads];
    
    executing_pertime = new LONG[num_threads];
    execute_identical = new LONG[num_threads];
    
    std::fill(number_of_accesses, number_of_accesses+num_threads, 0);
    std::fill(uniform_accesses, uniform_accesses+num_threads, 0);
    std::fill(affine_accesses, affine_accesses+num_threads, 0);
    std::fill(scattered_accesses, scattered_accesses+num_threads, 0);
    
    for (UINT i=0; i<num_threads; i++){
        for (UINT j=0; j<NUM_BITS; j++){
            mem_affine_distance[i][j] = 0;
            mem_scattered_interval[i][j] = 0;
        }
    }
    std::fill(intervalo_comum_embaralhado, intervalo_comum_embaralhado+NUM_BITS, 0);
    
    std::fill(executing_pertime, executing_pertime+num_threads, 0);
    std::fill(execute_identical, execute_identical+num_threads, 0);
    
    resulting_size = 0;
    total_instructions = 0;
    amount_divergence = 0;
    
    number_cycles = 0;
    
    inactive_length = new LONG[num_threads];
    std::fill(inactive_length, inactive_length+num_threads, 0);
    divergence_length = new StatisticCounter[num_threads];
    
    was_active = new bool[num_threads];
    std::fill(was_active, was_active+num_threads, true);
    
    first_thread = 0;
    other_thread = 0;
    
    alone_thread = new LONG[num_threads];
    std::fill(alone_thread, alone_thread+num_threads, 0);
}

void DataAnalyser::analyse(){
    
    int first = -1;
    UINT running = 0;
    for (UINT i=0; i<num_threads; i++){
        if (threads[i].is_active){
            instructions[i]++; // Track individual thread progress
            running++;
            if (first == -1){
                first = i;
            }
        }
        else {
            amount_divergence++;
        }
    }
    
    //Update counters
    resulting_size++;
    total_instructions += running;
    executing_pertime[running-1]++;
    
    
    //Count cycles
    //We still fetch one instruction at a time, but count a cycle only when we start fetching an instruction from the same threads again. (For instance, if the thread scheduling sequence is 1 2 1 2 3 2 2 2 3, we count 5 cycles: (1 2) (1 2 3) (2) (2) (2 3)).
    //Check the limit of simultaneous available fetch units to count cycles.
    fetch_counter++;
    if (fetch_counter == num_fetches){
        current_cycle.reset();
        number_cycles++;
        fetch_counter = 0;
    }
    for (UINT i=first; i<num_threads; i++){
        if (threads[i].is_active){
            if ( current_cycle.test(i) ){
                current_cycle.reset();
                number_cycles++;
                fetch_counter = 0;
            }
            current_cycle.set(i);
        }
    }
    
    
    
    CommonVar cv;
    cv.first = first;
    cv.running = running;
    
    //Specific analysis
    memory_analysis(&cv);
    registers_analysis(&cv);
    divergence_statistics(&cv);
    
}

void DataAnalyser::memory_analysis(CommonVar* cv){
    int first = cv->first;
    UINT running = cv->running;
    
    //Analysis of memory access pattern
    if (threads[first].mem_quant > 0){
        
        UINT running2 = 0;
        UINT tid_executando[num_threads];
        for (UINT i=0; i<num_threads; i++){
            if (threads[i].is_active){
                tid_executando[running2] = i;
                running2++;
            }
        }
        
        for (UINT j=0; j<threads[first].mem_quant; j++){
            
            ADDRINT lista_enderecos[running];
            running2 = 0;
            for (UINT i=0; i<num_threads; i++){
                if (threads[i].is_active){
                    lista_enderecos[running2] = threads[i].mem_address[j];
                    running2++;
                }
            }
            number_of_accesses[running-1]++;
            
            bool igual;
            
            //Check if it is the same address
            igual = true;
            for (UINT i=1; i<running; i++){
                igual = igual && lista_enderecos[i] == lista_enderecos[0];
            }
            if (igual){
                uniform_accesses[running-1]++;
                continue;
            }
            
            //Check if it is affine
            if ( lista_enderecos[1] > lista_enderecos[0] ){
                ADDRINT addr_dist = (lista_enderecos[1] - lista_enderecos[0]);
                UINT tid_dif = (tid_executando[1] - tid_executando[0]);
                ADDRINT difference =  addr_dist/tid_dif;
                
                igual = true;
                for (UINT i=1; i<running; i++){
                    ADDRINT esperado = lista_enderecos[0] + (tid_executando[i] - tid_executando[0])*difference;
                    
                    igual = igual && (esperado == lista_enderecos[i]);
                }
                if (igual){
                    affine_accesses[running-1]++;
                    mem_affine_distance[running-1][ first_bit(difference) - 1 ]++;
                    continue;
                }
            }
            
            //
            std::sort(lista_enderecos, lista_enderecos + running);
            
            //Then the accesses are scattered
            
            //Get the size of the interval
            ADDRINT interval = lista_enderecos[running-1] - lista_enderecos[0];
            
            scattered_accesses[running-1]++;
            mem_scattered_interval[running-1][ first_bit(interval) - 1 ]++;
            
            
            //Check if the distance is the same, despite shuffled.
            if (running == num_threads ){
                igual = true;
                ADDRINT distancia = lista_enderecos[1] - lista_enderecos[0];
                for (UINT i=2; i<running; i++){
                    igual = igual && (lista_enderecos[i] - lista_enderecos[i-1]) == distancia;
                }
                if (igual){
                    intervalo_comum_embaralhado[ first_bit(distancia) - 1]++;
                }
            }
            
        }
    }
}

void DataAnalyser::registers_analysis(CommonVar* cv){
    int first = cv->first;
    UINT running = cv->running;
    
    //Registers comparison to detect execute-identical instructions
    if (threads[first].reg_size > 0){
        
        bool exec_igual = true;
        if (running > 1){
            for (UINT i=first+1; i<num_threads; i++){
                if (threads[i].is_active){ //Para todas as threads em execução
                    if ( memcmp(threads[first].reg_data, threads[i].reg_data, threads[first].reg_size) != 0 ){
                        exec_igual = false;
                        break;
                    }
                }
            }
        }
        if (exec_igual){
            execute_identical[running-1]++;
        }
        
    }
    else {
        execute_identical[running-1]++;
    }    
}

void DataAnalyser::divergence_statistics(CommonVar* cv){
    UINT running = cv->running;
    
    //Length of divergent sections: for each thread, the number of cycles between the time it becomes inactive and the time it becomes active again.
    for (UINT i=0; i<num_threads; i++){
        
        if (was_active[i]){
            if (!threads[i].is_active){
                inactive_length[i] = 0;
            }
        }
        else {
            if (threads[i].is_active){
            	if(inactive_length[i] != 0) {
            		// Only count divergent sections
                	divergence_length[i].add(inactive_length[i]);
                }
                inactive_length[i] = 0;
            }
            else {
                inactive_length[i]++;
            }
        }
        was_active[i] = threads[i].is_active;
    }
    
    //Nesting depth of divergence blocks: We can approximate it with the number of different PCs at a given time.
    ADDRINT sorted_pc[num_threads];
    UINT running2 = 0;
    for (UINT i=0; i<num_threads; i++){
        if (threads[i].is_active){
            sorted_pc[running2] = threads[i].pc;
            running2++;
        }
    }
    std::sort(sorted_pc, sorted_pc+running2);
    UINT different_pc = 1;
    for (UINT i=1; i<running2; i++){
        if (sorted_pc[i] != sorted_pc[i-1]){
            different_pc++;
        }
    }
    divergence_depth.add(different_pc);
    
    //Path length unbalance: Difference in length of divergent paths from the divergence point to the reconvergence point. The length unbalance between (thread 1, but not all threads) and (other threads, but not thread 1).
    if (running == num_threads){
        if (first_thread > 0 || other_thread > 0){
            long long differece = first_thread - other_thread;
            LONG unbalance = (differece >= 0)? differece : -differece;
            
            div_unbalance.add(unbalance);
            div_differece.add(differece);
            
            first_thread = 0;
            other_thread = 0;
        }
    }
    else if (threads[0].is_active && running < num_threads){
        first_thread++;
    }
    else if (!threads[0].is_active){
        other_thread++;
    }
    
    //Threads that are executing alone
    if (running == 1){
        for (UINT i=0; i<num_threads; i++){
            if (threads[i].is_active){
                alone_thread[i]++;
                break;
            }
        }
    }
    
}

void DataAnalyser::check_result(){
    LONG total = 0;
    for (UINT i=0; i<num_threads; i++){
        total += instructions[i];
    }
    
    LONG total_por_vez1 = 0;
    LONG total_por_vez2 = 0;
    for (UINT i=0; i<num_threads; i++){
        total_por_vez1 += executing_pertime[i];
        total_por_vez2 += (i+1)*executing_pertime[i];
    }
    if (total != total_instructions || total_por_vez1 != resulting_size || total_por_vez2 != total){
        char* msg = new char[150];
        sprintf(msg, "Error in the heuristic: Inconsistent result: %llu %llu | %llu %llu/%llu.\n", total_por_vez1, resulting_size, total_por_vez2, total_instructions, total);
        throw Exception(msg);
    }
}

void DataAnalyser::print(){
    //Printing all results
    
    printf("Result");
    printf("Result: %llu / %llu\n", resulting_size, total_instructions);
    printf("Amount of Divergence: %llu / %llu = %.3f\n", amount_divergence, resulting_size*num_threads,
    	double(amount_divergence) / double(resulting_size*num_threads));
    
    printf("Data-Level Paralelism: %llu / %llu = %.3f\n", total_instructions, resulting_size,
    	double(total_instructions) / double(resulting_size));
    printf("Thread-Level Paralelism: %llu / %llu = %.3f\n", resulting_size, number_cycles,
    	double(resulting_size) / double(number_cycles));
    printf("Throughput DLPxTLP: %llu / %llu = %.3f\n", total_instructions, number_cycles,
    	double(total_instructions) / double(number_cycles));
    printf("\n");
    
    
    printf("Number of instructions that each thread executed\n");
    for (UINT i=0; i<num_threads; i++){
        printf("%d %llu\n", i, instructions[i]);
    }
    printf("\n");
    
    
    printf("Number of executing threads per time\n");
    for (UINT i=0; i<num_threads; i++){
        printf("%d %llu\n", i+1, executing_pertime[i]);
    }
    printf("\n");
    
    
    printf("Number of identical executions\n");
    for (UINT i=0; i<num_threads; i++){
        printf("%d %llu\n", i+1, execute_identical[i]);
    }
    printf("\n");
    
    
    printf("Divergence statistics\n");
    printf("Divergence lengths\n");
    for (UINT i=0; i<num_threads; i++){
        printf("%d %lf %llu %llu\n", i, divergence_length[i].average(), divergence_length[i].get_max(), divergence_length[i].get_min() );
    }
    printf("Divergence depth: %lf %llu %llu\n", divergence_depth.average(), divergence_depth.get_max(), divergence_depth.get_min() );
    printf("Divergence unbalance: %lf %llu %llu\n", div_unbalance.average(), div_unbalance.get_max(), div_unbalance.get_min() );
    printf("Divergence length bias: %lf %lld %lld\n", div_differece.average(), div_differece.get_max(), div_differece.get_min() );
    
    printf("Alone threads\n");
    for (UINT i=0; i<num_threads; i++){
        printf("%d %llu\n", i, alone_thread[i] );
    }
    printf("\n");
    
    
    printf("Memory accesses\n");
    printf("- Uniform Affine Scattered Total\n");
    for (UINT i=0; i<num_threads; i++){
        printf(
            "%d %llu %llu %llu %llu\n",
            i+1, uniform_accesses[i], affine_accesses[i], scattered_accesses[i], number_of_accesses[i]
        );
    }
    printf("\n");
    
    
    printf("Affine accesses. Distance between the accesses:\n");
        printf("_ ");
        for (UINT j=0; j<NUM_BITS; j++){
            printf("%d ", j); //Number of bits, expoente
        }
        printf("\n");
    for (UINT i=0; i<num_threads; i++){
        printf("%d ", i+1);//Number de threads
        for (UINT j=0; j<NUM_BITS; j++){
            printf("%llu ", mem_affine_distance[i][j]);
        }
        printf("\n");
    }
    
    printf("Scattered accesses. Divergence interval that contains the accesses:\n");
        printf("_ ");
        for (UINT j=0; j<NUM_BITS; j++){
            printf("%d ", j); //Number de bits, expoente
        }
        printf("\n");
    for (UINT i=0; i<num_threads; i++){
        printf("%d ", i+1);//Number de threads
        for (UINT j=0; j<NUM_BITS; j++){
            printf("%llu ", mem_scattered_interval[i][j]);
        }
        printf("\n");
    }
    
    printf("Scattered common distance with all threads active:\n");
    for (UINT i=0; i<NUM_BITS; i++){
        if (intervalo_comum_embaralhado[i] > 0)
            printf("%d: %llu\n", i, intervalo_comum_embaralhado[i]);
    }
}
