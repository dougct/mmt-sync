/*
 * This is a loop that is repeated until it finds a condition, then it executes a big function and finishes. It is like a sequential search. It may not find the condition, then the 'if' is a branch where the post-dominator is after the loop.
 * 
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
    
    while (i < 200){
        i++;
        array2[i] = x*array2[i-1];
        if (i == x){
            y = big_function(x);
            break;
        }
        array1[i] = x*array1[i-1];
    }
    resultado = i*x*y;
}

void prepare_test(int tid){
}
