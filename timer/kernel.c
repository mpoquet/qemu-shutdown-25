#include "mini_uart.h"
#include <stdint.h>

// Memory address used as a shared entry point table for secondary cores.
// QEMU uses this address to determine where each secondary core should start execution.
#define CORE1_TIMER_IRQCNTL  0x0000000044
#define CORE2_TIMER_IRQCNTL  0x0000000048
#define CORE3_TIMER_IRQCNTL  0x000000004C
#define CORE0_IRQ_SOURCE     0x0000000060
#define CORE_ENTRY_POINT_ADDR 0x00000000D8

void wake_up_core(int core_id, void (*entry_point)(void));
void kernel_main(void);
void second_startup(void) __attribute__((noreturn));

void enable_cntv(void);
extern void disable_cntv(void);
uint64_t read_cntfrq(void);
uint64_t read_cntvct(void);
void write_cntv_tval(uint64_t val);
void enable_irq(void);
void routing_cntv_to_core_irq(int core_id);
/*
 * Wakes up a secondary core (identified by core_id) by setting its entry point.
 * This is done by writing the address of the target function into a special memory location
 * that QEMU monitors. Once set, QEMU releases the corresponding core and starts execution
 * from that address.
 */
void wake_up_core(int core_id, void (*entry_point)(void)) {
    if (core_id < 1 || core_id > 3) return;

    volatile uint64_t *core_entry_point_table = (volatile uint64_t *)CORE_ENTRY_POINT_ADDR;
    core_entry_point_table[core_id] = (uint64_t)entry_point;

    __asm__ volatile ("dsb sy");  // Ensure visibility
}
/*
 * Main kernel function, executed only by core 0.
 * It wakes up secondary cores by assigning them an entry point,
 * initializes UART for serial output, and enters a loop to echo received characters.
 */
void kernel_main(void)
{
    uart_init();
    uart_send_string("Hello, world!\r\n");

    wake_up_core(1, second_startup);
    wake_up_core(2, second_startup);
    wake_up_core(3, second_startup);

    while (1) {
        uart_send(uart_recv());
    }
}
/**
 * @brief Startup routine for secondary cores (cores 1–3).
 *
 * Identifies the core ID, configures the virtual timer interrupt routing,
 * sets a short timer delay, enables the virtual timer and IRQs,
 * and finally enters a low-power wait loop using WFI.
 */
void second_startup(void) __attribute__((noreturn));
void second_startup(void) {
    uint64_t mpidr;
    asm volatile("mrs %0, mpidr_el1" : "=r" (mpidr));
    int core_id = mpidr & 0xFF;

    routing_cntv_to_core_irq(core_id);

    uint64_t freq = read_cntfrq();
    write_cntv_tval(freq+10);
    enable_cntv();
    enable_irq();

    while (1) {
        __asm__ volatile ("wfi");
    }
}
/**
 * @brief Enables the virtual timer (CNTV).
 *
 * Writes to the CNTV control register to activate the timer.
 */
void enable_cntv(void)
{
    uint64_t cntv_ctl = 1;
    asm volatile ("msr cntv_ctl_el0, %0" :: "r" (cntv_ctl));
}

/**
 * @brief Disables the virtual timer (CNTV).
 *
 * Writes to the CNTV control register to stop the timer.
 */
void disable_cntv(void)
{
    uint64_t cntv_ctl = 0;
    asm volatile ("msr cntv_ctl_el0, %0" :: "r" (cntv_ctl));
}

/**
 * @brief Reads the system timer frequency (CNTFRQ_EL0).
 *
 * @return Timer frequency in Hz.
 */
uint64_t read_cntfrq(void)
{
    uint64_t val;
    asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

/**
 * @brief Reads the current virtual counter value (CNTVCT_EL0).
 *
 * @return Current timer count.
 */
uint64_t read_cntvct(void)
{
    uint64_t val;
    asm volatile ("mrs %0, cntvct_el0" : "=r" (val));
    return val;
}

/**
 * @brief Sets the virtual timer compare value (CNTV_TVAL_EL0).
 *
 * @param val Value to set in the timer to trigger an interrupt.
 */

void write_cntv_tval(uint64_t val)
{
    asm volatile ("msr cntv_tval_el0, %0" :: "r" (val));
}

/**
 * @brief Enables IRQs by clearing the interrupt mask bits.
 *
 * Clears the I-bit in DAIF, allowing IRQs to be received.
 */
void enable_irq(void)
{
    asm volatile (
        "msr daifclr, #2\n"  // Clear IRQ mask (enable IRQ)
    );
}


/**
 * @brief Routes the virtual timer interrupt to the specified core.
 *
 * Writes to a memory-mapped IRQ control register to enable virtual timer
 * interrupts on the target core. The exact behavior depends on platform support
 * (may not be effective in QEMU).
 *
 * @param
void routing_cntv_to_core_irq(int core_id)
{
    volatile uint64_t *irq_reg = (volatile uint64_t *)(
        core_id == 1 ? CORE1_TIMER_IRQCNTL :
        core_id == 2 ? CORE2_TIMER_IRQCNTL :
        CORE3_TIMER_IRQCNTL);

    *irq_reg = 1; // Route virtual timer interrupt to the core
}
