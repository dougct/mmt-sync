#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Support/CFG.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"

#include <stdio.h>


using namespace llvm;


void insert_function_limits(Function &F);


namespace {
    
    struct FunctionAnalysis : public ModulePass {
        
        FunctionAnalysis() : ModulePass(ID) {}
        
        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesAll(); //it means that you are not destroying the results of other analysis.
            AU.addRequired<UnifyFunctionExitNodes>();
            AU.addRequired<PostDominatorTree>();
        }
        
        virtual bool runOnModule(Module &M){
            
            for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; Fit++) {
                Function& F = *Fit;
                
                if (F.begin() == F.end()) continue;
                
                //Ensure that the function has at most one return block.        
                getAnalysis<UnifyFunctionExitNodes>(F);
                
                //Inserting pragmas in the beginning and in the end of the function.
                insert_function_limits(F);
                
                //Inserting pragmas in branches and its postdominators
                insert_postdom_blocks(F);
                
            }
            return true;
        }
        
        void insert_postdom_blocks(Function &F){
            Module& M = *F.getParent();
            
            //Functions to be inserted as pragmas
            FunctionType* funvoid = FunctionType::get(Type::getVoidTy( M.getContext() ), false);
            
            //This is an unique number to identify the pragmas in a function.
            unsigned int pragma_counter = 0;
            
            
            //Inserting pragmas after the post-dominator
            
            PostDominatorTree* dt = &getAnalysis<PostDominatorTree>(F);
            
            for (Function::iterator bb = F.begin(), en = F.end(); bb != en; bb++) {
                
                TerminatorInst* term = (*bb).getTerminator();
                
                if ( isa<InvokeInst>(term) ){
                    continue;
                }
                
                if ( term->getNumSuccessors() > 1){
                    //This number of successors means branch.
                    
                    //Finding the post-dominator
                    succ_iterator si = succ_begin(bb);
                    BasicBlock* posdom = *si;
                    si++;
                    for (succ_iterator se = succ_end(bb); si != se; si++) {
                        BasicBlock* su2 = *si;
                        posdom = dt->findNearestCommonDominator(posdom, su2);
                        if (posdom == NULL)
                            break;
                    }
                    
                    //If the post-dominator exists
                    if (posdom != NULL){
                        pragma_counter++;
                        
                        char funname[40];
                        
                        sprintf(funname, "pragma_dom_begin%u", pragma_counter);
                        Function* dom_begin = M.getFunction(funname);
                        if (dom_begin == NULL){
                            dom_begin = Function::Create(funvoid, GlobalValue::ExternalLinkage, funname, &M);
                        }
                        CallInst* before = CallInst::Create(dom_begin);
                        before->setDoesNotThrow();
                        before->insertBefore( term );
                        
                        sprintf(funname, "pragma_dom_end%u", pragma_counter);
                        Function* dom_end = M.getFunction(funname);
                        if (dom_end == NULL){
                            dom_end = Function::Create(funvoid, GlobalValue::ExternalLinkage, funname, &M);
                        }
                        CallInst* after = CallInst::Create(dom_end);
                        after->setDoesNotThrow();
                        after->insertBefore( posdom->getFirstNonPHI() );
                        
                    }
                }
            }
        }
        
        
        static char ID;
    };
    char FunctionAnalysis::ID = 0;
    static RegisterPass<FunctionAnalysis> pass("execute", "Inserts pragmas to synchronize threads at the immediate post-dominator of branches.");
}
