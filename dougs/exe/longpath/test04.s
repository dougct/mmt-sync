	.file	"exe/longpath/test04.bc2"
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000003, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp3:
	.cfi_def_cfa_offset 16
.Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp5:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%rbx
	pushq	%rax
.Ltmp6:
	.cfi_offset %rbx, -40
.Ltmp7:
	.cfi_offset %r14, -32
.Ltmp8:
	.cfi_offset %r15, -24
	movl	%esi, %r14d
	movl	%edi, %ebx
	testl	%r14d, %r14d
	#APP
	#NO_APP
	movl	$2, %r15d
	cmovgl	%r14d, %r15d
	addl	%r15d, %ebx
	xorl	%r14d, %r14d
	jmp	.LBB0_1
	.align	16, 0x90
.LBB0_2:                                # %while.body
	callq	the_pragma_function
	pushq	%r15
	movq	$4000003, %r15
	popq	%r15
	callq	the_pragma_function
                                        #   in Loop: Header=BB0_1 Depth=1
	incl	%r14d
.LBB0_1:                                # %while.cond
	callq	the_pragma_function
	pushq	%r15
	movq	$4000002, %r15
	popq	%r15
	callq	the_pragma_function
                                        # =>This Inner Loop Header: Depth=1
	subl	%r15d, %ebx
	cmpl	%r15d, %ebx
	jg	.LBB0_2
# BB#3:                                 # %while.end
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
	popq	%r15
	callq	the_pragma_function
	#APP
	#NO_APP
	movl	%r14d, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp9:
	.size	_Z6divideii, .Ltmp9-_Z6divideii
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
.Ltmp13:
	.cfi_def_cfa_offset 16
.Ltmp14:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp15:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
.Ltmp16:
	.cfi_offset %rbx, -24
	#APP
	#NO_APP
	movslq	%edi, %rbx
	movq	%rbx, %rax
	shrq	$63, %rax
	addq	%rbx, %rax
	andq	$-2, %rax
	subq	%rax, %rbx
	je	.LBB1_2
# BB#1:                                 # %if.then
	movl	$10, %edi
	movl	$3, %esi
	jmp	.LBB1_3
.LBB1_2:                                # %if.else
	movl	$100, %edi
	movl	$30, %esi
.LBB1_3:                                # %JointBB
	callq	_Z6divideii
	testq	%rbx, %rbx
	xorl	%edi, %edi
	callq	pthread_exit
.Ltmp17:
	.size	_Z10threadProgi, .Ltmp17-_Z10threadProgi
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp21:
	.cfi_def_cfa_offset 16
.Ltmp22:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp23:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
.Ltmp24:
	.cfi_offset %rbx, -24
	movl	%edi, %ebx
	#APP
	#NO_APP
	#APP
	#NO_APP
	movl	%ebx, %edi
	callq	_Z10threadProgi
	movq	%rax, resultado(%rip)
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp25:
	.size	_Z12execute_testi, .Ltmp25-_Z12execute_testi
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
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
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp31:
	.size	_Z12prepare_testi, .Ltmp31-_Z12prepare_testi
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
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
	movl	_ZL2nt(%rip), %eax
	popq	%rbp
	callq	the_pragma_function
	pushq	%r15
	movq	$1000002, %r15
	popq	%r15
	callq	the_pragma_function
	retq
.Ltmp37:
	.size	_Z14number_threadsv, .Ltmp37-_Z14number_threadsv
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp40:
	.cfi_def_cfa_offset 16
.Ltmp41:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp42:
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
.Ltmp43:
	.size	_Z12number_timesv, .Ltmp43-_Z12number_timesv
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000003, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp47:
	.cfi_def_cfa_offset 16
.Ltmp48:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp49:
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
.Ltmp50:
	.cfi_offset %rbx, -32
.Ltmp51:
	.cfi_offset %r14, -24
	movl	%edi, %r14d
	#APP
	#NO_APP
	movl	$-1, %ebx
	jmp	.LBB6_1
	.align	16, 0x90
.LBB6_2:                                # %for.body
	callq	the_pragma_function
	pushq	%r15
	movq	$4000003, %r15
	popq	%r15
	callq	the_pragma_function
                                        #   in Loop: Header=BB6_1 Depth=1
	movl	%r14d, %edi
	callq	_Z12execute_testi
.LBB6_1:                                # %for.cond
	callq	the_pragma_function
	pushq	%r15
	movq	$4000002, %r15
	popq	%r15
	callq	the_pragma_function
                                        # =>This Inner Loop Header: Depth=1
	incl	%ebx
	cmpl	_ZL5repet(%rip), %ebx
	jl	.LBB6_2
# BB#3:                                 # %for.end
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
	popq	%r15
	callq	the_pragma_function
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
.Ltmp52:
	.size	execute_tests, .Ltmp52-execute_tests
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp56:
	.cfi_def_cfa_offset 16
.Ltmp57:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp58:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
.Ltmp59:
	.cfi_offset %rbx, -24
	movq	%rdi, %rbx
	#APP
	#NO_APP
	movl	(%rbx), %ebx
	movl	%ebx, %edi
	callq	_Z12prepare_testi
	movl	%ebx, %edi
	callq	execute_tests
	xorl	%edi, %edi
	callq	pthread_exit
.Ltmp60:
	.size	_Z10run_threadPv, .Ltmp60-_Z10run_threadPv
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000009, %r15
	popq	%r15
	callq	the_pragma_function
	pushq	%rbp
.Ltmp64:
	.cfi_def_cfa_offset 16
.Ltmp65:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp66:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	pushq	%rax
.Ltmp67:
	.cfi_offset %rbx, -56
.Ltmp68:
	.cfi_offset %r12, -48
.Ltmp69:
	.cfi_offset %r13, -40
.Ltmp70:
	.cfi_offset %r14, -32
.Ltmp71:
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000008, %r15
	popq	%r15
	callq	the_pragma_function
	movq	8(%r15), %rdi
	movl	$.L.str, %esi
	movl	$_ZL2nt, %edx
	xorl	%eax, %eax
	callq	sscanf
	cmpl	$3, %ebx
	jne	.LBB8_4
# BB#3:                                 # %if.then3
	callq	the_pragma_function
	pushq	%r15
	movq	$4000007, %r15
	popq	%r15
	callq	the_pragma_function
	movq	16(%r15), %rdi
	movl	$.L.str, %esi
	movl	$_ZL5repet, %edx
	xorl	%eax, %eax
	callq	sscanf
.LBB8_4:                                # %if.end6
	callq	the_pragma_function
	pushq	%r15
	movq	$4000006, %r15
	popq	%r15
	callq	the_pragma_function
	cmpl	$0, _ZL2nt(%rip)
	jle	.LBB8_11
# BB#5:                                 # %if.end9
	callq	the_pragma_function
	pushq	%r15
	movq	$4000005, %r15
	popq	%r15
	callq	the_pragma_function
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000005, %r15
	popq	%r15
	callq	the_pragma_function
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
	callq	the_pragma_function
	pushq	%r15
	movq	$4000004, %r15
	popq	%r15
	callq	the_pragma_function
                                        # =>This Inner Loop Header: Depth=1
	leal	-1(%r15), %eax
	cmpl	_ZL2nt(%rip), %eax
	jl	.LBB8_7
	jmp	.LBB8_8
	.align	16, 0x90
.LBB8_9:                                # %for.body21
	callq	the_pragma_function
	pushq	%r15
	movq	$4000004, %r15
	popq	%r15
	callq	the_pragma_function
                                        #   in Loop: Header=BB8_8 Depth=1
	movq	(%r12,%r13,8), %rdi
	xorl	%esi, %esi
	callq	pthread_join
	incq	%r13
.LBB8_8:                                # %for.cond19
	callq	the_pragma_function
	pushq	%r15
	movq	$4000003, %r15
	popq	%r15
	callq	the_pragma_function
                                        # =>This Inner Loop Header: Depth=1
	cmpl	_ZL2nt(%rip), %r13d
	jl	.LBB8_9
# BB#10:                                # %for.end27
	callq	the_pragma_function
	pushq	%r15
	movq	$4000002, %r15
	popq	%r15
	callq	the_pragma_function
	xorl	%r14d, %r14d
	movq	-48(%rbp), %rsp         # 8-byte Reload
.LBB8_11:                               # %return
	callq	the_pragma_function
	pushq	%r15
	movq	$4000001, %r15
	popq	%r15
	callq	the_pragma_function
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
.Ltmp72:
	.size	main, .Ltmp72-main
	.cfi_endproc

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

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d"
	.size	.L.str, 3


	.ident	"clang version 3.5 (trunk 201826) (llvm/trunk 201825)"
	.ident	"clang version 3.5 (trunk 201826) (llvm/trunk 201825)"
	.section	".note.GNU-stack","",@progbits
