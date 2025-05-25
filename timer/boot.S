.section ".text.boot"
.globl _start
// This section defines the entry point of the program. 
// It contains the boot code that runs first after the CPU is released from reset.
_start:
    mrs x0, mpidr_el1
    and x0, x0, #3

    cbz x0, core0_start

// This label marks the start of the initialization routine for core 0, 
// responsible for setting up the stack, clearing .bss, and entering the kernel.
core0_start:
    ldr x0, =_stack_top
    mov sp, x0

    // Nettoyer la section .bss
    ldr x1, =bss_begin
    ldr x2, =bss_end
    mov x3, #0
1:
    cmp x1, x2
    b.ge 2f
    str x3, [x1], #8
    b 1b
2:
    bl kernel_main

//Infinite loop used to safely halt execution after kernel_main returns (should never happen).
hang:
    wfi
    b hang

// Small utility function to disable IRQ interrupts by modifying the DAIF register.
.globl disable_irq
disable_irq:
    msr   daifset, #2
    ret

// This section reserves space for the stack and defines the top of the stack 
// for use during early initialization.
.section ".bss.stack"
.global _stack
.align 12
_stack:
    .space 0x1000
.global _stack_top
_stack_top:
