#include "mini_uart.h"
#include <stdint.h>

// Memory address used as a shared entry point table for secondary cores.
// QEMU uses this address to determine where each secondary core should start execution.
#define CORE_ENTRY_POINT_ADDR 0x00000000D8

extern void second_startup(void);

/*
 * Wakes up a secondary core (identified by core_id) by setting its entry point.
 * This is done by writing the address of the target function into a special memory location
 * that QEMU monitors. Once set, QEMU releases the corresponding core and starts execution
 * from that address.
 */
void wake_up_core(int core_id, void (*entry_point)(void)) {
    volatile uint64_t *core_entry_point_table = (volatile uint64_t *)CORE_ENTRY_POINT_ADDR;
    core_entry_point_table[core_id] = (uint64_t)entry_point;
}

/*
 * Main kernel function, executed only by core 0.
 * It wakes up secondary cores by assigning them an entry point,
 * initializes UART for serial output, and enters a loop to echo received characters.
 */
void kernel_main(void)
{
    wake_up_core(1, second_startup);
    wake_up_core(2, second_startup);
    wake_up_core(3, second_startup);

	uart_init();
	uart_send_string("Hello, world!\r\n");

	while (1) {
		uart_send(uart_recv());
	}
}

/*
 * Entry point function for each secondary core after it is released.
 * The core enters an infinite loop and executes the `wfi` instruction,
 * putting it into a low-power state until an interrupt is received.
 * This simulates an idle core waiting for work.
 */
void second_startup(void) {
    while(1) __asm__ volatile ("wfi");
}
