#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.hpp"

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
  long p, ID, NN, QQ, RR, SS;
  ID = (long)threadid;
  p = ID % 2;
  if (p) {
    NN = ID * 1000;
    QQ = ID + 1;
    QQ = NN * QQ;
    QQ++;
    QQ = QQ % 31;
  } else {
    NN = ID * 2000;
    QQ = ID + 2;
  }
  RR = divide(NN, QQ);
  if (p) {
    SS = RR ^ 42;
  } else {
    SS = RR % 3 + 21;
  }
  //printf("Thread ID: %ld, RR: %ld\n", ID, RR);
  pthread_exit(NULL);
  r = (ULONG)SS;
  return r;
}

/*
void test01() {
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   for(t=0; t<NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, threadProg, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

   // Last thing that main() should do
   pthread_exit(NULL);
}
*/

ULONG resultado = 0;

void execute_test(int tid){
    resultado = threadProg(tid);
}

void prepare_test(int tid){
}

/*
What do you think? Can you code this program using pthreads, and then
run it through Teo's framework? Then, can you simulate function
fusion, and re-run it? The program after fusion would be something
like:

void *threadProg(void *threadid) {
  long ID, p, NN, QQ, RR;
  ID = (long)threadid;
  p = ID % 2;
  NN = p ? ID * 1000 : ID * 2000;
  QQ = p ? ID + 1 : ID + 2;
  RR = divide(NN, QQ);
  printf("Thread ID: %ld, RR: %ld\n", ID, RR);
  pthread_exit(NULL);
}
*/

