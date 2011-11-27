#define STACKSIZE 8192
#define OFFSET_ESP 8188
#define OFFSET_EIP 8184
#define OFFSET_EBP 8180
    .text
    .cfi_sections   .debug_frame, .eh_frame
    .comm	marker,4
    .comm	stacks,64
	.p2align 4,,15
.globl reset
	.type	reset, @function
reset:
    .cfi_startproc
    .cfi_personality 0x0,__reset_personality
    pushl   %ebp
    .cfi_def_cfa_offset 8
    movl    %esp, %ebp
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
# protect the callee-saved registers
    pushl   %ebx
    pushl   %esi
    pushl   %edi
    .cfi_offset 3, -12
    .cfi_offset 6, -16
    .cfi_offset 7, -20
# allocate the new stack
    pushl   $STACKSIZE
    call    malloc
    addl    $4, %esp
# push the stack on the stack of stacks
    movl    marker, %ecx
    movl    $stacks, %edx
    movl    %eax, (%edx, %ecx, 4)
    incl    marker
# load the address of the function to call
    movl    20(%esp), %ecx
# remember the stack pointer
    movl    %esp, %edx
# switch the stack and push the old stack pointer
    movl    %eax, %esp
    addl    $STACKSIZE, %esp
    pushl   %edx
# do the call
    call    *%ecx
# shift0 was not called, do the cleanup
# go back to the old stack
    popl    %esp
# pop the uncaptured stack from the stack of stacks
    decl    marker
    movl    marker, %ecx
    movl    $stacks, %edx
    movl    (%edx, %ecx, 4), %edx
# free it
    movl    %eax, %ebx
    push    %edx
    call    free
    addl    $4, %esp
    movl    %ebx, %eax
# restore the callee-saved registers
    popl    %edi
    popl    %esi
    popl    %ebx
    leave
    ret
    .cfi_endproc
	.size	reset, .-reset
	.p2align 4,,15
.globl shift0
	.type	shift0, @function
shift0:
    .cfi_startproc
    decl    marker
    movl    marker, %ecx
    movl    $stacks, %edx
    movl    (%edx, %ecx, 4), %eax
    movl    %esp, 4(%eax)
    movl    %ebp, 8(%eax)
    movl    %ebx, 12(%eax)
    movl    %esi, 16(%eax)
    movl    %edi, 20(%eax)
    movl    %eax, %ecx
    addl    $STACKSIZE, %ecx
    subl    %esp, %ecx
    movl    %ecx, (%eax)
    movl    4(%esp), %edx
    movl    OFFSET_ESP(%eax), %esp
    .cfi_def_cfa_offset 8
    movl    OFFSET_EBP(%eax), %ebp
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
    .cfi_offset 3, -12
    .cfi_offset 6, -16
    .cfi_offset 7, -20
    pushl   %eax
    call    *%edx
    addl    $4, %esp
    popl    %edi
    popl    %esi
    popl    %ebx
    leave
    ret
    .cfi_endproc
	.size	shift0, .-shift0
	.p2align 4,,15
.globl runcont
	.type	runcont, @function
runcont:
    .cfi_startproc
    pushl   %ebp
    .cfi_def_cfa_offset 8
    movl    %esp, %ebp
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
    pushl   %ebx
    pushl   %esi
    pushl   %edi
    .cfi_offset 3, -12
    .cfi_offset 6, -16
    .cfi_offset 7, -20
    movl    20(%esp), %ebx
    pushl   (%ebx)
    call    malloc
    pushl   4(%ebx)
    pushl   %eax
    call    memcpy
    movl    %esp, OFFSET_ESP(%ebx)
    movl    %ebp, OFFSET_EBP(%ebx)
    movl    $contclean, OFFSET_EIP(%ebx)
    movl    36(%esp), %eax
    movl    %ebx, %ecx
    movl    4(%ecx), %esp
    movl    8(%ecx), %ebp
    movl    12(%ecx), %ebx
    movl    16(%ecx), %esi
    movl    20(%ecx), %edi
    ret
contclean:
    popl    %esp
    movl    %eax, %ebx
    pushl   8(%esp)
    pushl   4(%esp)
    pushl   12(%esp)
    call    memcpy
    popl    %eax
    call    free
    addl    $20, %esp
    movl    %ebx, %eax
    popl    %edi
    popl    %esi
    popl    %ebx
    leave
    ret
    .cfi_endproc
    .size   runcont, .-runcont
/*
	.p2align 4,,15
.globl __reset_personality
	.type	__reset_personality, @function
__reset_personality:
    .cfi_startproc
    pushl   %ebp
    .cfi_def_cfa_offset 8
    movl    %esp, %ebp
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
    leave
    ret
    .cfi_endproc
    .size   __reset_personality, .-__reset_personality
*/	
