#include "mini_uart.h"
#include <stdint.h>

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

void wake_up_core(int core_id, void (*entry_point)(void)) {
    if (core_id < 1 || core_id > 3) return;

    volatile uint64_t *core_entry_point_table = (volatile uint64_t *)CORE_ENTRY_POINT_ADDR;
    core_entry_point_table[core_id] = (uint64_t)entry_point;

    __asm__ volatile ("dsb sy");  // Ensure visibility
    __asm__ volatile ("sev");     // Wake up sleeping cores
}

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

void enable_cntv(void)
{
    uint64_t cntv_ctl = 1;
    asm volatile ("msr cntv_ctl_el0, %0" :: "r" (cntv_ctl));
}

void disable_cntv(void)
{
    uint64_t cntv_ctl = 0;
    asm volatile ("msr cntv_ctl_el0, %0" :: "r" (cntv_ctl));
}

uint64_t read_cntfrq(void)
{
    uint64_t val;
    asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

uint64_t read_cntvct(void)
{
    uint64_t val;
    asm volatile ("mrs %0, cntvct_el0" : "=r" (val));
    return val;
}

void write_cntv_tval(uint64_t val)
{
    asm volatile ("msr cntv_tval_el0, %0" :: "r" (val));
}

void enable_irq(void)
{
    asm volatile (
        "msr daifclr, #2\n"  // Clear IRQ mask (enable IRQ)
    );
}


// Stub to route timer interrupt (to be implemented for your platform)
void routing_cntv_to_core_irq(int core_id)
{
    volatile uint64_t *irq_reg = (volatile uint64_t *)(
        core_id == 1 ? CORE1_TIMER_IRQCNTL :
        core_id == 2 ? CORE2_TIMER_IRQCNTL :
        CORE3_TIMER_IRQCNTL);

    *irq_reg = 1; // Route virtual timer interrupt to the core
}
