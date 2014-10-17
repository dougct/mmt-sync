#include "Simulation.hpp"

#include <stdio.h>
#include <dlfcn.h>

int main(int argc, const char** argv){
    /* PARAMETERS:
     * 1 - Mode (what to print as result)
     * 2 - Number of fetch units (0 means the number of threads)
     * 3 - Heuristic file
     * 4 - Heuristic parameter
     * 5 - This and the next files are the list of instructions of each thread.
     */
    int num_threads = argc - 5;
    if (num_threads < 1){
        printf("Invalid number of parameters: %d threads.\n", num_threads);
        return -1;
    }
    
    int mode = 0;
    sscanf(argv[1], "%d", &mode);
    
    UINT num_fetches;
    sscanf(argv[2], "%d", &num_fetches);
    if (num_fetches == 0){
        num_fetches = num_threads;
    }
    
    
    //Load heuristic
    void* lib_handle = dlopen(argv[3], RTLD_LAZY);
    if (lib_handle == NULL){
        printf("%s\n", dlerror());
        return -1;
    }
    
    Simulation::HeuristicData heur;
    
    heur.new_heuristic = (Heuristic* (*)(UINT, Thread*, const char*)) dlsym(lib_handle, "new_heuristic");
    if (heur.new_heuristic == NULL){
        printf("%s\n", dlerror());
        return -1;
    }
    
    heur.delete_heuristic = (void (*)(Heuristic*)) dlsym(lib_handle, "delete_heuristic");
    if (heur.delete_heuristic == NULL){
        printf("%s\n", dlerror());
        return -1;
    }
    
    heur.parameter = argv[4];
    
    try {
        //Simulation
        Simulation simulation(num_threads, num_fetches, &argv[5], &heur);
        simulation.simulation();
        
        DataAnalyser* da = simulation.get_analyser();
        
        //Print result
        switch (mode){
            case 0:
                da->print();
                break;
                
            case 1:
                printf("%llu / %llu / %llu\n", da->resulting_size, da->total_instructions, da->number_cycles);
                break;
        }
        
    }
    catch(Exception& excp){
        printf("%s\n", excp.get_message());
        return -1;
    }
    
    dlclose(lib_handle);
    
    return 0;
}
