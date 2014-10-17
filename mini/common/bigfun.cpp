
int _save_var = -1;
static int vetor[40];
int big_function(int x){
    int save_var = _save_var;
    if (save_var < 0 ) vetor[0] += x;
    else vetor[0] -= x;
    if (save_var < 0 ) vetor[9] -= 3*x;
    else vetor[9]++;
    if (save_var < 0 ) vetor[7] += 4 + x;
    else vetor[7]++;
    if (save_var < 0 ) vetor[8] -= x + vetor[7];
    else vetor[8] -= vetor[6];
    if (save_var < 0 ) vetor[6] += x * vetor[0];
    else vetor[6] -= vetor[3]; 
    if (save_var < 0 ) vetor[4] -= x*x;
    else vetor[0] += vetor[6];
    if (save_var < 0 ) vetor[5] += x - 7;
    else vetor[19] += 19*vetor[20];
    if (save_var < 0 ) vetor[3] -= x + 3;
    else vetor[18] += 17*vetor[21];
    if (save_var < 0 ) vetor[1] += x*7;
    else vetor[27] += 16*vetor[22];
    if (save_var < 0 ) vetor[2] -= x/2;
    else vetor[17] += 15*vetor[23];
    if (save_var < 0 ) vetor[10] -= x%2;
    else vetor[18] += 14*vetor[24];
    if (save_var < 0 ) vetor[11] += x*x*2;
    else vetor[15] += 13*vetor[25];
    if (save_var < 0 ) vetor[14] -= x - vetor[11];
    else vetor[14] += 12*vetor[26];
    if (save_var < 0 ) vetor[20] += x - vetor[2]*2;
    else vetor[23] += 11*vetor[27];
    if (save_var < 0 ) vetor[19] -= x - 44;
    else vetor[11] += 3*vetor[28];
    if (save_var < 0 ) vetor[17] += x + 66;
    else vetor[10] += 5*vetor[29];
    if (save_var < 0 ) vetor[18] -= x + vetor[17];
    else vetor[29] += 6*vetor[28];
    if (save_var < 0 ) vetor[15] += x*vetor[15];
    else vetor[30] += 7*vetor[29];
    if (save_var < 0 ) vetor[12] -= x/4;
    
    return vetor[5] + vetor[6];
}
