/*
 * The simplest recursive quick sort algorithm.
 * 
 */

# include "common.hpp"
# include <stdlib.h>


static void _quick_sort(int *arranjo, int esq, int dir) {
    int Temp;

    int Pivo = arranjo[(esq+dir)/2];

    int i = esq;
    int j = dir;

    do{
        while(arranjo[i] < Pivo) i++;
        while(arranjo[j] > Pivo) j--;
        
        if (!(i < j)) break;
        
        Temp = arranjo[i]; arranjo[i]=arranjo[j]; arranjo[j]=Temp;
        i++; j--;
        
    } while(true);

    if(  esq <  j ) _quick_sort(arranjo, esq, j); 
    if((j+1) < dir) _quick_sort(arranjo, j+1, dir); 
}
void quick_sort(int *arranjo, int size) {
    _quick_sort(arranjo, 0, size-1);
}


int taman = 100;
int** arranjos[128];
int vez[128];

void execute_test(int tid){
    int* arranjo = arranjos[tid][ vez[tid] ];
    vez[tid]++;
    quick_sort(arranjo, taman);
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

