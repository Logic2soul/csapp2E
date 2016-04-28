	.file	"3.59.c"
	.text
	.globl	switch_prob
	.type	switch_prob, @function
switch_prob:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	subl	$28, %eax
	cmpl	$5, %eax
	ja	.L2
	movl	.L4(,%eax,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L4:
	.long	.L3
	.long	.L2
	.long	.L3
	.long	.L5
	.long	.L6
	.long	.L7
	.text
.L3:
	sall	$3, -4(%ebp)
	jmp	.L8
.L5:
	sarl	$3, -4(%ebp)
	jmp	.L8
.L6:
	movl	-4(%ebp), %edx
	movl	%edx, %eax
	sall	$3, %eax
	subl	%edx, %eax
	movl	%eax, -4(%ebp)
.L7:
	movl	-4(%ebp), %eax
	imull	-4(%ebp), %eax
	movl	%eax, -4(%ebp)
.L2:
	addl	$11, -4(%ebp)
.L8:
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	switch_prob, .-switch_prob
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)"
	.section	.note.GNU-stack,"",@progbits
