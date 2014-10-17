/*
 * The heap sort algorithm.
 * 
 */

# include "common.hpp"
# include <stdlib.h>


void heap_sort(int *arranjo, int N){ //N -> tamanho
    int pai, filho;
    int Temp;
    int i = N/2;

    while (true){
        if (i > 0){
            i--;
            Temp = arranjo[i];
        } else {
            N--;
            if (N == 0) return;
            Temp = arranjo[N];
            arranjo[N] = arranjo[0];
        }

        pai = i;
        filho = i*2 + 1;

        int controle=0;
        while (filho < N && controle==0){

            if ((filho + 1 < N)  &&  (arranjo[filho + 1] > arranjo[filho]))
                filho++;

            if (arranjo[filho] > Temp) {
                arranjo[pai] = arranjo[filho];
                pai = filho;
                filho = pai*2 + 1;
            } else  controle=1;
        }
        arranjo[pai] = Temp;
    }
}


int taman = 100;
int** arranjos[128];
int vez[128];

void execute_test(int tid){
    int* arranjo = arranjos[tid][ vez[tid] ];
    vez[tid]++;
    heap_sort(arranjo, taman);
}

void prepare_test(int tid){
    int vezes = number_times();
    
    int** arranjo = new int*[vezes];
    
    srand(tid);
    for (int j=0; j<vezes; j++){
        
        int* arr = new int[taman];
        for (int i=0; i<taman; i++){
            arr[i] = rand()%10000 + (rand()%10000)*10000;
        }
        
        arranjo[j] = arr;
    }
    
    arranjos[tid] = arranjo;
    vez[tid] = 0;
}

