#include "pin.H"
#include <stdio.h>
#include <string.h>

#define COMPILER_SUPPORT

#include "PragmasCodes.hpp"

const UINT32 MAX_TID = 64 + 1;

static FILE* arq_saida;
static UINT32 contador_statico;

static bool OutFile[MAX_TID];
static UINT8 secao_critica[MAX_TID];

static UINT64 ins_critica[MAX_TID];
static UINT64 ins_sequencial[MAX_TID];
static UINT64 quantidade[MAX_TID];
static UINT64 ins_shared_lib[MAX_TID];

static UINT64 ins_calls[MAX_TID];
static UINT64 ins_rets[MAX_TID];

static bool pthread_barrier_called[MAX_TID];
static bool pthread_exit_called[MAX_TID];
static bool pthread_cancel_called[MAX_TID];
static bool pthread_key_called[MAX_TID];
static bool recursive_work_thread[MAX_TID];
static bool setjmp_called[MAX_TID];
static bool exit_fun_called[MAX_TID];

static bool verificar_fun;
static bool include_shared;

#ifdef COMPILER_SUPPORT
static ADDRINT the_pragma_function_addr = 0;

static bool ignorar_print[MAX_TID];

static bool fun_call_begin[MAX_TID];
static bool fun_call_end[MAX_TID];
static UINT32 dom_begin_list[MAX_TID];
static UINT8 dom_end_counter[MAX_TID];

static UINT64 fun_begin_counter[MAX_TID];
static UINT64 fun_end_counter[MAX_TID];
static UINT64 fun_begin_counter_aux[MAX_TID];
static UINT64 fun_end_counter_aux[MAX_TID];

static struct {
    UINT32 flags;
} stored_delayed_call[MAX_TID];
#endif

#include "../Execute/src/InstructionFlags.hpp"

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "instcount.txt", "specify file name");
KNOB<string> KnobThreadInicial(KNOB_MODE_WRITEONCE, "pintool", "e", " ", "specify thread function");
KNOB<string> KnobIncludeShared(KNOB_MODE_WRITEONCE, "pintool", "s", "false", "ignore shared libraries");


static VOID proc_ins(THREADID tid, UINT32 flags){
    if (OutFile[tid]){
        if ( secao_critica[tid] == 0 ){
            
            #ifdef COMPILER_SUPPORT
            if (fun_call_begin[tid]){
                fun_call_begin[tid] = false;
                fun_begin_counter[tid]++;
            }
            if (fun_call_end[tid]){
                fun_call_end[tid] = false;
                fun_end_counter[tid]++;
            }
            
            dom_begin_list[tid] = 0;
            dom_end_counter[tid] = 0;
            #endif
            
            quantidade[tid]++;
            if ( InstructionFlags::is_function_call(flags) ){
                ins_calls[tid]++;
            }
            if ( InstructionFlags::is_function_ret(flags) ){
                ins_rets[tid]++;
            }
            
        }
        else {
            ins_critica[tid]++;
        }
    }
    else {
        ins_sequencial[tid]++;
    }
}
static VOID docount(THREADID tid, UINT32 flags){
    
    #ifdef COMPILER_SUPPORT
    if (ignorar_print[tid]){
        return;
    }
    
    if ( OutFile[tid] && secao_critica[tid] == 0 ){
        
        if (fun_call_begin[tid]){
            //had found call, but the function does not start with pragma
            fun_call_begin[tid] = false;
            //restore saved instruction
            proc_ins(tid, stored_delayed_call[tid].flags ); //process the delayed instruction "call"
            //Now it will process the current instruction
        }
        
        UINT8 allflags = (UINT8)flags;
        using namespace InstructionFlags;
        if ( (allflags & IsBranch) != 0 || (allflags & IsCall) != 0 ){
            //found a call instruction, then delay it until it is called
            fun_call_begin[tid] = true;
            //save the instruction
            stored_delayed_call[tid].flags = flags;
            return;
        }
        
    }
    #endif
    
    proc_ins(tid, flags);
}
static VOID count_shared(THREADID tid, UINT32 flags){
    if (OutFile[tid]){
        if ( secao_critica[tid] == 0 ){
            ins_shared_lib[tid]++;
            if (include_shared){
                if ( InstructionFlags::is_function_call(flags) ){
                    ins_calls[tid]++;
                }
                if ( InstructionFlags::is_function_ret(flags) ){
                    ins_rets[tid]++;
                }
            }
        }
        else {
            ins_critica[tid]++;
        }
    }
    else {
        ins_sequencial[tid]++;
    }
}

#ifdef COMPILER_SUPPORT
static VOID exec_pragma(THREADID tid){
    ignorar_print[tid] = !ignorar_print[tid];
}
static VOID dopragma(THREADID tid, ADDRINT arg){
    if ( ignorar_print[tid] && OutFile[tid] ){
        if (arg == CODE_IGNORE_BEGIN){
            secao_critica[tid]++;
        }
        else if (arg == CODE_IGNORE_END){
            secao_critica[tid]--;
        }
        else if (secao_critica[tid] == 0){
            if (arg == CODE_FUN_BEGIN){
                fun_begin_counter_aux[tid]++;
                if (fun_call_begin[tid]){
                    //If found call and the pragma, then process the delayed call inserting pragma
                    //restore saved instruction
                    proc_ins(tid, stored_delayed_call[tid].flags); //process the delayed instruction "call"
                }
            }
            else if (arg == CODE_FUN_END){
                fun_end_counter_aux[tid]++;
                fun_call_end[tid] = true;
            }
            else if (arg > MIN_DOM_BEGIN && arg < MAX_DOM_BEGIN){
                arg -= MIN_DOM_BEGIN;
                dom_begin_list[tid] = arg;
            }
            else if (arg > MIN_DOM_END && arg < MAX_DOM_END){
                arg -= MIN_DOM_END;
                dom_end_counter[tid]++;
            }
        }
    }
    
}
#endif


VOID Instruction(INS ins, VOID *v){
    
    RTN ins_fun = INS_Rtn(ins);
    bool rtn_valid = RTN_Valid(ins_fun);
    bool shared_lib = !rtn_valid || !IMG_IsMainExecutable( SEC_Img( RTN_Sec(ins_fun) ) );
    
    #ifdef COMPILER_SUPPORT
    if ( !shared_lib && RTN_Address(ins_fun) == the_pragma_function_addr ){
        return;
    }
    
    if ( !shared_lib && INS_IsProcedureCall(ins) && INS_IsDirectCall(ins) ){
        ADDRINT dest = INS_DirectBranchOrCallTargetAddress(ins);
        if ( dest == the_pragma_function_addr ){
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)exec_pragma,
                IARG_THREAD_ID,
            IARG_END);
            contador_statico -= 1;
            return;
        }
    }
    
    if ( !shared_lib && (INS_Opcode(ins) == XED_ICLASS_MOVQ || INS_Opcode(ins) == XED_ICLASS_MOV ) ){
        REG regi;
        if ( sizeof(ADDRINT) == sizeof(UINT64) ){
            regi = REG_R15;
        }
        else {
            regi = REG_EDX;
        }
        
        
        INS nxt1 = INS_Next(ins);
        INS nxt2 = INS_Next(nxt1);
        if (
            INS_MaxNumWRegs(ins) == 1 && INS_RegW(ins, 0) == regi && INS_MaxNumRRegs(ins) == 0 &&
            INS_Valid(nxt1) && INS_Valid(nxt2) &&
            INS_Opcode(nxt1) == XED_ICLASS_POP &&
            INS_IsProcedureCall(nxt2) && INS_IsDirectCall(nxt2) &&
            INS_DirectBranchOrCallTargetAddress(nxt2) == the_pragma_function_addr
        ){
            INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)dopragma,
                IARG_THREAD_ID,
                IARG_REG_VALUE, regi,
            IARG_END);
            return;
        }
    }
    #endif
    
    
    //Set instruction info
    using namespace InstructionFlags;
    UINT8 flags = 0;
    if( INS_IsBranch(ins) )        flags |= IsBranch;
    if( INS_IsDirectBranch(ins) )  flags |= IsDirectBranch;
    if( INS_IsCall(ins) )          flags |= IsCall;
    if( INS_IsProcedureCall(ins) ) flags |= IsProcedureCall;
    if( INS_IsDirectCall(ins) )    flags |= IsDirectCall;
    if( INS_IsRet(ins) )           flags |= IsRet;
    if( INS_HasFallThrough(ins) )  flags |= HasFallThrough;
    
    //Register the instruction
    if (!shared_lib){
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount,
            IARG_THREAD_ID,
            IARG_UINT32, flags,
        IARG_END);
    }
    else {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_shared,
            IARG_THREAD_ID,
            IARG_UINT32, flags,
        IARG_END);
    }
    
    if (!shared_lib || include_shared){
        contador_statico++;
    }
    
}


VOID mutex_lock(THREADID tid){
    secao_critica[tid]++;
}
VOID mutex_unlock(THREADID tid){
    secao_critica[tid]--;
}
VOID mutex_trylock(THREADID tid, UINT32 ret){
    if (ret == 0){
        mutex_lock(tid);
    }
}
VOID barrier_after(THREADID tid){
    mutex_unlock(tid);
    pthread_barrier_called[tid] = true;
}


VOID fimThread(THREADID tid);
VOID pthread_exit_before(THREADID tid){
    if ( OutFile[tid] ){
        pthread_exit_called[tid] = true;
        fimThread(tid);
    }
}
VOID exit_before(THREADID tid){
    if ( OutFile[tid] ){
        exit_fun_called[tid] = true;
    }
}
VOID pthread_key_before(THREADID tid){
    pthread_key_called[tid] = true;
}
VOID pthread_cancel_before(THREADID tid){
    pthread_cancel_called[tid] = true;
}
VOID setjmp_before(THREADID tid){
    setjmp_called[tid] = true;
}


VOID inicioThread(THREADID tid, ADDRINT sp){
    if(!OutFile[tid]){
        OutFile[tid] = true;
        quantidade[tid] = 0;
        ins_shared_lib[tid] = 0;
    }
    else {
        recursive_work_thread[tid] = true;
    }
}
VOID fimThread(THREADID tid){
    OutFile[tid] = false;
}


VOID Image(IMG img, VOID *v){
    
    fprintf( arq_saida, "%s\n", IMG_Name(img).c_str() );
    
    //If it is pthread library image
    if (RTN_Valid( RTN_FindByName(img, "pthread_create") )){
        
        const char* fun_locks[] = {"pthread_mutex_lock", "pthread_spin_lock", "pthread_rwlock_wrlock", "pthread_rwlock_rdlock" };
        for (int i=0; i<4; i++){
            RTN mutexLock = RTN_FindByName(img, fun_locks[i] );
            if (RTN_Valid(mutexLock)){
                RTN_Open(mutexLock);
                RTN_InsertCall(mutexLock, IPOINT_AFTER, (AFUNPTR)mutex_lock,
                    IARG_THREAD_ID,
                IARG_END);
                RTN_Close(mutexLock);
            }
        }
        
        const char* fun_unlocks[] = {"pthread_mutex_unlock", "pthread_spin_unlock", "pthread_rwlock_unlock" };
        for (int i=0; i<3; i++){
            RTN mutexUnlock = RTN_FindByName(img, fun_unlocks[i]);
            if (RTN_Valid(mutexUnlock)){
                RTN_Open(mutexUnlock);
                RTN_InsertCall(mutexUnlock, IPOINT_BEFORE, (AFUNPTR)mutex_unlock,
                    IARG_THREAD_ID,
                IARG_END);
                RTN_Close(mutexUnlock);
            }
        }
        
        const char* fun_trylocks[] = {"pthread_mutex_trylock", "pthread_spin_trylock", "pthread_rwlock_trywrlock", "pthread_rwlock_tryrdlock" };
        for (int i=0; i<4; i++){
            RTN mutexTrylock = RTN_FindByName(img, fun_trylocks[i]);
            if (RTN_Valid(mutexTrylock)){
                RTN_Open(mutexTrylock);
                RTN_InsertCall(mutexTrylock, IPOINT_AFTER, (AFUNPTR)mutex_trylock,
                    IARG_THREAD_ID,
                    IARG_FUNCRET_EXITPOINT_VALUE,
                IARG_END);
                RTN_Close(mutexTrylock);
            }
        }
        
        //Other
        RTN condWait = RTN_FindByName(img, "pthread_cond_wait");
        if (RTN_Valid(condWait)){
            RTN_Open(condWait);
            RTN_InsertCall(condWait, IPOINT_BEFORE, (AFUNPTR)mutex_lock,
                IARG_THREAD_ID,
            IARG_END);
            RTN_InsertCall(condWait, IPOINT_AFTER, (AFUNPTR)mutex_unlock,
                IARG_THREAD_ID,
            IARG_END);
            RTN_Close(condWait);
        }
        
        RTN barrierWait = RTN_FindByName(img, "pthread_barrier_wait");
        if (RTN_Valid(barrierWait)){
            RTN_Open(barrierWait);
            RTN_InsertCall(barrierWait, IPOINT_BEFORE, (AFUNPTR)mutex_lock,
                IARG_THREAD_ID,
            IARG_END);
            RTN_InsertCall(barrierWait, IPOINT_AFTER, (AFUNPTR)barrier_after,
                IARG_THREAD_ID,
            IARG_END);
            RTN_Close(barrierWait);
        }
        
        RTN t_exit = RTN_FindByName(img, "pthread_exit");
        if (RTN_Valid(t_exit)){
            RTN_Open(t_exit);
            RTN_InsertCall(t_exit, IPOINT_BEFORE, (AFUNPTR)pthread_exit_before,
                IARG_THREAD_ID,
            IARG_END);
            RTN_Close(t_exit);
        }
        
        RTN pt_key = RTN_FindByName(img, "pthread_key_create");
        if (RTN_Valid(pt_key)){
            RTN_Open(pt_key);
            RTN_InsertCall(pt_key, IPOINT_BEFORE, (AFUNPTR)pthread_key_before,
                IARG_THREAD_ID,
            IARG_END);
            RTN_Close(pt_key);
        }
        
        RTN pt_cancel_rtn = RTN_FindByName(img, "pthread_cancel");
        if (RTN_Valid(pt_cancel_rtn)){
            RTN_Open(pt_cancel_rtn);
            RTN_InsertCall(pt_cancel_rtn, IPOINT_BEFORE, (AFUNPTR)pthread_cancel_before,
                IARG_THREAD_ID,
            IARG_END);
            RTN_Close(pt_cancel_rtn);
        }
        RTN pt_kill_rtn = RTN_FindByName(img, "pthread_kill");
        if (RTN_Valid(pt_kill_rtn)){
            RTN_Open(pt_kill_rtn);
            RTN_InsertCall(pt_kill_rtn, IPOINT_BEFORE, (AFUNPTR)pthread_cancel_before,
                IARG_THREAD_ID,
            IARG_END);
            RTN_Close(pt_kill_rtn);
        }
    }
    
    RTN exit_rtn = RTN_FindByName(img, "exit");
    if (RTN_Valid(exit_rtn)){
        RTN_Open(exit_rtn);
        RTN_InsertCall(exit_rtn, IPOINT_BEFORE, (AFUNPTR)exit_before,
            IARG_THREAD_ID,
        IARG_END);
        RTN_Close(exit_rtn);
    }
    RTN setjmp_rtn = RTN_FindByName(img, "setjmp");
    if (RTN_Valid(setjmp_rtn)){
        RTN_Open(setjmp_rtn);
        RTN_InsertCall(setjmp_rtn, IPOINT_BEFORE, (AFUNPTR)setjmp_before,
            IARG_THREAD_ID,
        IARG_END);
        RTN_Close(setjmp_rtn);
    }
    
    if ( IMG_IsMainExecutable(img) ){
        
        //Beginning and the end of a thread
        if (verificar_fun){
            RTN threadInicio = RTN_FindByName(img, KnobThreadInicial.Value().c_str() );
            if (RTN_Valid(threadInicio)){
                RTN_Open(threadInicio);
                RTN_InsertCall(threadInicio, IPOINT_BEFORE, (AFUNPTR)inicioThread,
                    IARG_THREAD_ID,
                    IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);
                RTN_InsertCall(threadInicio, IPOINT_AFTER, (AFUNPTR)fimThread,
                    IARG_THREAD_ID,
                IARG_END);
                RTN_Close(threadInicio);
            }
        }
        
        #ifdef COMPILER_SUPPORT
        the_pragma_function_addr = RTN_Address( RTN_FindByName(img, "the_pragma_function") );
        #endif
    }
    
}


VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v){
    if(threadid >= 0 && threadid < MAX_TID){
        OutFile[threadid] = false;
        secao_critica[threadid] = 0;
        
        ins_sequencial[threadid] = 0;
        ins_critica[threadid] = 0;
        
        ins_calls[threadid] = 0;
        ins_rets[threadid] = 0;
        
        pthread_barrier_called[threadid] = false;
        pthread_exit_called[threadid] = false;
        pthread_cancel_called[threadid] = false;
        pthread_key_called[threadid] = false;
        recursive_work_thread[threadid] = false;
        setjmp_called[threadid] = false;
        exit_fun_called[threadid] = false;
        
        #ifdef COMPILER_SUPPORT
        ignorar_print[threadid] = false;
        
        fun_call_begin[threadid] = false;
        fun_call_end[threadid] = false;
        
        dom_begin_list[threadid] = 0;
        dom_end_counter[threadid] = 0;
        
        fun_begin_counter[threadid] = 0;
        fun_end_counter[threadid] = 0;
        fun_begin_counter_aux[threadid] = 0;
        fun_end_counter_aux[threadid] = 0;
        #endif
        
        if (!verificar_fun){
            inicioThread(threadid, PIN_GetContextReg( ctxt, REG_STACK_PTR) );
        }
        
    }
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v){
    if(threadid >= 0 && threadid < MAX_TID){
        if (OutFile[threadid] || !verificar_fun){
            fimThread(threadid);
        }
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v){
    fprintf(arq_saida, "Instructions: %u\n", contador_statico);
    for (unsigned int i=0; i<MAX_TID; i++){
        if (ins_critica[i] != 0 || ins_sequencial[i] != 0 || quantidade[i] != 0 || ins_shared_lib[i] != 0){
            fprintf(arq_saida, "%u | %llu - %llu %llu %llu | %lld (%lld) ! %lld (%lld) ! %lld (%lld) | %u %u %u %u - %u %u %u |  \n",
                i,
                
                (long long unsigned)quantidade[i],
                (long long unsigned)ins_shared_lib[i],
                (long long unsigned)ins_critica[i],
                (long long unsigned)ins_sequencial[i],
                
                #ifdef COMPILER_SUPPORT
                (long long)(fun_begin_counter[i] - fun_end_counter[i]),
                (long long)(fun_begin_counter[i]),
                (long long)(fun_begin_counter_aux[i] - fun_end_counter_aux[i]),
                (long long)(fun_begin_counter_aux[i]),
                #else
                (long long)0,
                (long long)0,
                (long long)0,
                (long long)0,
                #endif
                (long long)(ins_calls[i] - ins_rets[i]),
                (long long)(ins_calls[i]),
                
                (pthread_barrier_called[i])? 1 : 0,
                (pthread_exit_called[i])? 1 : 0,
                (pthread_cancel_called[i])? 1 : 0,
                (pthread_key_called[i])? 1 : 0,
                
                (recursive_work_thread[i])? 1 : 0,
                (setjmp_called[i])? 1 : 0,
                (exit_fun_called[i])? 1 : 0
                
            );
            
        }
    }
    fclose(arq_saida);
    
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

INT32 Usage(){
    return -1;
}

int main(int argc, char * argv[]){
    PIN_InitSymbols();
    
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();
    
    verificar_fun = KnobThreadInicial.Value() != " ";
    include_shared = KnobIncludeShared.Value() == "true";
    
    arq_saida = fopen(KnobOutputFile.Value().c_str(), "w");
    contador_statico = 0;
    
    // Register Analysis routines to be called when a thread begins/ends
    PIN_AddThreadStartFunction(ThreadStart, NULL);
    PIN_AddThreadFiniFunction(ThreadFini, NULL);
    
    IMG_AddInstrumentFunction(Image, NULL);
    INS_AddInstrumentFunction(Instruction, NULL);
    
    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, NULL);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
