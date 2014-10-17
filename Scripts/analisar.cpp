# include <stdio.h>
# include <algorithm>

void pula_ate(FILE* arq, char c){
    while (true){
        char ch = fgetc(arq);
        if (ch == c)
            return;
    }
}
void le_ate(FILE* arq, char c, char* dest){
    int p = 0;
    while (true) {
        char ch = fgetc(arq);
        if (ch == c)
            break;
        dest[p] = ch;
        p++;
    }
    dest[p] = 0;
}


struct Result{
    char heuristica[40];
    unsigned long long size;
    unsigned long long total;
    unsigned long long ciclos;
};

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("Invalid arguments.\n");
        return -1;
    }
    FILE* arq = fopen(argv[1], "r");
    if (arq == NULL){
        printf("Invalid input file.\n");
        return -1;
    }
    
    //Contar número de heurísticas
    pula_ate(arq, '\n');
    pula_ate(arq, '\n');
    int heuristicas = 0;
    while (true){
        char ch =  fgetc(arq);
        if (ch == '\n')
            break;
        heuristicas++;
        pula_ate(arq, '\n');
    }
    fseek(arq, 0, SEEK_SET);
    
    //Contar número de benchmarks
    pula_ate(arq, '\n');
    int benchmarks = 0;
    while (true){
        char ch;
        
        pula_ate(arq, '\n');
        ch = fgetc(arq);
        if (ch == '\n'){
            benchmarks++;
            
            ch = fgetc(arq);
            if (ch == '\n'){
                break;
            }
        }
    }
    fseek(arq, 0, SEEK_SET);
    
    
    //Lendo todos os dados
    Result res[4][benchmarks][heuristicas];
    char  bench_nome[benchmarks][40];
    
    //Para cada número de threads
    for (int k=0; k<4; k++){
        pula_ate(arq, '\n'); //Linha com o número de threads
        
        //Para cada benchmark
        for (int j=0; j<benchmarks; j++){
            le_ate(arq, '\n', bench_nome[j]);
            
            //Para cada heurística
            for (int i=0; i<heuristicas; i++){
                le_ate(arq, ':', res[k][j][i].heuristica);
                fscanf(arq, "%llu", &res[k][j][i].size);
                pula_ate(arq, '/');
                fscanf(arq, "%llu", &res[k][j][i].total);
                pula_ate(arq, '/');
                fscanf(arq, "%llu", &res[k][j][i].ciclos);
                pula_ate(arq, '\n');
            }
            pula_ate(arq, '\n');
            
        }
        pula_ate(arq, '\n');
        pula_ate(arq, '\n');
    }
    
    fclose(arq);
    
    
    
    //Imprimir tabela ordenada com os dados
    
    //Para cada benchmark
    for (int j=0; j<benchmarks; j++){
        
        char filenome[50];
        sprintf(filenome, "%s/%s_table.txt", argv[2], bench_nome[j]);
        FILE* outp = fopen(filenome, "w");
        if (outp == NULL){
            printf("Invalid output folder.\n");
            return -1;
        }
        
        int nt = 1;
        for (int k=0; k<4; k++){
            nt *= 2;
            
            fprintf(outp, "%d threads\n", nt );
            
            unsigned long long smallest_total = (unsigned long long)-1;
            Result resu[heuristicas];
            for (int i=0; i<heuristicas; i++){
                resu[i] = res[k][j][i];
                if ( resu[i].total < smallest_total ){
                    smallest_total = resu[i].total;
                }
            }
            struct _sort{
                static bool comp(const Result& a, const Result& b) {
                    return a.size < b.size;
                }
            };
            std::stable_sort(resu, resu+heuristicas, _sort::comp);
            
            for (int i=0; i<heuristicas; i++){
                
                //Tamanho em relação ao limite inferior
                double ratio = (double)resu[i].size/((double)smallest_total/nt);
                //double ratio = (double)resu[i].size/((double)resu[i].total/nt);
                double tlp = double(resu[i].size) / double(resu[i].ciclos);
                double dlp = double(resu[i].total) / double(resu[i].size);
                
                //Redução de tamanho
                //double ratio = (double)resu[i].size / smallest_total;
                
                fprintf(outp, "%s:  %lf  %lf  %lf\n", resu[i].heuristica, ratio, tlp, dlp );
            }
            fprintf(outp, "\n");
        }
        
        fclose(outp);
    }
    
    
    
    return 0;
}
