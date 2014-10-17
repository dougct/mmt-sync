# include "Heuristic.hpp"
# include <list>
# include <stdio.h>

class Sync : public Heuristic {
    struct BlocoPragma {
        bool* threads;
        UINT* level_incr;
        int quant;
        UINT identifier;
        UINT level;
    };
    typedef std::list<BlocoPragma>::iterator iterator;
    
    std::list<BlocoPragma> stack;
    UINT* function_level;
    int ref;
    
    UINT tamanho_FHB;
    ADDRINT** tabela_FHB;
    UINT* ultima_pos;
    bool* para_executar;
    short nafila;
    
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
    
    
    function_level = new UINT[nt];
    for (UINT i=0; i<nt; i++){
        function_level[i] = 1;
    }
    
    //Inserting the base of the stack, where all thread are active and synchronized
    BlocoPragma novo;
    novo.identifier = 0;
    novo.quant = nt;
    novo.level_incr = new UINT[nt];
    novo.threads = new bool[nt];
    for (UINT i=0; i<nt; i++){
        novo.threads[i] = true;
        novo.level_incr[i] = 0;
    }
    stack.push_front( novo );
}

Sync::~Sync(){
    delete [] para_executar;
    for (UINT i=0; i<num_threads; i++){
        delete [] tabela_FHB[i];
    }
    delete [] tabela_FHB;
    delete [] ultima_pos;
    
    delete [] function_level;
    delete [] stack.front().threads;
    delete [] stack.front().level_incr;
    stack.pop_front();
}


void Sync::choose_threads(){
    
    //check if a thread found a post-dominator
    for (UINT i=0; i<num_threads; i++){
        if (t[i].is_active && !t[i].ignore){
            
            //check if it is one of the pragmas on the stack for the current function
            for (UINT j=0; j<t[i].pragma_end_counter; j++){
                
                for (iterator it = stack.begin(), end = stack.end(); it != end; ){
                    
                    if ((*it).level < function_level[i]){
                        //it is not on the stack, then ignore.
                        break;
                    }
                    else if ( (*it).identifier == t[i].pragma_end_list[j] && (*it).threads[i]){
                        //it is on the stack, then wait for the other threads
                        
                        (*it).threads[i] = false;
                        (*it).quant--;
                        function_level[i] -= (*it).level_incr[i];
                        
                        //if it is the last thread to achieve this pragma
                        if ((*it).quant == 0) {
                            //All threads are synchronized here, then remove the pragma from the stack
                            delete [] (*it).threads;
                            delete [] (*it).level_incr;
                            it = stack.erase(it);
                            continue;
                        }
                        
                    }
                    it++;
                    
                }
            }
        }
    }
    
    
    //sub-heuristic
    
    //check if a thread found a post-dominator
    for (UINT i=0; i<num_threads; i++){
        if (t[i].is_active && !t[i].ignore){
            if (t[i].pragma_end_counter > 0 || t[i].pragma_begin_list > 0){
                t[i].basic_block = true;
            }
        }
    }
    
    do {
        if ( ref != -1 && !(t[ref].basic_block || t[ref].bb_priority > 0) && !t[ref].ignore ){
            for (UINT i=0; i<num_threads; i++){
                t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
            }
            break;
        }
        
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
        
        bool ret = false;
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
                ret = true;
                break;
            }
        }
        
        if (ret) break;
        
        //Passsou por todos e nenhum estava marcado.
        nafila = 0;
    } while(true);
    
    
    
    if (ref == -1){
        throw Exception("Error detected by heuristic code: No thread was chosen to execute.");
    }
    for (UINT i=0; i<num_threads; i++){
        t[i].is_active = !t[i].ignore && t[i].pc == t[ref].pc;
    }
    
    
    //if it found the beginning of a divergence (branch)
    if (t[ref].pragma_begin_list > 0){
        //check if the pragma is in the stack for the current function
        for (iterator it = stack.begin(), end = stack.end(); it != end; it++){
            if ((*it).level < function_level[ref]){
                //it is not on the stack, then insert this pragma on the stack
                
                BlocoPragma novo;
                novo.identifier = t[ref].pragma_begin_list;
                novo.level = function_level[ref];
                novo.quant = 0;
                novo.level_incr = new UINT[num_threads];
                novo.threads = new bool[num_threads];
                for (UINT i=0; i<num_threads; i++){
                    novo.threads[i] = t[i].is_active;
                    novo.level_incr[i] = function_level[ref] - function_level[i];
                    if (novo.threads[i]){
                        novo.quant++;
                        function_level[i] += novo.level_incr[i];
                    }
                }
                
                stack.push_front(novo);
                
                break;
            }
            else if ( (*it).identifier == t[ref].pragma_begin_list ) {
                //it is already on the stack, then ignore. It was found again due to loop.
                break;
            }
        }
    }
    
    
    //If this instruction calls a function
    if ( t[ref].pragma_fun_call ){
        for (UINT i=0; i<num_threads; i++){
            if (t[i].is_active){
                function_level[i]++;
            }
        }
    }
    //If this instruction returns from a function
    if ( t[ref].pragma_fun_ret ){
        for (UINT i=0; i<num_threads; i++){
            if (t[i].is_active){
                function_level[i]--;
            }
        }
    }
    
}
