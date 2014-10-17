/*
 * This is a loop that is repeated until it finds a condition, then it executes a big function and finishes.
 * 
 * A good optimization would move the 'if' block to the place after the loop.
 * 
 */


# include "common.hpp"


# include "bigfun.cpp"

ULONG resultado = 0;
int array1[400];
int array2[400];

void execute_test(int x){
    int y = 0;
    int i = 0;
    
    if (x >= 0) {
        while (true){
            i++;
            array2[i] = x*array2[i-1];
            if (i == x){
                y = big_function(x);
                break;
            }
            array1[i] = x*array1[i-1];
        }
    }
    resultado = i*x*y;
}

void prepare_test(int tid){
}
