	.file	"exe/simple/test02.bc2"
	.text
	.globl	_Z6divideii
	.align	16, 0x90
	.type	_Z6divideii,@function
_Z6divideii:                            # @_Z6divideii
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp2:
	.cfi_def_cfa_offset 16
.Ltmp3:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp4:
	.cfi_def_cfa_register %rbp
	#APP
	#NO_APP
	addl	%esi, %edi
	xorl	%ecx, %ecx
	.align	16, 0x90
.LBB0_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	movl	%ecx, %eax
	leal	1(%rax), %ecx
	subl	%esi, %edi
	cmpl	%esi, %edi
	jg	.LBB0_1
# BB#2:                                 # %while.end
	#APP
	#NO_APP
                                        # kill: EAX<def> EAX<kill> RAX<kill>
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp5:
	.size	_Z6divideii, .Ltmp5-_Z6divideii
	.cfi_endproc

	.globl	_Z10threadProgi
	.align	16, 0x90
	.type	_Z10threadProgi,@function
_Z10threadProgi:                        # @_Z10threadProgi
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp9:
	.cfi_def_cfa_offset 16
.Ltmp10:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp11:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
.Ltmp12:
	.cfi_offset %rbx, -24
	#APP
	#NO_APP
	movslq	%edi, %rbx
	movq	%rbx, %rcx
	shrq	$63, %rcx
	addq	%rbx, %rcx
	andq	$-2, %rcx
	movq	%rbx, %rax
	subq	%rcx, %rax
	je	.LBB1_2
# BB#1:                                 # %cond.true
	imulq	$1000, %rbx, %rdi       # imm = 0x3E8
	jmp	.LBB1_3
.LBB1_2:                                # %cond.false
	imulq	$2000, %rbx, %rdi       # imm = 0x7D0
.LBB1_3:                                # %cond.end
	leal	1(%rbx), %ecx
	leal	2(%rbx), %esi
	testq	%rax, %rax
	cmovneq	%rcx, %rsi
                                        # kill: EDI<def> EDI<kill> RDI<kill>
                                        # kill: ESI<def> ESI<kill> RSI<kill>
	callq	_Z6divideii
	movslq	%eax, %rdx
	movl	$.L.str, %edi
	xorl	%eax, %eax
	movq	%rbx, %rsi
	callq	printf
	xorl	%edi, %edi
	callq	pthread_exit
.Ltmp13:
	.size	_Z10threadProgi, .Ltmp13-_Z10threadProgi
	.cfi_endproc

	.globl	_Z12execute_testi
	.align	16, 0x90
	.type	_Z12execute_testi,@function
_Z12execute_testi:                      # @_Z12execute_testi
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp16:
	.cfi_def_cfa_offset 16
.Ltmp17:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp18:
	.cfi_def_cfa_register %rbp
	#APP
	#NO_APP
	#APP
	#NO_APP
	callq	_Z10threadProgi
	movq	%rax, resultado(%rip)
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp19:
	.size	_Z12execute_testi, .Ltmp19-_Z12execute_testi
	.cfi_endproc

	.globl	_Z12prepare_testi
	.align	16, 0x90
	.type	_Z12prepare_testi,@function
_Z12prepare_testi:                      # @_Z12prepare_testi
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp22:
	.cfi_def_cfa_offset 16
.Ltmp23:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp24:
	.cfi_def_cfa_register %rbp
	#APP
	#NO_APP
	#APP
	#NO_APP
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp25:
	.size	_Z12prepare_testi, .Ltmp25-_Z12prepare_testi
	.cfi_endproc

	.globl	_Z14number_threadsv
	.align	16, 0x90
	.type	_Z14number_threadsv,@function
_Z14number_threadsv:                    # @_Z14number_threadsv
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp28:
	.cfi_def_cfa_offset 16
.Ltmp29:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp30:
	.cfi_def_cfa_register %rbp
	#APP
	#NO_APP
	#APP
	#NO_APP
	movl	_ZL2nt(%rip), %eax
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp31:
	.size	_Z14number_threadsv, .Ltmp31-_Z14number_threadsv
	.cfi_endproc

	.globl	_Z12number_timesv
	.align	16, 0x90
	.type	_Z12number_timesv,@function
_Z12number_timesv:                      # @_Z12number_timesv
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp34:
	.cfi_def_cfa_offset 16
.Ltmp35:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp36:
	.cfi_def_cfa_register %rbp
	#APP
	#NO_APP
	#APP
	#NO_APP
	movl	_ZL5repet(%rip), %eax
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp37:
	.size	_Z12number_timesv, .Ltmp37-_Z12number_timesv
	.cfi_endproc

	.globl	execute_tests
	.align	16, 0x90
	.type	execute_tests,@function
execute_tests:                          # @execute_tests
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp41:
	.cfi_def_cfa_offset 16
.Ltmp42:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp43:
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
.Ltmp44:
	.cfi_offset %rbx, -32
.Ltmp45:
	.cfi_offset %r14, -24
	movl	%edi, %r14d
	#APP
	#NO_APP
	movl	$-1, %ebx
	jmp	.LBB6_1
	.align	16, 0x90
.LBB6_2:                                # %for.body
                                        #   in Loop: Header=BB6_1 Depth=1
	movl	%r14d, %edi
	callq	_Z12execute_testi
.LBB6_1:                                # %for.cond
                                        # =>This Inner Loop Header: Depth=1
	incl	%ebx
	cmpl	_ZL5repet(%rip), %ebx
	jl	.LBB6_2
# BB#3:                                 # %for.end
	#APP
	#NO_APP
	popq	%rbx
	popq	%r14
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp46:
	.size	execute_tests, .Ltmp46-execute_tests
	.cfi_endproc

	.globl	_Z10run_threadPv
	.align	16, 0x90
	.type	_Z10run_threadPv,@function
_Z10run_threadPv:                       # @_Z10run_threadPv
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp50:
	.cfi_def_cfa_offset 16
.Ltmp51:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp52:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
.Ltmp53:
	.cfi_offset %rbx, -24
	#APP
	#NO_APP
	movl	(%rdi), %ebx
	movl	%ebx, %edi
	callq	_Z12prepare_testi
	movl	%ebx, %edi
	callq	execute_tests
	xorl	%edi, %edi
	callq	pthread_exit
.Ltmp54:
	.size	_Z10run_threadPv, .Ltmp54-_Z10run_threadPv
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	callq	the_pragma_function
	pushq	%r15
	movq	$1000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp58:
	.cfi_def_cfa_offset 16
.Ltmp59:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp60:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	pushq	%rax
.Ltmp61:
	.cfi_offset %rbx, -56
.Ltmp62:
	.cfi_offset %r12, -48
.Ltmp63:
	.cfi_offset %r13, -40
.Ltmp64:
	.cfi_offset %r14, -32
.Ltmp65:
	.cfi_offset %r15, -24
	movq	%rsi, %r15
	movl	%edi, %ebx
	#APP
	#NO_APP
	movl	$-1, %r14d
	cmpl	$2, %ebx
	jl	.LBB8_11
# BB#1:                                 # %entry
	cmpl	$3, %ebx
	jg	.LBB8_11
# BB#2:                                 # %if.end
	movq	8(%r15), %rdi
	movl	$.L.str1, %esi
	movl	$_ZL2nt, %edx
	xorl	%eax, %eax
	callq	sscanf
	cmpl	$3, %ebx
	jne	.LBB8_4
# BB#3:                                 # %if.then3
	movq	16(%r15), %rdi
	movl	$.L.str1, %esi
	movl	$_ZL5repet, %edx
	xorl	%eax, %eax
	callq	sscanf
.LBB8_4:                                # %if.end6
	cmpl	$0, _ZL2nt(%rip)
	jle	.LBB8_11
# BB#5:                                 # %if.end9
	movq	%rsp, -48(%rbp)         # 8-byte Spill
	movl	_ZL2nt(%rip), %eax
	movq	%rsp, %rbx
	leaq	15(,%rax,4), %rax
	andq	$-16, %rax
	subq	%rax, %rbx
	movq	%rbx, %rsp
	movl	_ZL2nt(%rip), %eax
	movq	%rsp, %r12
	leaq	15(,%rax,8), %rax
	andq	$-16, %rax
	subq	%rax, %r12
	movq	%r12, %rsp
	movl	$1, %r15d
	xorl	%r13d, %r13d
	movq	%r12, %r14
	jmp	.LBB8_6
	.align	16, 0x90
.LBB8_7:                                # %for.body
                                        #   in Loop: Header=BB8_6 Depth=1
	movl	%r15d, (%rbx)
	xorl	%esi, %esi
	movl	$_Z10run_threadPv, %edx
	movq	%r14, %rdi
	movq	%rbx, %rcx
	callq	pthread_create
	addq	$4, %rbx
	addq	$8, %r14
	incl	%r15d
.LBB8_6:                                # %for.cond
                                        # =>This Inner Loop Header: Depth=1
	leal	-1(%r15), %eax
	cmpl	_ZL2nt(%rip), %eax
	jl	.LBB8_7
	jmp	.LBB8_8
	.align	16, 0x90
.LBB8_9:                                # %for.body21
                                        #   in Loop: Header=BB8_8 Depth=1
	movq	(%r12,%r13,8), %rdi
	xorl	%esi, %esi
	callq	pthread_join
	incq	%r13
.LBB8_8:                                # %for.cond19
                                        # =>This Inner Loop Header: Depth=1
	cmpl	_ZL2nt(%rip), %r13d
	jl	.LBB8_9
# BB#10:                                # %for.end27
	movq	-48(%rbp), %rsp         # 8-byte Reload
	xorl	%r14d, %r14d
.LBB8_11:                               # %return
	#APP
	#NO_APP
	movl	%r14d, %eax
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp66:
	.size	main, .Ltmp66-main
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Thread ID: %ld, RR: %ld\n"
	.size	.L.str, 25

	.type	resultado,@object       # @resultado
	.bss
	.globl	resultado
	.align	8
resultado:
	.quad	0                       # 0x0
	.size	resultado, 8

	.type	_ZL2nt,@object          # @_ZL2nt
	.local	_ZL2nt
	.comm	_ZL2nt,4,4
	.type	_ZL5repet,@object       # @_ZL5repet
	.data
	.align	4
_ZL5repet:
	.long	1                       # 0x1
	.size	_ZL5repet, 4

	.type	.L.str1,@object         # @.str1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str1:
	.asciz	"%d"
	.size	.L.str1, 3


	.ident	"clang version 3.5 (trunk 201826) (llvm/trunk 201825)"
	.ident	"clang version 3.5 (trunk 201826) (llvm/trunk 201825)"
	.section	".note.GNU-stack","",@progbits
