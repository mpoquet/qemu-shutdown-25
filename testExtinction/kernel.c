#include <stdint.h>
#define CORE_ENTRY_POINT_ADDR 0x00000000D8  // Adresse de la table des points d'entrée des cores secondaire
extern void second_startup(void);
void wake_up_core(int core_id, void (*entry_point)(void)) {
    volatile uint64_t *core_entry_point_table = (volatile uint64_t *)CORE_ENTRY_POINT_ADDR;
    core_entry_point_table[core_id] = (uint64_t)entry_point; // Point d'entrée pour ce core

    __asm__ volatile ("sev");  // Envoyer un SEV pour réveiller le core secondaire
}
void kernel_main(void) {
    wake_up_core(1, second_startup);
    wake_up_core(2, second_startup);
    wake_up_core(3, second_startup);
    while (1) {
        // mis à 100% du cpu principal
    }
}
void second_startup(void) {
    // Boucle d'attente
    while (1) {
        __asm__ volatile ("wfi");  // Attendre un événement
    }
}

