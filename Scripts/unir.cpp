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
    char linha[100];
    char nome[100];
    double size;
};

int main(int argc, char* argv[]){
    FILE* arq1 = fopen(argv[1], "r");
    FILE* arq2 = fopen(argv[2], "r");
    FILE* arq3 = fopen(argv[3], "w");
    if (arq1 == NULL || arq2 == NULL || arq3 == NULL){
        return -1;
    }
    
    //Contar número de heurísticas de cada arquivo
    int heusr1 = 0;
    int heusr2 = 0;
    pula_ate(arq1, '\n');
    while (true){
        char ch = fgetc(arq1);
        if (ch == '\n')
            break;
        heusr1++;
        pula_ate(arq1, '\n');
    }
    fseek(arq1, 0, SEEK_SET);
    
    pula_ate(arq2, '\n');
    while (true){
        char ch = fgetc(arq2);
        if (ch == '\n')
            break;
        heusr2++;
        pula_ate(arq2, '\n');
    }
    fseek(arq2, 0, SEEK_SET);
    
    
    int tot = heusr1 + heusr2;
    
    for (int j=0; j<4; j++){
        char sequen[100];
        le_ate(arq1, '\n', sequen);
        le_ate(arq2, '\n', sequen);
        
        fprintf(arq3, "%s\n", sequen);
        
        Result results[tot];
        
        for (int i=0; i<heusr1; i++){
            int p = i;
            le_ate(arq1, '\n', results[p].linha );
            sscanf(results[p].linha, "%s %lf", results[p].nome, &results[p].size);
        }
        for (int i=0; i<heusr2; i++){
            int p = i + heusr1;
            le_ate(arq2, '\n', results[p].linha );
            sscanf(results[p].linha, "%s %lf", results[p].nome, &results[p].size);
        }
        
        struct _sort{
            static bool comp(const Result& a, const Result& b) {
                return a.size < b.size;
            }
        };
        std::stable_sort(results, results+tot, _sort::comp);
        
        for (int i=0; i<tot; i++){
            fprintf(arq3, "%s\n", results[i].linha);
        }
        
        pula_ate(arq1, '\n');
        pula_ate(arq2, '\n');
        fprintf(arq3, "\n");
    }
    
    return 0;
}

