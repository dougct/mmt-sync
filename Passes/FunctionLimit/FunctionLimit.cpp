#include "llvm/Pass.h"
#include "llvm/IR/Module.h"

using namespace llvm;


void insert_function_limits(Function &F);


namespace {
    
    struct FunctionAnalysis : public ModulePass {
        
        FunctionAnalysis() : ModulePass(ID) {}
        
        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesAll(); //it means that you are not destroying the results of other analysis.
        }
        
        
        virtual bool runOnModule(Module &M){
            for (Module::iterator fit = M.begin(), Fend = M.end(); fit != Fend; fit++) {
                Function& F = *fit;
                
                if (F.begin() == F.end()) continue;
                
                insert_function_limits(F);
                
            }
            return true;
        }
        
        static char ID;
    };
    char FunctionAnalysis::ID = 0;
    static RegisterPass<FunctionAnalysis> pass("execute", "Inserts pragmas in the beginning and in the end of the functions.");
}
