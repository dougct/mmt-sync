/*
 * An example where MaxSP will fail.
 * 
 * Control flow graph: A->B,C; B->D,E; C calls a function; C->D; D->E (A is the branch and E is the post-dominator)
 * 
 * The basic block D has a big code that would be executed twice by MaxSP.
 */

# include "common.hpp"


int afunction(int x){
    return x*2+2;
}


static int vecto[40];

void execute_test(int x){
    int save_var = 1;
    int z = 2*x + 2;
    if (x % 2 == 0){
        x += afunction(z);
        vecto[0] = x*z;
    }
    else {
        vecto[1] = x - z;
        if (x % 4 == 1 ){
            vecto[2] = z + x;
        }
        else {
            vecto[33] = vecto[32];
            goto final_code;
        }
    }
    
    save_var = -x;
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
    
    final_code:
    vecto[39] = vecto[12] + z + x - 1;
}

void prepare_test(int tid){
}
 
