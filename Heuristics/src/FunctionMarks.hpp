
static bool is_function_call(Thread* t){
    return t->pragma_fun_call;
    //return InstructionFlags::is_function_call(t->cf_flags);
}

static bool is_function_ret(Thread* t){
    return t->pragma_fun_ret;
    //return InstructionFlags::is_function_ret(t->cf_flags);
}
