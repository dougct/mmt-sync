	.file	"exe/postdom/test04.bc2"
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
	movl	%edi, %ebx
	testl	%esi, %esi
	#APP
	#NO_APP
	movl	$2, %r15d
	cmovgl	%esi, %r15d
	addl	%r15d, %ebx
	xorl	%eax, %eax
	.align	16, 0x90
.LBB0_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	movl	%eax, %r14d
	callq	the_pragma_function
	pushq	%r15
	movq	$2000001, %r15
	popq	%r15
	callq	the_pragma_function
	leal	1(%r14), %eax
	subl	%r15d, %ebx
	cmpl	%r15d, %ebx
	jg	.LBB0_1
# BB#2:                                 # %while.end
	callq	the_pragma_function
	pushq	%r15
	movq	$3000001, %r15
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
	pushq	%r14
	pushq	%rbx
.Ltmp16:
	.cfi_offset %rbx, -32
.Ltmp17:
	.cfi_offset %r14, -24
	#APP
	#NO_APP
	movslq	%edi, %r14
	movq	%r14, %rbx
	shrq	$63, %rbx
	addq	%r14, %rbx
	andq	$-2, %rbx
	callq	the_pragma_function
	pushq	%r15
	movq	$2000001, %r15
	popq	%r15
	callq	the_pragma_function
	subq	%rbx, %r14
	je	.LBB1_2
# BB#1:                                 # %if.then
	movl	$10, %edi
	movl	$3, %esi
	jmp	.LBB1_3
.LBB1_2:                                # %if.else
	movl	$100, %edi
	movl	$30, %esi
.LBB1_3:                                # %JointBB
	callq	the_pragma_function
	pushq	%r15
	movq	$3000001, %r15
	popq	%r15
	callq	the_pragma_function
	callq	_Z6divideii
	testq	%r14, %r14
	xorl	%edi, %edi
	callq	pthread_exit
.Ltmp18:
	.size	_Z10threadProgi, .Ltmp18-_Z10threadProgi
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
.Ltmp21:
	.cfi_def_cfa_offset 16
.Ltmp22:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp23:
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
.Ltmp24:
	.size	_Z12execute_testi, .Ltmp24-_Z12execute_testi
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
.Ltmp27:
	.cfi_def_cfa_offset 16
.Ltmp28:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp29:
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
.Ltmp30:
	.size	_Z12prepare_testi, .Ltmp30-_Z12prepare_testi
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
.Ltmp33:
	.cfi_def_cfa_offset 16
.Ltmp34:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp35:
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
.Ltmp36:
	.size	_Z14number_threadsv, .Ltmp36-_Z14number_threadsv
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
.Ltmp39:
	.cfi_def_cfa_offset 16
.Ltmp40:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp41:
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
.Ltmp42:
	.size	_Z12number_timesv, .Ltmp42-_Z12number_timesv
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
.Ltmp46:
	.cfi_def_cfa_offset 16
.Ltmp47:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp48:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%rbx
	pushq	%rax
.Ltmp49:
	.cfi_offset %rbx, -40
.Ltmp50:
	.cfi_offset %r14, -32
.Ltmp51:
	.cfi_offset %r15, -24
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
	movl	_ZL5repet(%rip), %r15d
	callq	the_pragma_function
	pushq	%r15
	movq	$2000001, %r15
	popq	%r15
	callq	the_pragma_function
	incl	%ebx
	cmpl	%r15d, %ebx
	jl	.LBB6_2
# BB#3:                                 # %for.end
	callq	the_pragma_function
	pushq	%r15
	movq	$3000001, %r15
	popq	%r15
	callq	the_pragma_function
	#APP
	#NO_APP
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
	#APP
	#NO_APP
	movl	(%rdi), %ebx
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
	subq	$24, %rsp
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
	callq	the_pragma_function
	pushq	%r15
	movq	$2000001, %r15
	popq	%r15
	callq	the_pragma_function
	movl	$-1, %r14d
	cmpl	$2, %ebx
	jl	.LBB8_11
# BB#1:                                 # %entry
	cmpl	$3, %ebx
	jg	.LBB8_11
# BB#2:                                 # %if.end
	movq	8(%r15), %rdi
	movl	$.L.str, %esi
	movl	$_ZL2nt, %edx
	xorl	%eax, %eax
	callq	sscanf
	callq	the_pragma_function
	pushq	%r15
	movq	$2000002, %r15
	popq	%r15
	callq	the_pragma_function
	cmpl	$3, %ebx
	jne	.LBB8_4
# BB#3:                                 # %if.then3
	movq	16(%r15), %rdi
	movl	$.L.str, %esi
	movl	$_ZL5repet, %edx
	xorl	%eax, %eax
	callq	sscanf
.LBB8_4:                                # %if.end6
	callq	the_pragma_function
	pushq	%r15
	movq	$3000002, %r15
	popq	%r15
	callq	the_pragma_function
	movl	_ZL2nt(%rip), %ebx
	callq	the_pragma_function
	pushq	%r15
	movq	$2000003, %r15
	popq	%r15
	callq	the_pragma_function
	cmpl	$0, %ebx
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
	movq	%rsp, %r14
	leaq	15(,%rax,8), %rax
	andq	$-16, %rax
	subq	%rax, %r14
	movq	%r14, -56(%rbp)         # 8-byte Spill
	movq	%r14, %rsp
	movl	$1, %r15d
	jmp	.LBB8_6
	.align	16, 0x90
.LBB8_12:                               # %for.body
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
	leal	-1(%r15), %r12d
	movl	_ZL2nt(%rip), %r13d
	callq	the_pragma_function
	pushq	%r15
	movq	$2000004, %r15
	popq	%r15
	callq	the_pragma_function
	cmpl	%r13d, %r12d
	jl	.LBB8_12
# BB#7:
	movq	-56(%rbp), %r14         # 8-byte Reload
	xorl	%ebx, %ebx
	jmp	.LBB8_8
	.align	16, 0x90
.LBB8_9:                                # %for.body21
                                        #   in Loop: Header=BB8_8 Depth=1
	movq	(%r14,%rbx,8), %rdi
	xorl	%esi, %esi
	callq	pthread_join
	incq	%rbx
.LBB8_8:                                # %for.cond19
	callq	the_pragma_function
	pushq	%r15
	movq	$3000004, %r15
	popq	%r15
	callq	the_pragma_function
                                        # =>This Inner Loop Header: Depth=1
	movl	_ZL2nt(%rip), %r15d
	callq	the_pragma_function
	pushq	%r15
	movq	$2000005, %r15
	popq	%r15
	callq	the_pragma_function
	cmpl	%r15d, %ebx
	jl	.LBB8_9
# BB#10:                                # %for.end27
	callq	the_pragma_function
	pushq	%r15
	movq	$3000005, %r15
	popq	%r15
	callq	the_pragma_function
	xorl	%r14d, %r14d
	movq	-48(%rbp), %rsp         # 8-byte Reload
.LBB8_11:                               # %return
	callq	the_pragma_function
	pushq	%r15
	movq	$3000001, %r15
	popq	%r15
	callq	the_pragma_function
	callq	the_pragma_function
	pushq	%r15
	movq	$3000003, %r15
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
