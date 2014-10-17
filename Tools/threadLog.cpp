#include "pin.H"
#include <stdio.h>

static FILE* OutFile;
static PIN_MUTEX outLock;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "threads.txt", "specify file name");

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v){
    PIN_MutexLock(&outLock);
    fprintf(OutFile, "inicio %d\n", threadid);
    PIN_MutexUnlock(&outLock);
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v){
    PIN_MutexLock(&outLock);
    fprintf(OutFile, "fim %d\n", threadid);
    PIN_MutexUnlock(&outLock);
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v){
    fclose(OutFile);
    PIN_MutexFini(&outLock);
}

VOID func_thread(VOID *fun){
    PIN_MutexLock(&outLock);
    fprintf(OutFile, "pthread_create %s %lu\n", RTN_FindNameByAddress((ADDRINT)fun).c_str(), (ADDRINT)fun);
    PIN_MutexUnlock(&outLock);
}

VOID Image(IMG img, VOID *v){
    
    RTN create = RTN_FindByName(img, "pthread_create");
    if (RTN_Valid(create)){
        RTN_Open(create);
        
        RTN_InsertCall(create, IPOINT_BEFORE, (AFUNPTR)func_thread,
            IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
        IARG_END);
        RTN_Close(create);
    }
    
}

INT32 Usage()
{
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[]){

    PIN_InitSymbols();

    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();
    
    OutFile = fopen( KnobOutputFile.Value().c_str(), "w");
    PIN_MutexInit( &outLock );
    
    // Register Analysis routines to be called when a thread begins/ends
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);
    
    IMG_AddInstrumentFunction(Image, 0);
    
    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}

