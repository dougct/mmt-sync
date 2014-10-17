#include "Heuristic.hpp"
#include <stdio.h>

class Sync : public Heuristic {
    UINT tamanho_FHB;
    ADDRINT** tabela_FHB;
    UINT* ultima_pos;
    bool* para_executar;
    short nafila;
    short ref;
    
  public:
    Sync(UINT nt, Thread* t, const char* param);
    ~Sync();
    
    void choose_threads();
};

Heuristic* new_heuristic(UINT nt, Thread* t, const char* param){
    return new Sync(nt, t, param);
}
void delete_heuristic(Heuristic* h){
    delete h;
}


Sync::Sync(UINT nt, Thread* t, const char* param)
: Heuristic(nt, t)
{
    sscanf(param, "%d", &tamanho_FHB);
    
    ref = -1;
    nafila = 0;
    para_executar = new bool[num_threads];
    
    //Criando a FHB de cada thread
    tabela_FHB = new ADDRINT*[num_threads];
    ultima_pos = new UINT[num_threads];
    for (UINT i=0; i<num_threads; i++){
        ultima_pos[i] = 0;
        tabela_FHB[i] = new ADDRINT[tamanho_FHB];
        for (UINT j=0; j<tamanho_FHB; j++){
            tabela_FHB[i][j] = 0;
        }
    }
}

Sync::~Sync(){
    delete [] para_executar;
    for (UINT i=0; i<num_threads; i++){
        delete [] tabela_FHB[i];
    }
    delete [] tabela_FHB;
    delete [] ultima_pos;
}


void Sync::choose_threads(){
    
    do {
        
        if (nafila == 0){
            
            //Procurar o PC de cada thread na FHB das outras e calcular a prioridade.
            int prioridade[num_threads];
            for (UINT k=0; k<num_threads; k++){//Para cada thread
                prioridade[k] = 0; //A prioridade de cada thread.
                for (UINT i=0; i<num_threads; i++){ //Para cada uma das demais threads
                    if (i != k){
                        for (UINT j=0; j<tamanho_FHB; j++){ //Para cada posição da FHB de outra thread
                            if (t[k].pc == tabela_FHB[i][j]){ //O PC está na tabela
                                prioridade[k]++;
                                break;
                            }
                        }
                    }
                }
            }
            
            //Escolher thread de maior prioridade
            int primeiro = -1;
            for (UINT i=0; i<num_threads; i++){
                if (!t[i].ignore){
                    if (primeiro == -1 || prioridade[i] > prioridade[primeiro]){
                        primeiro = i;
                    }
                }
            }
            
            //Ver quais threads tem a mesma prioridade da maior, e colocar para executar cada uma delas por vez. 
            for (UINT i=0; i<num_threads; i++){
                para_executar[i] = false;
                if (!t[i].ignore){
                    if (prioridade[i] == prioridade[primeiro]){
                        para_executar[i] = true;
                    }
                }
            }
            
        }
        
        //Zerando lista em execução
        for (UINT j=0; j<num_threads; j++){
            t[j].is_active = false;
        }
        
        ref = -1;
        //Marcando threads em execução
        for (UINT j=nafila; j<num_threads; j++){
            if ( para_executar[j] ){
                for (UINT i=j; i<num_threads; i++){
                    t[i].is_active = !t[i].ignore && t[i].pc == t[j].pc;
                    
                    if (t[i].is_active){
                        if (ref == -1)
                            ref = i;
                        
                        para_executar[i] = false;
                        
                        //Adicionando instrução na tabela FHB
                        tabela_FHB[i][ ultima_pos[i] ] = t[i].pc;
                        ultima_pos[i]++;
                        ultima_pos[i] %= tamanho_FHB;
                    }

                }
                nafila = j+1;
                nafila %= num_threads;
                return;
            }
        }
        
        //Passsou por todos e nenhum estava marcado.
        nafila = 0;
    } while(true);
    
}
