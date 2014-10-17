#ifndef __InstructionFlags__HEADER__
#define __InstructionFlags__HEADER__

namespace InstructionFlags {
    static const unsigned char IsBranch = 1;
    static const unsigned char IsDirectBranch = 2;
    static const unsigned char IsCall = 4;
    static const unsigned char IsDirectCall = 8;
    static const unsigned char IsProcedureCall = 16;
    static const unsigned char IsRet = 32;
    static const unsigned char HasFallThrough = 64;
    
    inline bool is_end_hardware_block(unsigned char flags){
        // IsBranch | IsCall | IsRet | !HasFallThrough - The next instruction is the beginning of a basic block.
        return (flags & IsBranch) != 0 || (flags & IsCall) != 0 || (flags & IsRet) != 0 || (flags & HasFallThrough) == 0;
        
    }
    inline bool is_direct_branch(unsigned char flags){
        // IsDirectBranch - Conditional or unconditional branch
        return (flags & IsDirectBranch) != 0;
    }
    inline bool is_conditional_branch(unsigned char flags){
        // IsDirectBranch & HasFallThrough - Conditional branch
        return (flags & IsDirectBranch) != 0 && (flags & HasFallThrough) != 0;
    }
    inline bool is_unconditional_branch(unsigned char flags){
        // IsDirectBranch & !HasFallThrough - Unconditional branch
        return (flags & IsDirectBranch) != 0 && (flags & HasFallThrough) == 0;
    }
    inline bool is_indirect_branch(unsigned char flags){
        // IsBranch & !IsDirectBranch - Indirect branch
        return (flags & IsBranch) != 0 && (flags & IsDirectBranch) == 0;
    }
    inline bool is_indirect_call(unsigned char flags){
        // IsProcedureCall - function call
        return (flags & IsProcedureCall) != 0 && (flags & IsDirectCall) == 0;
    }
    inline bool is_function_call(unsigned char flags){
        // IsProcedureCall - function call
        return (flags & IsProcedureCall) != 0;
    }
    inline bool is_function_ret(unsigned char flags){
        // IsRet - function return
        return (flags & IsRet) != 0;
    }
};

#endif
