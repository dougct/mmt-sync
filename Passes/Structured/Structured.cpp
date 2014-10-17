#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/CFG.h"

using namespace llvm;


namespace {
    
    struct FunctionAnalysis : public ModulePass {
        
        FunctionAnalysis() : ModulePass(ID) {}
        
        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesAll(); //it means that you are not destroying the results of other analysis.
        }
        
        
        virtual bool runOnModule(Module &M){
            
            //Functions to be inserted as pragmas
            FunctionType* funvoid = FunctionType::get(Type::getVoidTy( M.getContext() ), false);
            
            Function* fun_while = Function::Create(funvoid, GlobalValue::ExternalLinkage, "pragma_while_begin", &M);
            
            
            for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; Fit++) {
                Function& F = *Fit;
                
                if (F.begin() == F.end()) continue;
                
                //Changing the control flow of the function
                
                for (Function::iterator bb = F.begin(), en = F.end(); bb != en; bb++){
                    //Loops are detected when the basic block has multiple previous basic block
                    
                    
                    
                }
                
            }
            return true;
        }
        
        static char ID;
    };
    char FunctionAnalysis::ID = 0;
    static RegisterPass<FunctionAnalysis> pass("execute", "Transforms the control flow graph into a structured control flow with special instructions.");
}
