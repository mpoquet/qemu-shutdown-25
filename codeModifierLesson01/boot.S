#include "mm.h"

.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1
	and x0, x0, #0xFF              // Extraire l'id du core (bits 0 à 7)
    mov x19, x0  

	cbz x0, master                 // Si c'est le core 0, aller à la section master
    b .
master:
	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero

	mov	sp, #LOW_MEMORY
	bl	kernel_main	// should never come here
    b .
