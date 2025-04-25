.section ".text.boot"

.globl _start
_start:
    // Lire l'identifiant du core courant (MPIDR_EL1)
    mrs x0, mpidr_el1
    and x0, x0, #0xFF              // Extraire l'id du core (bits 0 à 7)
    mov x19, x0                    // Sauvegarder l'id du core dans x19

    cbz x0, master                 // Si c'est le core 0, aller à la section master

master:
    // Code pour le core 0, initialisation
    // Démarre kernel_main qui est la fonction principale du kernel
    bl kernel_main
    b .
