#include "mini_uart.h"
#include <stdint.h>

#define CORE_ENTRY_POINT_ADDR 0x00000000D8  // Adresse de la table des points d'entrée des cores secondaires

// Déclaration d'une fonction pour que chaque core secondaire commence là
extern void second_startup(void);

extern void _passiv_loop(void);

void wake_up_core(int core_id, void (*entry_point)(void)) {
    volatile uint64_t *core_entry_point_table = (volatile uint64_t *)CORE_ENTRY_POINT_ADDR;
    core_entry_point_table[core_id] = (uint64_t)entry_point; // Point d'entrée pour ce core

    __asm__ volatile ("sev");  // Envoyer un SEV pour réveiller le core secondaire
}

void kernel_main(void)
{
	wake_up_core(1, second_startup);

    // Réveiller le core 2
    wake_up_core(2, second_startup);

    // Réveiller le core 3
    wake_up_core(3, second_startup);

	uart_init();
	uart_send_string("Hello, world!\r\n");

    char c;
	while (1) {
        c = uart_recv();
        if ('1' <= c && c <= '3') {
            uart_send(c);
            wake_up_core(c - '0', _passiv_loop);
        }
	}
}

void second_startup(void) {
    // Code pour les cores secondaires (tous les cores autres que le 0)
    // Initialiser la pile (si nécessaire)
    // Boucle d'attente
    while (1) {
        __asm__ volatile ("wfi");  // Attendre un événement
    }
}
