#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Support/CFG.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"

#include <stdio.h>
#include <set>
#include <list>
#include <map>

using namespace llvm;


void insert_function_limits(Function &F);


namespace {
    
    struct FunctionAnalysis : public ModulePass {
        
        FunctionAnalysis() : ModulePass(ID) {}
        
        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesAll(); //it means that you are not destroying the results of other analysis.
            AU.addRequired<UnifyFunctionExitNodes>();
        }
        
        
        FunctionType* funvoid;
        Module* currentM;
        
        //Beginning
        virtual bool runOnModule(Module &M){
            
            funvoid = FunctionType::get(Type::getVoidTy( M.getContext() ), false);
            currentM = &M;
            
            for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; Fit++) {
                Function& F = *Fit;
                
                if (F.begin() == F.end()) continue;
                
                //Inserting pragmas in the beginning and in the end of the function.
                insert_function_limits(F);
                
                //Inserting pragmas in branches and its postdominators
                insert_longest_path(F);
                
            }
            return true;
        }
        
        void insert_longest_path(Function &F){
            
            //The CFG must br simplified (-simplifycfg)
            
            //Insert pragmas
            if ( is_acyclic_cfg(F) ){
                floyd_warshall(F);
            }
            else if (F.size() <= 60){
                insert_longest_path_bf(F);
            }
            else {
                edge_covering_tree(F);
            }
        }
        
        void insert_the_pragma(BasicBlock &bb, int longest_distance){
            char funname[40];
            static int pragma_counter = 0; //avoid some compiler optimizations
            pragma_counter++;
            sprintf(funname, "pragma_basic_block_%06u_%u", pragma_counter, longest_distance);
            Function* pragma = currentM->getFunction(funname);
            if (pragma == NULL){
                pragma = Function::Create(funvoid, GlobalValue::ExternalLinkage, funname, currentM);
            }
            CallInst* thecall = CallInst::Create(pragma);
            thecall->setDoesNotThrow();
            //thecall->insertBefore( bb.getFirstNonPHI() );
            thecall->insertBefore( bb.getFirstInsertionPt() );
        }
        
        bool is_acyclic_cfg(Function &F){
            std::map<BasicBlock*, bool> visited;
            return is_acyclic_cfg(visited, &F.getEntryBlock() );
        }
        bool is_acyclic_cfg(std::map<BasicBlock*, bool>& visited, BasicBlock* node){
            std::pair< std::map<BasicBlock*, bool>::iterator, bool> res;
            res = visited.insert( std::make_pair(node, false) );
            
            if (!res.second){
                //Is is already present
                //If it is finished (true), it is not a cycle. If it was visited (false), it is a cycle.
                return res.first->second;
            }
            
            for (succ_iterator si = succ_begin(node), se = succ_end(node); si != se; si++){
                if (*si == node){
                    //self-loop
                }
                
                if ( !is_acyclic_cfg(visited, *si) ){
                    return false;
                }
            }
            res.first->second = true;
            return true;
        }
        
        bool isFinalBlock(BasicBlock* bb){
            TerminatorInst* term = bb->getTerminator();
            if (term->getNumSuccessors() > 0){
                return false;
            }
            if (isa<ReturnInst>(term)){
                return true;
            }
            
            for (BasicBlock::iterator it = bb->end(), end = bb->begin(); ; ){
                it--;
                if (it == end)
                    break;
                
                CallInst* inst = dyn_cast<CallInst>( &*it );
                if (inst != NULL){
                    Value* name = inst->getCalledValue();
                    if (name->hasName() && (name->getName().str() == "pthread_exit") ){
                        return true;
                    }
                }
            }
            return false;
        }
        
        
        
        
        void floyd_warshall(Function &F){
            
            int numV = F.size();
            int dest = -1;
            BasicBlock* vertices[numV];
            std::map<BasicBlock*, int> amap;
            
            int ii = 0;
            for (Function::iterator bb = F.begin(), en = F.end(); bb != en; bb++) {
                vertices[ii] = &*bb;
                amap[&*bb] = ii;
                if ( isFinalBlock(&*bb) ){
                    dest = ii;
                }
                ii++;
            }
            if (dest == -1){
                return;
            }
            
            int** distance = new int*[numV];
            for (int i=0; i<numV; i++) {
                distance[i] = new int[numV];
                for (int j=0; j<numV; j++) {
                    distance[i][j] = 1;
                }
                distance[i][i] = 0;
                
                for (succ_iterator si = succ_begin(vertices[i]), se = succ_end(vertices[i]); si != se; si++){
                    distance[i][ amap[*si] ] = -1;
                }
                
            }
            
            for (int k=0; k<numV; k++) {
                for (int i=0; i<numV; i++) {
                    for (int j=0; j<numV; j++) {
                        int nov = distance[i][k] + distance[k][j];
                        if (nov < distance[i][j] ){
                            distance[i][j] = nov;
                        }
                    }
                }
            }
            
            for (int i=0; i<numV; i++) {
                if (distance[i][dest] < 1){
                    insert_the_pragma(*vertices[i], (-distance[i][dest]) + 1);
                }
                delete [] distance[i];
            }
            delete [] distance;
        }
        
        
        
        
        
        
        typedef std::set<BasicBlock*> BSet;
        
        //edge_covering_tree from Ocelot
        struct TreeNode{
            BasicBlock* parent;
            std::list<BasicBlock*> children;
            int priority;
            
            TreeNode(BasicBlock* pa, int pr)
            : parent(pa), priority(pr) {}
        };
        typedef std::map<BasicBlock*, TreeNode> CTree;

        
        Function* currentFun;
        void edge_covering_tree(Function &F){
            //Ensure that the function has at most one return block.
            currentFun = &F;
            getAnalysis<UnifyFunctionExitNodes>(F);
            
            //Find last
            BasicBlock* root = NULL;
            for (Function::iterator bb = F.begin(), en = F.end(); bb != en; bb++) {
                BasicBlock& block = (*bb);
                if ( isFinalBlock(&block) ){
                    root = &block;
                    break;
                }
            }
            if (root == NULL){
                //report_fatal_error("Error: No final basic block was found.\n");
                return;
            }
            
            // Build the edge-covering tree
            CTree ce_tree;
            CTree::iterator beg = ce_tree.insert(std::make_pair(root, TreeNode(NULL, 1))).first;
            _visitNode(ce_tree, beg);
            
            //Insert pragmas with priorities
            for (CTree::iterator bb = ce_tree.begin(), en = ce_tree.end(); bb != en; bb++) {
                insert_the_pragma(*bb->first, bb->second.priority);
            }
            
        }
        
        void updatePriority(CTree& newtree, CTree::iterator node){
            for(std::list<BasicBlock*>::iterator child = node->second.children.begin(), end = node->second.children.end(); child != end; child++){
                CTree::iterator childNode = newtree.find( *child );
                childNode->second.priority = node->second.priority + 1;
                updatePriority(newtree, childNode);
            }
        }
        void _visitNode(CTree& newtree, CTree::iterator node){
            
            for (pred_iterator pi = pred_begin(node->first), pe = pred_end(node->first); pi != pe; pi++){
                BasicBlock* predecessor = *pi;
                
                TreeNode tn(node->first, node->second.priority + 1);
                std::pair<CTree::iterator, bool> res = newtree.insert(std::make_pair(predecessor, tn));
                
                if(res.second){
                    //not found, a new one has been inserted
                    node->second.children.push_back(predecessor);
                    _visitNode(newtree, res.first);
                }
                else{
                    // swap if it results in a longer path through the subtree
                    if(res.first->second.priority < node->second.priority + 1){
                        
                        // detect loops
                        
                        if(node->first != predecessor && !hasPredecessorPath(node->first, predecessor) ){
                            //If there is not a path from node until predecessor
                            
                            res.first->second.priority = node->second.priority + 1;
                            res.first->second.parent = node->first;
                            
                            //remove old
                            std::list<BasicBlock*>& old = newtree.find( res.first->second.parent )->second.children;
                            for (std::list<BasicBlock*>::iterator it = old.begin(), end = old.begin(); it != end; it++){
                                if (*it == predecessor){
                                    old.erase(it);
                                    break;
                                }
                            }
                            
                            node->second.children.push_back(predecessor);
                            
                            updatePriority(newtree, res.first);
                        }
                    }
                }
            }
        }
        
        bool hasPredecessorPath(BasicBlock* node, BasicBlock* predecessor){
            //Check if there is path from node until predecessor
            BSet visited;
            return  _hasPredecessorPath(visited, node, predecessor);
        }
        bool _hasPredecessorPath(BSet& visited, BasicBlock* node, BasicBlock* predecessor){
            
            BSet::iterator bb = visited.find(node);
            if (bb != visited.end()){
                return false;
            }
            visited.insert(node);
            
            for (succ_iterator si = succ_begin(node), se = succ_end(node); si != se; si++){
                if (*si == predecessor){
                    return true;
                }
                else {
                    if ( _hasPredecessorPath(visited, *si, predecessor) ){
                        return true;
                    }
                }
            }
            return false;
        }
        


        
        
        
        
        
        
        
        
        
        //Brute force
        void insert_longest_path_bf(Function &F){
            for (Function::iterator bb = F.begin(), en = F.end(); bb != en; bb++) {
                BasicBlock& block = (*bb);
                int longest_distance = get_longest_distance_bf(block);
                insert_the_pragma(block, longest_distance);
            }
        }
        
        int get_longest_distance_bf(BasicBlock& bb){
            std::set<BasicBlock*> visitados;
            int longest_distance = 1;
            visitados.insert(&bb);
            get_longest_distance_bf(bb, visitados, 0, &longest_distance);
            return longest_distance;
        }
        
        void get_longest_distance_bf(BasicBlock& bb, std::set<BasicBlock*>& visitados, int current_distance, int* longest_distance){
            current_distance++;
            if (isFinalBlock(&bb) && current_distance > *longest_distance){
                *longest_distance = current_distance;
            }
            else {
                for (succ_iterator si = succ_begin(&bb), se = succ_end(&bb); si != se; si++){
                    BasicBlock* su = *si;
                    if ( visitados.insert(su).second ){
                        get_longest_distance_bf(*su, visitados, current_distance, longest_distance);
                    }
                }
            }
            visitados.erase(&bb);
        }
        
        
        static char ID;
    };
    char FunctionAnalysis::ID = 0;
    static RegisterPass<FunctionAnalysis> pass("execute", "Inserts pragmas with the longest distance until the end for all basic blocks.");
}
