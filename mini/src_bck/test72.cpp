/*
 * The shell sort algorithm.
 * 
 */

# include "common.hpp"
# include <stdlib.h>


void shell_sort(int * vet, int size) {
    int i , j , value;
    int gap = 1;
    do {
        gap = 3*gap+1;
    } while(gap < size);
    do {
        gap /= 3;
        for(i = gap; i < size; i++){
            value =vet[i];
            j = i - gap;
            while (j >= 0 && value < vet[j]) {
                vet [j + gap] =vet[j];
                j -= gap;
            }
            vet [j + gap] = value;
        }
    } while ( gap > 1);
}



int taman = 100;
int** arranjos[128];
int vez[128];

void execute_test(int tid){
    int* arranjo = arranjos[tid][ vez[tid] ];
    vez[tid]++;
    shell_sort(arranjo, taman);
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
