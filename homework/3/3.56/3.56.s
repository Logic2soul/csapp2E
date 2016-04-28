	.file	"3.56.c"
	.text
	.globl	loop
	.type	loop, @function
loop:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$16, %esp
	.cfi_offset 3, -12
	movl	$1431655765, -8(%ebp)
	movl	$-2147483648, -12(%ebp)
	jmp	.L2
.L3:
	movl	-12(%ebp), %eax
	movl	8(%ebp), %edx
	andl	%edx, %eax
	xorl	%eax, -8(%ebp)
	movl	12(%ebp), %eax
	movzbl	%al, %eax
	movl	-12(%ebp), %edx
	movl	%edx, %ebx
	movl	%eax, %ecx
	sarl	%cl, %ebx
	movl	12(%ebp), %eax
	movzbl	%al, %eax
	movl	$1, %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	subl	$1, %eax
	andl	%ebx, %eax
	movl	%eax, -12(%ebp)
.L2:
	cmpl	$0, -12(%ebp)
	jne	.L3
	movl	-8(%ebp), %eax
	addl	$16, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	loop, .-loop
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)"
	.section	.note.GNU-stack,"",@progbits
