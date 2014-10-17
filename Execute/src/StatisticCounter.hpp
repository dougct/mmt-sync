#ifndef __HEADER_StatisticCounter_H__
#define __HEADER_StatisticCounter_H__

#include "Types.hpp"

class StatisticCounter{
    long long min;
    long long max;
    long long quant;
    long long sum;
    
  public:
    StatisticCounter() : quant(0), sum(0) {}
    
    void add(long long val);
    double average();
    
    long long get_max(){
        return max;
    }
    long long get_min(){
        return min;
    }
};


#endif
