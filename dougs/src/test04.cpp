#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../common/common.hpp"

#define NUM_THREADS 10

int divide(int N, int Q) {
  int R = 0;
  if (Q <= 0) {
    Q = 2;
  }
  while (N > Q) {
    N -= Q;
    R++;
  }
  return R;
}

ULONG threadProg(int threadid) {
  ULONG r = 0;
  long ID, NN, QQ, RR, SS;
  ID = (long)threadid;

  if (ID % 2) {
    NN = 10;
    QQ = 3;
    RR = divide(NN, QQ);
    RR++;
  } else {
    NN = 100;
    QQ = 30;
    RR = divide(NN, QQ);
    RR++;
  }
  pthread_exit(NULL);
  r = (ULONG)SS;
  return r;
}

ULONG resultado = 0;

void execute_test(int tid){
    resultado = threadProg(tid);
}

void prepare_test(int tid){
}


