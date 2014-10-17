/*
 * This function calls the same big function in both 'then' and 'else' of an 'if' statement.
 * The PC of big function is smaller than the calling function.
 * 
 * MinSP-PC cannot synchronize the call, but Long can.
 * 
 */

# include "common.hpp"


# include "bigfun.cpp"

int colateral_a = 0;
int colateral_b = 0;
int colateral_c = 0;
int colateral_d = 0;

void execute_test(int x){
    if (x % 2 == 0){
        colateral_a = 21*x - x/4;
        int y = big_function(x - colateral_a);
        colateral_b = y*y;
    }
    else {
        colateral_c = x/2 + 3*x;
        int y = big_function(x + colateral_c);
        colateral_d = (y << 3);
    }
}

void prepare_test(int tid){
}

