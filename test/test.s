	.file	"test.c"
	.text
	.section	.rodata
.LC0:
	.string	"%d %d %d %d %d %d %d %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$10, -32(%rbp)
	movl	$20, -28(%rbp)
	movl	$30, -24(%rbp)
	movl	$40, -20(%rbp)
	movl	$50, -16(%rbp)
	movl	$50, -12(%rbp)
	movl	$50, -8(%rbp)
	movl	$50, -4(%rbp)
	movl	-16(%rbp), %r8d
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %ecx
	movl	-28(%rbp), %edx
	movl	-32(%rbp), %eax
	subq	$8, %rsp
	movl	-4(%rbp), %esi
	pushq	%rsi
	movl	-8(%rbp), %esi
	pushq	%rsi
	movl	-12(%rbp), %esi
	pushq	%rsi
	movl	%r8d, %r9d
	movl	%edi, %r8d
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addq	$32, %rsp
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 13.2.1 20230801"
	.section	.note.GNU-stack,"",@progbits
