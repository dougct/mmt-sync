/*
 * The iterative quick sort algorithm.
 * 
 */

# include "common.hpp"
# include <stdlib.h>


void quick_sort2(int *arranjo, int tamanho) {
    int Temp, Pivo;
    int i, j, esq, dir;

    int Pilha[tamanho];
    int Apt = 2;
    Pilha[0] = 0;         //esq
    Pilha[1] = tamanho-1; //dir
    while (Apt>0){
        Apt--; dir = Pilha[Apt];
        Apt--; esq = Pilha[Apt];

        i = esq;
        j = dir;
        Pivo = arranjo[(esq+dir)/2];
        
        do{
            while(arranjo[i] < Pivo) i++;
            while(arranjo[j] > Pivo) j--;
            
            if (!(i < j)) break;
            
            Temp = arranjo[i]; arranjo[i]=arranjo[j]; arranjo[j]=Temp;
            i++; j--;
        } while(true);

        if(  esq <  j ){
            Pilha[Apt] = esq; Apt++; //esq
            Pilha[Apt] = j  ; Apt++; //dir
        }
        if((j+1) < dir){
            Pilha[Apt] = j+1; Apt++; //esq
            Pilha[Apt] = dir; Apt++; //dir
        }
    }
}


int taman = 100;
int** arranjos[128];
int vez[128];

void execute_test(int tid){
    int* arranjo = arranjos[tid][ vez[tid] ];
    vez[tid]++;
    quick_sort2(arranjo, taman);
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

