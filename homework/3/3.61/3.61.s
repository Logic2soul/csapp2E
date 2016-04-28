	.file	"3.61.c"
	.text
	.globl	var_prod_ele
	.type	var_prod_ele, @function
var_prod_ele:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$32, %esp
	movl	8(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, -16(%ebp)
	movl	8(%ebp), %edx
	subl	$1, %edx
	movl	%edx, -20(%ebp)
	movl	%eax, %edx
	movl	20(%ebp), %eax
	imull	%edx, %eax
	leal	0(,%eax,4), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -24(%ebp)
	movl	24(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	16(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -4(%ebp)
	movl	$0, -8(%ebp)
	movl	$0, -12(%ebp)
	jmp	.L2
.L3:
	movl	-12(%ebp), %edx
	movl	-24(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %edx
	movl	-4(%ebp), %eax
	movl	(%eax), %eax
	imull	%edx, %eax
	addl	%eax, -8(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -4(%ebp)
	addl	$4, -12(%ebp)
.L2:
	movl	8(%ebp), %eax
	sall	$2, %eax
	cmpl	-12(%ebp), %eax
	jg	.L3
	movl	-8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	var_prod_ele, .-var_prod_ele
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)"
	.section	.note.GNU-stack,"",@progbits
