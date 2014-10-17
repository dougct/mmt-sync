/*
 * This tests the capacity to keep the iteration synchronized.
 * 
 * This is a loop with a 'continue' statement inside an 'if' that is executed after the call of a big function.
 * 
 */


# include "common.hpp"


# include "bigfun.cpp"


int array[40];

void execute_test(int x){
    for (int i=0; i<16; i++){
        int z = big_function(i);
        if (i%4 == x%4){
            array[i+1] += z*array[8];
            continue;
        }
        array[i+1] -= 4*array[0] + z;
    }
    array[39] = x;
}

void prepare_test(int tid){
}

