#include <stdio.h>
#include <string.h>
#include <list>

#include "../Tools/PragmasCodes.hpp"

int main(){
    
    FILE* arq = stdin;
    
    std::list<char*> lines;
    
    while(true){
        char* line = new char[200];
        char* ret = fgets(line, 200, arq);
        if (ret == NULL){
            delete [] line;
            break;
        }
        
        lines.push_back(line);
    }
    
    const char* asm_code;
    if ( sizeof(void*) == sizeof(long long) ){
        asm_code = "\tcallq\tthe_pragma_function\n\tpushq\t%%r15\n\tmovq\t$%u, %%r15\n\tpopq\t%%r15\n\tcallq\tthe_pragma_function\n";
    }
    else {
        asm_code = "\tcall\tthe_pragma_function\n\tpushl\t%%edx\n\tmov\t$%u, %%edx\n\tpopl\t%%edx\n\tcall\tthe_pragma_function\n";
    }
    
    int len1 = strlen("\tcallq\tpragma_dom_begin");
    int len2 = strlen("\tcallq\tpragma_dom_end");
    int len3 = strlen("\tcallq\tpragma_basic_block_");
    for(std::list<char*>::iterator it=lines.begin(), end = lines.end(); it != end; it++){
        char* line = *it;
        if (strcmp(line, "\tcallq\tpragma_function_begin\n") == 0){
            it = lines.erase(it);
            for (std::list<char*>::iterator beg=lines.begin(); it!=beg; it--){
                char* line2 = *it;
                if (
                    strcmp(line2, "# BB#0:                                 # %entry\n") == 0 ||
                    strcmp(line2, "\t.cfi_startproc\n") == 0
                ){
                    it++;
                    sprintf(line, asm_code, CODE_FUN_BEGIN);
                    it = lines.insert(it, line);
                    break;
                }
            }
        }
        else if (strcmp(line, "\tcallq\tpragma_function_end\n") == 0){
            it--;
            std::list<char*>::iterator saved_it = it;
            it++;
            it = lines.erase(it);
            for (; it!=end; it++){
                char* line2 = *it;
                if (
                    strcmp(line2, "\tret\n") == 0 ||
                    strcmp(line2, "\tretq\n") == 0 ||
                    
                    // # TAILCALL
                    strncmp(line2, "\tjmp\t", 5) == 0 ||
                    strncmp(line2, "\tjmpq\t", 6) == 0
                ){
                    sprintf(line, asm_code, CODE_FUN_END);
                    it = lines.insert(it, line);
                    break;
                }
                else if (strcmp(line2, "\t.cfi_endproc\n") == 0){
                    
                    fprintf(stderr, "No return instruction was found.\n");
                    for (int i = 0; i<10; i++){
                        it--;
                    }
                    for (int i = 0; i<10; i++){
                        fprintf(stderr, "%s", *it);
                        it++;
                    }
                    return -1;
                }
            }
            it = saved_it;
        }
        else if (strncmp(line, "\tcallq\tpragma_dom_begin", len1) == 0){
            //No move
            unsigned int param;
            sscanf(line + len1, "%u", &param);
            sprintf(line, asm_code, MIN_DOM_BEGIN + param);
        }
        else if (strncmp(line, "\tcallq\tpragma_dom_end", len2) == 0){
            unsigned int param;
            sscanf(line + len2, "%u", &param);
            it = lines.erase(it);
            it--;
            for (std::list<char*>::iterator beg=lines.begin(); it!=beg; it--){
                char* line2 = *it;
                if (strncmp(line2, "# BB#", 5) == 0 || strncmp(line2, ".LBB", 4) == 0){
                    it++;
                    sprintf(line, asm_code, MIN_DOM_END + param);
                    it = lines.insert(it, line);
                    break;
                }
            }
        }
        else if (strncmp(line, "\tcallq\tpragma_basic_block_", len3) == 0){
            unsigned int param;
            sscanf(line + len3 + 6 + 1, "%u", &param);
            it = lines.erase(it);
            it--;
            for (std::list<char*>::iterator beg=lines.begin(); it!=beg; it--){
                char* line2 = *it;
                if (strncmp(line2, "# BB#", 5) == 0 || strncmp(line2, ".LBB", 4) == 0 || strcmp(line2, "\t.cfi_startproc\n") == 0){
                    it++;
                    sprintf(line, asm_code, BASIC_BLOCK_PRIORITY + param);
                    it = lines.insert(it, line);
                    break;
                }
            }
        }
    }
    
    for(std::list<char*>::iterator it=lines.begin(), end = lines.end(); it != end; it++){
        char* line = *it;
        fputs(line, stdout);
        delete [] line;
    }
    
}
