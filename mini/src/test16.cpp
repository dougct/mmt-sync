/*
 * This examples tests if a heuristics can keep the synchronization after an indirect call. There is a big code after that call.
 * 
 */


# include "common.hpp"

int small_fun1(int x);
int small_fun2(int x);

char atribuir[40];
void execute_test(int x){
    int (*fun)(int);
    if (x % 2 == 0){
        fun = small_fun1;
    }
    else {
        fun = small_fun2;
    }
    int h = fun(x);
    
    //Big code
    atribuir[0] = h + x;
    atribuir[1] = h * x;
    atribuir[2] = h - x;
    atribuir[3] = atribuir[2] + h;
    atribuir[4] = atribuir[3] + x;
    atribuir[5] = atribuir[3] * x;
    atribuir[6] = atribuir[4] * h;
    atribuir[7] = atribuir[5] + atribuir[6];
    atribuir[8] = atribuir[7] + 2;
    atribuir[9] = h + x;
    atribuir[10] = h * x;
    atribuir[11] = x - h;
    atribuir[12] = atribuir[10] + atribuir[9];
    atribuir[13] = atribuir[11] + atribuir[8];
    atribuir[14] = atribuir[7] + atribuir[6];
    atribuir[15] = atribuir[12] + atribuir[13];
    atribuir[16] = atribuir[14] + atribuir[15];
    atribuir[17] = atribuir[16] + atribuir[1];
    atribuir[18] = h + x;
    atribuir[19] = h * x;
    atribuir[20] = h - x;
    atribuir[21] = atribuir[2] + h;
    atribuir[22] = atribuir[3] + x;
    atribuir[23] = atribuir[3] * x;
    atribuir[24] = atribuir[4] * h;
    atribuir[25] = atribuir[5] + atribuir[6];
    atribuir[26] = atribuir[7] + 2;
    atribuir[27] = h + x;
    atribuir[28] = h * x;
    atribuir[29] = x - h;
    atribuir[30] = atribuir[10] + atribuir[9];
    atribuir[31] = atribuir[11] + atribuir[8];
    atribuir[32] = atribuir[7] + atribuir[6];
    atribuir[33] = atribuir[12] + atribuir[13];
    atribuir[34] = atribuir[14] + atribuir[15];
    atribuir[35] = atribuir[16] + atribuir[1];
    
}

int small_fun1(int x){
    int z = x*x;
    if (x % 2 == 0){
        z = z+9;
    }
    else {
        z = 9*z;
    }
    return z;
}
int small_fun2(int x){
    int z = x*x;
    if (x % 2 == 0){
        z = z+1;
    }
    else {
        z = 2*z;
    }
    return z;
}


void prepare_test(int tid){
}
