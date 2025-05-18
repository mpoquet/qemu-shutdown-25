.section ".text.boot"
.globl _start

_start:
    // Obtenir le numéro de cœur (MPIDR_EL1)
    mrs x0, mpidr_el1
    and x0, x0, #3        // extraire core ID

    // Sauter à l'adresse de démarrage si core != 0
    cbz x0, core0_start

    ldr x1, =0x400000D8   // Table des points d'entrée (adresse partagée)
    lsl x0, x0, #3        // x0 *= 8 (64 bits par core)
    ldr x2, [x1, x0]      // charger entry point pour ce core
    br x2                 // sauter vers l'adresse

core0_start:
    // Initialiser le stack pointer (à adapter à ton linker)
    ldr x0, =_stack_top
    mov sp, x0

    // Nettoyer la section .bss
    ldr x1, =__bss_start
    ldr x2, =__bss_end
    mov x3, #0
1:
    cmp x1, x2
    b.ge 2f
    str x3, [x1], #8
    b 1b
2:

    bl kernel_main
hang:
    wfe
    b hang

.globl disable_irq
disable_irq:
    msr   daifset, #2
    ret

// Réserve pour la pile
.section ".bss.stack"
.global _stack
.align 12
_stack:
    .space 0x1000
.global _stack_top
_stack_top:
