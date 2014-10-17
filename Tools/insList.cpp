#include "pin.H"
#include <stdio.h>
#include <string.h>

#define COMPILER_SUPPORT

#include "PragmasCodes.hpp"

const UINT32 MAX_TID = 64 + 1;

static FILE* OutFile[MAX_TID];
static ADDRINT sp_base[MAX_TID];
static UINT8 secao_critica[MAX_TID];
static bool hard_basic_block_begin[MAX_TID];
static bool barrier_flag[MAX_TID];

static bool verificar_fun;
static bool include_shared;
static bool full_analysis;

#ifdef COMPILER_SUPPORT
static UINT32 basic_block_priority[MAX_TID];

static ADDRINT the_pragma_function_addr = 0;

static bool ignorar_print[MAX_TID];
static bool delayed_branch[MAX_TID];

static bool fun_call_begin[MAX_TID];
static bool fun_call_end[MAX_TID];

static UINT32 dom_begin_list[MAX_TID];
static UINT8 dom_end_counter[MAX_TID];
static UINT32 dom_end_list[MAX_TID][256];

static struct {
    ADDRINT pc;
    ADDRINT sp;
    UINT32 flags;
    UINT32 memops;
    UINT32 regops;
    UINT32 basic_block_priority;
} stored_delayed_call[MAX_TID];

static UINT8 stored_delayed_memory_quan[MAX_TID];
static ADDRINT stored_delayed_memory[MAX_TID][4];
static UINT8 stored_delayed_regis_quan[MAX_TID];
static UINT8 stored_delayed_regis[MAX_TID][256];
#endif

#include "../Execute/src/InstructionFlags.hpp"

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "insList", "specify file name");
KNOB<string> KnobThreadInicial(KNOB_MODE_WRITEONCE, "pintool", "e", " ", "specify thread function");
KNOB<string> KnobIncludeShared(KNOB_MODE_WRITEONCE, "pintool", "s", "false", "include shared libraries");
KNOB<string> KnobFullAnalysis(KNOB_MODE_WRITEONCE, "pintool", "f", "false", "print memory addresses and registers");

static void writef(FILE* arq, void* ptr, UINT32 size){
    unsigned char* pos = (unsigned char*) ptr;
    for (UINT32 i=0; i<size; i++){
        fputc(pos[i], arq);
    }
}

static VOID proc_ins(THREADID tid, ADDRINT pc, ADDRINT sp, UINT32 flags, UINT32 memops, UINT32 regops){
    if (OutFile[tid] != NULL && secao_critica[tid] == 0){
        
        ADDRINT max = (ADDRINT)-1024;
        sp = (max - (sp_base[tid] - sp));
        
        UINT8 mems = (UINT8)memops; //mems includes some flags
        if (barrier_flag[tid]){
            mems += 128;
            barrier_flag[tid] = false;
        }
        if (hard_basic_block_begin[tid]){
            mems += 64;
            hard_basic_block_begin[tid] = false;
        }
        
        #ifdef COMPILER_SUPPORT
        if (fun_call_begin[tid]){
            mems += 32;
            fun_call_begin[tid] = false;
        }
        if (fun_call_end[tid]){
            mems += 16;
            fun_call_end[tid] = false;
        }
        
        bool pragmas = dom_begin_list[tid] > 0 || dom_end_counter[tid] > 0;
        if (pragmas){
            if (delayed_branch[tid]){
                delayed_branch[tid] = false;
                pragmas = false;
            }
            else {
                mems += 8;
            }
        }
        #endif
        
        UINT8 regs = (UINT8)regops;
        UINT8 allflags = (UINT8)flags;
        
        writef(OutFile[tid], &pc, sizeof(ADDRINT) );
        writef(OutFile[tid], &sp, sizeof(ADDRINT) );
        writef(OutFile[tid], &mems, sizeof(UINT8) );
        writef(OutFile[tid], &regs, sizeof(UINT8) );
        writef(OutFile[tid], &allflags, sizeof(UINT8) );
        
        #ifdef COMPILER_SUPPORT
        writef(OutFile[tid], &basic_block_priority[tid], sizeof(UINT32) );
        basic_block_priority[tid] = 0;
        
        if (pragmas){
            writef(OutFile[tid], &dom_begin_list[tid], sizeof(UINT32) );
            dom_begin_list[tid] = 0;
            
            writef(OutFile[tid], &dom_end_counter[tid], sizeof(UINT8) );
            for (UINT32 i=0; i<dom_end_counter[tid]; i++){
                writef(OutFile[tid], &dom_end_list[tid][i], sizeof(UINT32) );
            }
            dom_end_counter[tid] = 0;
        }
        #endif
        
        if ( InstructionFlags::is_end_hardware_block(allflags) ){
            hard_basic_block_begin[tid] = true;
        }
    }
}

VOID print_delayed_ins(THREADID tid){
    
    //restore saved instruction
    UINT32 temp = basic_block_priority[tid];
    basic_block_priority[tid] = stored_delayed_call[tid].basic_block_priority;
    proc_ins(tid,
        stored_delayed_call[tid].pc,
        stored_delayed_call[tid].sp,
        stored_delayed_call[tid].flags,
        stored_delayed_call[tid].memops,
        stored_delayed_call[tid].regops
    ); //process the delayed instruction "call"
    basic_block_priority[tid] = temp;
    
    //Memory accesses
    for (UINT8 i = 0; i < stored_delayed_memory_quan[tid]; i++){
        writef(OutFile[tid], &(stored_delayed_memory[tid][i]), sizeof(ADDRINT));
    }
    stored_delayed_memory_quan[tid] = 0;
    
    //Registers
    writef(OutFile[tid], &(stored_delayed_regis[tid]), stored_delayed_regis_quan[tid]);
    stored_delayed_regis_quan[tid] = 0;
}

VOID doprint(THREADID tid, ADDRINT pc, ADDRINT sp, UINT32 flags, UINT32 memops, UINT32 regops){
    #ifdef COMPILER_SUPPORT
    if (ignorar_print[tid]){
        return;
    }
    
    if (OutFile[tid] != NULL && secao_critica[tid] == 0 ){
        
        if (fun_call_begin[tid]){
            //had found call, but the function does not start with pragma
            fun_call_begin[tid] = false;
            delayed_branch[tid] = true;
            
            //process the delayed instruction "call"
            print_delayed_ins(tid);
            
            //Now it will process the current instruction
        }
        
        UINT8 allflags = (UINT8)flags;
        using namespace InstructionFlags;
        if ( (allflags & IsBranch) != 0 || (allflags & IsCall) != 0 ){
            //found a call instruction, then delay it until it is called
            fun_call_begin[tid] = true;
            //save the instruction
            stored_delayed_call[tid].pc = pc;
            stored_delayed_call[tid].sp = sp;
            stored_delayed_call[tid].flags = flags;
            stored_delayed_call[tid].memops = memops;
            stored_delayed_call[tid].regops = regops;
            stored_delayed_call[tid].basic_block_priority = basic_block_priority[tid];
            return;
        }
        
    }
    #endif
    
    proc_ins(tid, pc, sp, flags, memops, regops);
}

#ifdef COMPILER_SUPPORT
static VOID exec_pragma(THREADID tid){
    if (OutFile[tid] != NULL && secao_critica[tid] == 0){
        ignorar_print[tid] = !ignorar_print[tid];
    }
}
static VOID dopragma(THREADID tid, ADDRINT arg){
    if ( ignorar_print[tid] && OutFile[tid] != NULL ){
        if (arg == CODE_IGNORE_BEGIN){
            secao_critica[tid]++;
        }
        else if (arg == CODE_IGNORE_END){
            secao_critica[tid]--;
        }
        else if (secao_critica[tid] == 0){
            if (arg > BASIC_BLOCK_PRIORITY && arg < BASIC_BLOCK_PRIORITY_MAX){
                basic_block_priority[tid] = arg - BASIC_BLOCK_PRIORITY;
            }
            else if (arg == CODE_FUN_BEGIN){
                if (fun_call_begin[tid]){
                    //If found call and the pragma, then process the delayed call inserting pragma
                    
                    //process the delayed instruction "call"
                    print_delayed_ins(tid);
                    
                }
            }
            else if (arg == CODE_FUN_END){
                fun_call_end[tid] = true;
            }
            else if (arg > MIN_DOM_BEGIN && arg < MAX_DOM_BEGIN){
                arg -= MIN_DOM_BEGIN;
                dom_begin_list[tid] = arg;
            }
            else if (arg > MIN_DOM_END && arg < MAX_DOM_END){
                arg -= MIN_DOM_END;
                dom_end_list[tid][ dom_end_counter[tid] ] = arg;
                dom_end_counter[tid]++;
            }
        }
    }
    
}
#endif


VOID print_memory(THREADID tid, ADDRINT addre, ADDRINT sp){
    if (!ignorar_print[tid] && OutFile[tid] != NULL && secao_critica[tid] == 0){
        if (addre <= sp_base[tid] && addre >= sp){
            //O endreço está na pilha de execução, por isso é melhor usar valor relativo para achar acesso afim.
            addre = tid*8*1024*1024 + (sp_base[tid] - addre);
        }
        
        if (fun_call_begin[tid]){
            //delayed call
            stored_delayed_memory[tid][ stored_delayed_memory_quan[tid] ] = addre;
            stored_delayed_memory_quan[tid]++;
        }
        else {
            writef(OutFile[tid], &addre, sizeof(ADDRINT));
        }
    }
}

VOID print_register_data(THREADID tid, void* ptr, UINT32 size){
    if (fun_call_begin[tid]){
        //delayed call
        unsigned char* pos = (unsigned char*) ptr;
        for (UINT32 i=0; i<size; i++){
            stored_delayed_regis[tid][ stored_delayed_regis_quan[tid] ] = pos[i];
            stored_delayed_regis_quan[tid]++;
        }
    }
    else {
        writef(OutFile[tid], ptr, size);
    }
}

VOID print_register(THREADID tid, ADDRINT reg){
    if (!ignorar_print[tid] && OutFile[tid] != NULL && secao_critica[tid] == 0){
        print_register_data(tid, &reg, sizeof(ADDRINT));
    }
}
VOID print_register_ref(THREADID tid, ADDRINT reg, UINT32 taman){
    if (!ignorar_print[tid] && OutFile[tid] != NULL && secao_critica[tid] == 0){
        print_register_data(tid, (void*)reg, taman);
    }
}
VOID print_register_stack(THREADID tid, ADDRINT reg){
    if (!ignorar_print[tid] && OutFile[tid] != NULL && secao_critica[tid] == 0){
        ADDRINT max = (ADDRINT)-1024;
        reg = (max - (sp_base[tid] - reg));
        print_register_data(tid, &reg, sizeof(ADDRINT));
    }
}
VOID print_register_87(THREADID tid, CONTEXT* ctxt){
    if (!ignorar_print[tid] && OutFile[tid] != NULL && secao_critica[tid] == 0){
        FPSTATE fpstate;
        memset(&fpstate.fxsave_legacy, 0, sizeof(FXSAVE));
        PIN_GetContextFPState(ctxt, &fpstate);
        print_register_data(tid, &fpstate.fxsave_legacy, (int)( (UINT64)&fpstate.fxsave_legacy._sts[0] - (UINT64)&fpstate.fxsave_legacy ) );
        // ./pin/extras/components/include/util/intel-fp.hpp
    }
}
VOID print_register_st(THREADID tid, CONTEXT* ctxt, UINT32 reg){
    if (!ignorar_print[tid] && OutFile[tid] != NULL && secao_critica[tid] == 0){
        FPSTATE fpstate;
        PIN_GetContextFPState(ctxt, &fpstate);
        print_register_data(tid, &fpstate.fxsave_legacy._sts[reg]._fp, 10);
    }
}


VOID Instruction(INS ins, VOID *v){
    
    RTN ins_fun = INS_Rtn(ins);
    bool rtn_valid = RTN_Valid(ins_fun);
    bool shared_lib = !rtn_valid || !IMG_IsMainExecutable( SEC_Img( RTN_Sec(ins_fun) ) );
    
    if (shared_lib && !include_shared){
        return;
    }
    
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
    
    UINT32 mem_acessos = 0;
    UINT32 regdata_size = 0;
    
    UINT32 memOperands = 0;
    UINT32 regOperands = 0;
    
    if (full_analysis){
        
        memOperands = INS_MemoryOperandCount(ins);
        regOperands = INS_MaxNumRRegs(ins);
        
        //Get the number of memory acceses
        for (UINT32 memOp = 0; memOp < memOperands; memOp++){
            if ( INS_MemoryOperandIsRead(ins, memOp) ){
                mem_acessos++;
            }
            if ( INS_MemoryOperandIsWritten(ins, memOp) ){
                mem_acessos++;
            }
        }
        
        //Get the total size of registers data
        regdata_size = 0;
        for (UINT32 i = 0; i < regOperands; i++){
            REG regi = INS_RegR(ins, i);
            
            if (regi == REG_X87){
                FXSAVE x87reg;
                regdata_size += (UINT32)( (UINT64)&x87reg._sts[0] - (UINT64)&x87reg );
            }
            else if ( (regi >= REG_ST0 && regi <= REG_ST7) || (regi >= REG_MM0 && regi <= REG_MM7) ){
                regdata_size += 10;
            }
            else if ( (regi >= REG_XMM0 && regi <= REG_XMM7) || (regi >= REG_XMM8 && regi <= REG_XMM15) ){
                regdata_size += 16;
            }
            else {
                regdata_size += sizeof(ADDRINT);
            }
        }
    }
    
    
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
    
    //Register the instruction and the number of operands
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)doprint,
        IARG_THREAD_ID,
        IARG_INST_PTR,
        IARG_REG_VALUE, REG_STACK_PTR,
        IARG_UINT32, flags,
        IARG_UINT32, mem_acessos,
        IARG_UINT32, regdata_size,
    IARG_END);
    
    if (full_analysis){
        //Register each memory access
        for (UINT32 memOp = 0; memOp < memOperands; memOp++){
            if ( INS_MemoryOperandIsRead(ins, memOp) ){
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_memory,
                    IARG_THREAD_ID,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);
            }
            if ( INS_MemoryOperandIsWritten(ins, memOp) ){
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_memory,
                    IARG_THREAD_ID,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);
            }
        }
        
        //Register each read register
        for (UINT32 i = 0; i < regOperands; i++){
            REG regi = INS_RegR(ins, i);
            
            //Stack registers: SP and FP/BP
            //Problem: It may be used for other purpose
            if (
                regi == REG_STACK_PTR ||
                regi == REG_SP || regi == REG_ESP ||
                regi == REG_BP || regi == REG_EBP || regi == REG_GBP
            ){
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_register_stack,
                    IARG_THREAD_ID,
                    IARG_REG_VALUE, regi,
                IARG_END);
            }
            
            //Different registers
            else if (regi == REG_X87 ){
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_register_87,
                    IARG_THREAD_ID,
                    IARG_CONTEXT,
                IARG_END);
            }
            else if ( (regi >= REG_ST0 && regi <= REG_ST7) ) {
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_register_st,
                    IARG_THREAD_ID,
                    IARG_CONTEXT,
                    IARG_UINT32, (UINT32)(regi - REG_ST0),
                IARG_END);
            }
            else if ( (regi >= REG_MM0 && regi <= REG_MM7) ){
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_register_ref,
                    IARG_THREAD_ID,
                    IARG_REG_CONST_REFERENCE, regi,
                    IARG_UINT32, (UINT32)10,
                IARG_END);
            }
            else if ( (regi >= REG_XMM0 && regi <= REG_XMM7) || (regi >= REG_XMM8 && regi <= REG_XMM15) ){
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_register_ref,
                    IARG_THREAD_ID,
                    IARG_REG_CONST_REFERENCE, regi,
                    IARG_UINT32, (UINT32)16,
                IARG_END);
            }
            
            //Common registers
            else {
                //REG_GFLAGS ou REG_EFLAGS are included
                
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_register,
                    IARG_THREAD_ID,
                    IARG_REG_VALUE, regi,
                IARG_END);
            }
        }
        
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
    barrier_flag[tid] = true;
}


VOID inicioThread(THREADID tid, ADDRINT sp){
    if(OutFile[tid] == NULL){
        char nome[20];
        sprintf(nome, "%s%d.txt", KnobOutputFile.Value().c_str(), tid);
        OutFile[tid] = fopen(nome, "w");
        sp_base[tid] = sp;
    }
}
VOID fimThread(THREADID tid){
    if ( OutFile[tid] != NULL){
        fclose( OutFile[tid] );
        OutFile[tid] = NULL;
    }
}


VOID Image(IMG img, VOID *v){
    
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
            RTN_InsertCall(t_exit, IPOINT_BEFORE, (AFUNPTR)fimThread,
                IARG_THREAD_ID,
            IARG_END);
            RTN_Close(t_exit);
        }
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
        OutFile[threadid] = NULL;
        secao_critica[threadid] = 0;
        
        hard_basic_block_begin[threadid] = false;
        barrier_flag[threadid] = false;
        
        #ifdef COMPILER_SUPPORT
        basic_block_priority[threadid] = 0;
        ignorar_print[threadid] = false;
        delayed_branch[threadid] = false;
        fun_call_begin[threadid] = false;
        fun_call_end[threadid] = false;
        dom_begin_list[threadid] = 0;
        dom_end_counter[threadid] = 0;
        #endif
        
        stored_delayed_memory_quan[threadid] = 0;
        stored_delayed_regis_quan[threadid] = 0;
        
        if (!verificar_fun){
            inicioThread(threadid, PIN_GetContextReg( ctxt, REG_STACK_PTR) );
        }
        
    }
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v){
    if(threadid >= 0 && threadid < MAX_TID){
        if (OutFile[threadid] != NULL || !verificar_fun){
            fimThread(threadid);
        }
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v){
    
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
    full_analysis = KnobFullAnalysis.Value() == "true";
    
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
