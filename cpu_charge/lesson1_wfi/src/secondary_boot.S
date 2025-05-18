// secondary_boot.s - Fichier de démarrage pour les cœurs secondaires
.globl _secondary_start

_secondary_start:                      // Étiquette modifiée
    mrs    x0, mpidr_el1              // Lire l'ID du processeur (MPIDR)
    and    x0, x0, #0xff              // Extraire les 8 bits de l'ID du processeur
    cmp    x0, #0                     // Comparer l'ID avec 0 (cœur principal)
    bne    secondary_core             // Si ce n'est pas le cœur principal, aller dans le code des cœurs secondaires

    b      _secondary_start           // Cœur principal ne doit pas entrer ici

secondary_core:
    // Cœurs secondaires entrent dans la boucle wfi pour attendre une interruption
    wfi                                 // Attendre une interruption
    b     secondary_core               // Revenir dans la boucle pour rester en veille
