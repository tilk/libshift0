/* Code for the shift0/reset0 operators and the runcont function.
 * shift0.S
 *
 * Copyright (c) 2011, Marek Materzok <tilk@tilk.eu>
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *
 * The software is provided 'as is', without any warranty.
 */

#define STACKSIZE 8192
#define OFFSET_ESP 8188
#define OFFSET_EIP 8184
#define OFFSET_EBP 8180
    .text
    .cfi_sections   .debug_frame, .eh_frame
    .comm	marker,4
    .comm	stacks,64
	
    .p2align 4,,15
.globl reset_arg
	.type	reset_arg, @function
reset_arg:
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
# load the address of the function to call and of the argument
    movl    20(%esp), %ecx
    movl    24(%esp), %ebx
# remember the stack pointer
    movl    %esp, %edx
# switch the stack and push the old stack pointer
    movl    %eax, %esp
    addl    $STACKSIZE, %esp
    pushl   %edx
# do the call
    pushl   $reset_after
    jmp     reset_in
# shift0_arg was not called, do the cleanup
# go back to the old stack
reset_after:
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
	.size	reset_arg, .-reset_arg
    
    .p2align 4,,15
.globl reset_in
	.type	reset_in, @function
reset_in:
    .cfi_startproc
    pushl   %ebp
    .cfi_def_cfa_offset 8
    movl    %esp, %ebp
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
    pushl   %ebx
    call    *%ecx
    popl    %ebx
    popl    %ebp
    ret
    .cfi_endproc
	.size	reset_in, .-reset_in
	
    .p2align 4,,15
.globl shift0_arg
	.type	shift0_arg, @function
shift0_arg:
    .cfi_startproc
# pop the current stack from the stack of stacks
    decl    marker
    movl    marker, %ecx
    movl    $stacks, %edx
    movl    (%edx, %ecx, 4), %eax
# store the initial runcont
    movl    $runcont_clean, (%eax)
# store the callee-saved registers and the stack pointer
    movl    %esp, 4(%eax)
    movl    %ebp, 8(%eax)
    movl    %ebx, 12(%eax)
    movl    %esi, 16(%eax)
    movl    %edi, 20(%eax)
# calculate and store the stack size
    movl    %eax, %ecx
    addl    $STACKSIZE, %ecx
    subl    %esp, %ecx
    movl    %ecx, 24(%eax)
# remember the parameters
    movl    4(%esp), %edx
    movl    8(%esp), %ecx
# switch the stack
    movl    OFFSET_ESP(%eax), %esp
    .cfi_def_cfa_offset 8
    movl    OFFSET_EBP(%eax), %ebp
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
    .cfi_offset 3, -12
    .cfi_offset 6, -16
    .cfi_offset 7, -20
# call the parameter
    pushl   %ecx
    pushl   %eax
    call    *%edx
    addl    $8, %esp
# restore the callee-saved registers
    popl    %edi
    popl    %esi
    popl    %ebx
    leave
    ret
    .cfi_endproc
	.size	shift0_arg, .-shift0_arg
	
    .p2align 4,,15
.globl runcont_clean
	.type	runcont_clean, @function
runcont_clean:
    .cfi_startproc
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
# backup the stack
    movl    20(%esp), %ebx
    movl    $runcont_reentry, (%ebx)
    pushl   24(%ebx)
    call    malloc
    pushl   4(%ebx)
    pushl   %eax
    call    memcpy
    movl    %eax, 28(%ebx)
    addl    $12, %esp
# do the actual calling
    jmp     runcont_common
    .cfi_endproc
    .size   runcont_clean, .-runcont_clean

    .p2align 4,,15
.globl runcont_common
	.type	runcont_common, @function
runcont_common:
    .cfi_startproc
    .cfi_def_cfa_offset 8
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
    .cfi_offset 3, -12
    .cfi_offset 6, -16
    .cfi_offset 7, -20
# set-up the stack for running
    movl    %esp, OFFSET_ESP(%ebx)
    movl    %ebp, OFFSET_EBP(%ebx)
    movl    $runcont_after, OFFSET_EIP(%ebx)
# remember the parameter
    movl    24(%esp), %eax
# restore the callee-saved registers and the stack, then return
    movl    %ebx, %ecx
    movl    4(%ecx), %esp
    movl    8(%ecx), %ebp
    movl    12(%ecx), %ebx
    movl    16(%ecx), %esi
    movl    20(%ecx), %edi
    ret
    .cfi_personality 0x0,__runcont_personality
runcont_after:
# go back to the old stack
    popl    %esp
# store the runcont for dirtied continuations
    movl    20(%esp), %ebx
    movl    $runcont_dirty, (%ebx)
# restore the callee-saved registers and return
    popl    %edi
    popl    %esi
    popl    %ebx
    leave
    ret
    .cfi_endproc
    .size   runcont_common, .-runcont_common
	
    .p2align 4,,15
.globl runcont_dirty
	.type	runcont_dirty, @function
runcont_dirty:
    .cfi_startproc
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
# restore the original stack
    movl    20(%esp), %ebx
    movl    $runcont_reentry, (%ebx)
    pushl   24(%ebx)
    pushl   28(%ebx)
    pushl   4(%ebx)
    call    memcpy
    addl    $12, %esp
# do the actual calling
    jmp     runcont_common
    .cfi_endproc
    .size   runcont_dirty, .-runcont_dirty
	
    .p2align 4,,15
.globl runcont_reentry
	.type	runcont_reentry, @function
runcont_reentry:
    .cfi_startproc
    pushl   %ebp
    .cfi_def_cfa_offset 8
    movl    %esp, %ebp
    .cfi_offset 5, -8
    .cfi_def_cfa_register 5
    call    abort
    leave
    ret
    .cfi_endproc
    .size   runcont_reentry, .-runcont_reentry
    
    .p2align 4,,15
.globl runcont
	.type	runcont, @function
runcont:
    .cfi_startproc
# call the right runcont impl
    movl    4(%esp), %eax
    jmp    *(%eax)
    .cfi_endproc
    .size   runcont, .-runcont
	
