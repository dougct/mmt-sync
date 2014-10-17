#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Support/CFG.h"

using namespace llvm;


void insert_function_limits(Function &F){
    
    //Functions to be inserted as pragmas
    FunctionType* funvoid = FunctionType::get(Type::getVoidTy( F.getParent()->getContext() ), false);
    
    InlineAsm* fun_begin;
    InlineAsm* fun_end;
    if (sizeof(void*)==sizeof(long long)){
        fun_begin = InlineAsm::get(funvoid, "callq\tpragma_function_begin", "", false);
        fun_end = InlineAsm::get(funvoid, "callq\tpragma_function_end", "", false);
    }
    else {
        fun_begin = InlineAsm::get(funvoid, "call\tpragma_function_begin", "", false);
        fun_end = InlineAsm::get(funvoid, "call\tpragma_function_end", "", false);
    }
    
    //In the beginning
    {
        BasicBlock& first = F.getEntryBlock();
        int preds = 0;
        for (pred_iterator pi = pred_begin(&first), pe = pred_end(&first); pi != pe; pi++){
            preds++;
        }
        if (preds > 0){
            report_fatal_error("Error: The first basic block has predecessor.\n");
            //TODO: insert a new basic block in the beginning
        }
        
        CallInst* before = CallInst::Create(fun_begin);
        before->setDoesNotThrow();
        before->insertBefore( first.getFirstNonPHI() );
    }
    
    //In the end
    for (Function::iterator bb = F.begin(), en = F.end(); bb != en; bb++){
        TerminatorInst* inst = bb->getTerminator();
        
        if ( isa<ReturnInst>(inst) ){
            CallInst* after = CallInst::Create(fun_end);
            after->setDoesNotThrow();
            //after->insertBefore( inst );
            after->insertBefore( bb->getFirstNonPHI() );
        }
    }
    
}
