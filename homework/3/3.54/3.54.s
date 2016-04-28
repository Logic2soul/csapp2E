	.file	"3.54.c"
	.text
	.globl	decode2
	.type	decode2, @function
decode2:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	12(%ebp), %eax
	movl	16(%ebp), %edx
	subl	%eax, %edx
	movl	%edx, %eax
	xorl	8(%ebp), %eax
	movl	12(%ebp), %edx
	movl	16(%ebp), %ecx
	subl	%edx, %ecx
	movl	%ecx, %edx
	sall	$15, %edx
	sarl	$15, %edx
	imull	%edx, %eax
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	decode2, .-decode2
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)"
	.section	.note.GNU-stack,"",@progbits
