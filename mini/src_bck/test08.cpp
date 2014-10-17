/*
 * This is a loop with a 'break' statement. The loop is followed by a big code. It allows to test heuristics that uses the branch predictor.
 * The heuristics should wait all threads after executing 'break' and before executing the big code.
 * 
 */


# include "common.hpp"


int vecto[40];

void execute_test(int x){
    for (int i=0; i<100; i++){
        if (i%16 == x%16){
            vecto[i%40] += 2*vecto[8];
            break;
        }
        vecto[i%40] -= 4*vecto[0];
    }
    
    //Big code
    int save_var = -x;
    if (save_var < 0 ) vecto[0] += x;
    else vecto[0] -= x;
    if (save_var < 0 ) vecto[9] -= 3*x;
    else vecto[9]++;
    if (save_var < 0 ) vecto[7] += 4 + x;
    else vecto[7]++;
    if (save_var < 0 ) vecto[8] -= x + vecto[7];
    else vecto[8] -= vecto[6];
    if (save_var < 0 ) vecto[6] += x * vecto[0];
    else vecto[6] -= vecto[3]; 
    if (save_var < 0 ) vecto[4] -= x*x;
    else vecto[0] += vecto[6];
    if (save_var < 0 ) vecto[5] += x - 7;
    else vecto[19] += 19*vecto[20];
    if (save_var < 0 ) vecto[3] -= x + 3;
    else vecto[18] += 17*vecto[21];
    if (save_var < 0 ) vecto[1] += x*7;
    else vecto[27] += 16*vecto[22];
    if (save_var < 0 ) vecto[2] -= x/2;
    else vecto[17] += 15*vecto[23];
    if (save_var < 0 ) vecto[10] -= x%2;
    else vecto[18] += 14*vecto[24];
    if (save_var < 0 ) vecto[11] += x*x*2;
    else vecto[15] += 13*vecto[25];
    if (save_var < 0 ) vecto[14] -= x - vecto[11];
    else vecto[14] += 12*vecto[26];
    if (save_var < 0 ) vecto[20] += x - vecto[2]*2;
    else vecto[23] += 11*vecto[27];
    if (save_var < 0 ) vecto[19] -= x - 44;
    else vecto[11] += 3*vecto[28];
    if (save_var < 0 ) vecto[17] += x + 66;
    else vecto[10] += 5*vecto[29];
    if (save_var < 0 ) vecto[18] -= x + vecto[17];
    else vecto[29] += 6*vecto[28];
    if (save_var < 0 ) vecto[15] += x*vecto[15];
    else vecto[30] += 7*vecto[29];
    if (save_var < 0 ) vecto[12] -= x/4;
    
    vecto[39] = vecto[12] + x;
}

void prepare_test(int tid){
}
