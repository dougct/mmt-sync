/*
 * A recursive function of two calls with big code before the recursive call.
 * 
 */

# include "common.hpp"


static int vecto[40];
static ULONG fibonacci(ULONG num, ULONG* Cache){
    
    ULONG result = 1;
    if ( Cache[num] > 0 ){
        result = Cache[num];
    }
    else {
        //big code
        int x = num;
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
        vecto[39] = vecto[29] + x - 1;
        
        if (num > 1){
            result = fibonacci(num-1, Cache) + fibonacci(num-2, Cache);
        }
        Cache[num] = result;
    }
    return result;
}


ULONG resultado = 0;

ULONG* Caches[128];

void execute_test(int tid){
    ULONG *Cache = Caches[tid];
    
    resultado = fibonacci(tid, Cache);
}

void prepare_test(int tid){
    Caches[tid] = new ULONG[10000];
    for (int i=0; i<10000; i++){
        Caches[tid][i] = 0;
    }
}
