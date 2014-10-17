# include <stdio.h>
# include <pthread.h>
# include "common.hpp"

static int nt;
int number_threads(){
    return nt;
}


static int repet = 1;
int number_times(){
    return repet;
}


extern "C" void execute_tests(int tid){
    for (int i=0; i<repet; i++){
        execute_test(tid);
    }
}

void* run_thread(void* arg){
    int tid = *(int*)arg;
    prepare_test(tid);
    execute_tests(tid);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]){
    if (argc < 2 || argc > 3) return -1;
    sscanf(argv[1], "%d", &nt);
    if (argc == 3){
        sscanf(argv[2], "%d", &repet);
    }
    if (nt <= 0) return -1;
    
    int tid[nt];
    pthread_t threads[nt];
    for (int i = 0; i< nt; i++){
        tid[i] = i + 1;
        pthread_create(&threads[i], NULL, run_thread, &tid[i]);
    }
    for (int i = 0; i < nt; i++){
        pthread_join(threads[i], NULL);
    }
    return 0;
}
