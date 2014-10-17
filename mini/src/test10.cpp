/*
 * This is a loop with a 'continue' statement inside an 'if'. It calls a big function if the current iteration continues.
 * 
 * This is an example where synchronized iteration will not be able to synchronize the big function call.
 * 
 */


# include "common.hpp"


# include "bigfun.cpp"


int vecto[40];

void execute_test(int x){
    for (int i=0; i<16; i++){
        if (i%4 != x%4){
            vecto[i+1] += 3*vecto[8];
            continue;
        }
        int z = big_function(i);
        vecto[i+1] -= 4*vecto[0] + z;
    }
    vecto[39] = x;
}

void prepare_test(int tid){
}

