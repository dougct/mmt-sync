#ifndef __HEADER_TYPES_H__
#define __HEADER_TYPES_H__

#include <cstdint>

typedef unsigned long ADDRINT; //It has the size of pointer
typedef unsigned long long LONG; //It is 64 bit
typedef unsigned int UINT; //It is 32 bit

class Exception{
    const char* msg;
    bool del;
  public:
    Exception(const char* m) : msg(m), del(false) {}
    Exception(char* m) : msg(m), del(true) {}
    ~Exception(){
        if (del){
            delete [] msg;
        }
    }
    
    const char* get_message() const{
        return msg;
    }
};

#endif
