#include "StatisticCounter.hpp"

void StatisticCounter::add(long long val){
    if (quant == 0 || val < min)
        min = val;
    if (quant == 0 || val > max)
        max = val;
    sum += val;
    quant++;
}

double StatisticCounter::average(){
    if (quant == 0) return 0;
    return (double)sum/quant;
}

/*
double deviation(){
    //Requires all values
    
    long long sigma = 0;
    for () {
        int t = (val - average);
        sigma += t*t;
    }
    return sqrt( (double)sigma/quant );
}
*/
