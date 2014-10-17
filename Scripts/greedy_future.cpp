#include <stdio.h>

typedef unsigned long ADDRINT;

struct ThreadStruct{
    FILE* arq;
    unsigned long long size;
    unsigned long long total;
    
    ThreadStruct(){
        arq = NULL;
        size = 0;
        total = 0;
    }
    ~ThreadStruct(){
        close();
    }
    void close(){
        if (arq != NULL){
            fclose(arq);
        }
        arq = NULL;
    }
};


ADDRINT get_ins(FILE* input){
    ADDRINT ins;
    
    unsigned char* pos = (unsigned char*) &ins;
    for (unsigned int i=0; i<sizeof(ADDRINT); i++){
        pos[i] = fgetc(input);
    }
    for (unsigned int i=0; i<(sizeof(ADDRINT) + 3*sizeof(char)); i++){
        fgetc(input);
    }
    
    if (feof(input)) return 0; //0 means end of file
    else return ins;
}

void print_ins(FILE* outp, ADDRINT ins){
    unsigned char* pos = (unsigned char*) &ins;
    for (unsigned int i=0; i<sizeof(ADDRINT); i++){
        fputc(pos[i], outp);
    }
    for (unsigned int i=0; i<(sizeof(ADDRINT) + 3*sizeof(char)); i++){
        fputc(0, outp);
    }
}


void merge2(ThreadStruct* resu, ThreadStruct* t1, ThreadStruct* t2){
    FILE* thread1 = t1->arq;
    FILE* thread2 = t2->arq;
    FILE* temp = tmpfile();
    resu->arq = temp;
    
    while(true){
        ADDRINT ins1 = get_ins( thread1 );
        ADDRINT ins2 = get_ins( thread2 );
        
        if (ins1 == 0){
            while (true){
                ins2 = get_ins( thread2 );
                if (ins2 == 0) goto end_func;
                print_ins(temp, ins2);
                resu->size++;
                resu->total++;
            }
        }
        else if (ins2 == 0){
            while (true){
                ins1 = get_ins( thread1 );
                if (ins1 == 0) goto end_func;
                print_ins(temp, ins1);
                resu->size++;
                resu->total++;
            }
        }
        
        ADDRINT inst1 = ins1;
        ADDRINT inst2 = ins2;
        
        if (ins1 != ins2){
            unsigned long long min_next_1 = 0;
            unsigned long long min_next_2 = 0;
            
            unsigned long long next_1 = 0;
            unsigned long long next_2 = 0;
            
            unsigned long long file_pos_1 = ftello64( thread1 );
            while (true){
                unsigned long long file_pos_2 = ftello64( thread2 );
                while (true){
                    if (ins1 == ins2){
                        if( (min_next_1 == 0 && min_next_2 == 0) || (next_1 + next_2 < min_next_1 + min_next_2) ){
                            min_next_1 = next_1;
                            min_next_2 = next_2;
                        }
                        break;
                    }
                    
                    ins2 = get_ins( thread2 );
                    if (ins2 == 0){
                        break;
                    }
                    next_2++;
                }
                fseeko64(thread2, file_pos_2, SEEK_SET);
                
                if ( !(min_next_1 == 0 && min_next_2 == 0) && next_1 >= (min_next_1 + min_next_2) ){
                    break;
                }
                
                ins1 = get_ins( thread1 );
                if (ins1 == 0){
                    break;
                }
                next_1++;
            }
            fseeko64(thread1, file_pos_1, SEEK_SET);
            
            resu->size += min_next_1 + min_next_2;
            resu->total += min_next_1 + min_next_2;
            if (min_next_1 > 0){
                print_ins(temp, inst1);
                for (unsigned long long i=0; i<min_next_1-1; i++){
                    ins1 = get_ins( thread1 );
                    print_ins(temp, ins1);
                }
            }
            if (min_next_2 > 0){
                print_ins(temp, inst2);
                for (unsigned long long i=0; i<min_next_2-1; i++){
                    ins2 = get_ins( thread2 );
                    print_ins(temp, ins2);
                }
            }
            
        }
        else {
            print_ins(temp, ins1);
            resu->size++;
            resu->total += 2;
        }
    }
    
    end_func:;
    
    fseeko64(temp, 0, SEEK_SET);
    return;
}

void merge(ThreadStruct* resu, ThreadStruct* thread, int x){
    if (x == 1){
        *resu = thread[0];
        thread[0].arq = NULL;
        return;
    }
    if (x == 2){
        merge2(resu, &thread[0], &thread[1]);
        return;
    }
    
    ThreadStruct re1;
    ThreadStruct re2;
    
    merge(&re1, thread, x/2);
    merge(&re2, thread+x/2, x-x/2);
    
    merge2(resu, &re1, &re2);
    
    return;
}


int main(int argc, char *argv[]){
    int nt = argc - 1;
    if (argc <= 0) return -1;
    
    ThreadStruct input[nt];
    for (int i=0; i<nt; i++){
        input[i].arq = fopen(argv[i+1], "r");
        input[i].size = 0;
        if (input[i].arq == NULL) return -1;
    }
    
    ThreadStruct resu;
    merge(&resu, input, nt);
    
    
    printf("%llu / %llu\n", resu.size, resu.total);
    
    return 0;
}
