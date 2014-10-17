/*
 * It is a function with a loop where big function is called inside an 'if' statement that is true 50% of the times.
 * 
 */


# include "common.hpp"


# include "bigfun.cpp"

ULONG resultado = 0;

void execute_test(int x){
    int z = 0;
    int i = 0;
    while (i<40){
        if (i%2 == x%2){
            z += big_function(i*x);
        }
        i++;
    }
    
    resultado = z;
}

void prepare_test(int tid){
}
