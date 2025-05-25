# Raspberry Pi 3B – Generic Timer Attempt to Wake Secondary Cores

This part of the project is a bare-metal experimental branch that explores how to wake up the secondary cores (cores 1, 2, and 3) on a Raspberry Pi 3B using the generic virtual timer (CNTV) and the `WFI` (Wait For Interrupt) instruction. It is designed to run under QEMU using the `qemu-raspi3` target.

This version builds on the main project described in the [Lesson01Modify README](../Lesson01Modify/README.md) and adds an attempt to use timer-based interrupt wakeups for secondary cores.

## Current Status

This implementation is currently non-functional. Although we configure virtual timers on each secondary core and enable IRQs, the cores do not appear to receive interrupts. As a result, they remain stuck in their `WFI` loop. This behavior is likely due to QEMU limitations or missing platform-specific interrupt routing.

For a more stable implementation, we recommend using [lesson03 from M. Poquet](https://github.com/mpoquet/raspberry-pi-os-qemu/tree/master/src/lesson03), which successfully manages secondary core wakeup and simple timers under QEMU.

## Objective

In a physical Raspberry Pi 3B, all cores boot from the same physical address. Firmware typically parks secondary cores, and operating systems must wake them.

In QEMU, only core 0 executes from `boot.S` by default. Secondary cores are idle until released with a `SEV` instruction. This version attempts to:

- Assign per-core entry points via a table at address `0x000000D8`
- Use `SEV` to release secondary cores
- Configure a virtual timer (`CNTV`) on each secondary core
- Enable IRQs and put each secondary core into `WFI`
- Use the timer interrupt to wake up each core from `WFI`

## References and Inspirations

- Eggman's QEMU Timer implementation:  
  https://github.com/eggman/raspberrypi/tree/master/qemu-raspi3/timer01  
  Used as a reference for setting up CNTV registers and core IRQ routing.

- M. Poquet’s Lesson 03:  
  https://github.com/mpoquet/raspberry-pi-os-qemu/tree/master/src/lesson03  
  A reliable base demonstrating core wake-up and timer setup in QEMU.

## Boot Flow Description

### `boot.S`

Core 0 initializes memory and jumps to `kernel_main`.


## kernel_main in kernel.c
Core 0 initializes the UART.

Then it uses wake_up_core() to assign second_startup() to cores 1, 2, and 3.

second_startup
Each secondary core performs the following:

Reads its core ID.

Routes its virtual timer interrupt using routing_cntv_to_core_irq(core_id).

Reads the timer frequency and sets a short timeout via cntv_tval_el0.

Enables the virtual timer and IRQs.

Enters an infinite WFI loop.

Example:

```c
void second_startup(void) {
    routing_cntv_to_core_irq(core_id);
    write_cntv_tval(freq + 10);
    enable_cntv();
    enable_irq();

    while (1) {
        __asm__ volatile ("wfi");
    }
}
```
## Limitations and Known Issues
QEMU does not simulate CNTV interrupts per core reliably.

Even though cntv_ctl_el0 and cntv_tval_el0 are configured, no interrupt seems to trigger.

routing_cntv_to_core_irq() may not be effective under QEMU, as interrupt routing is platform-dependent.

### What Works
Core 0 boots and initializes the UART.

Entry points for secondary cores are correctly written and read.

Secondary cores enter second_startup() and wait in WFI.

### What Does Not Work
CNTV timer interrupts are not delivered under QEMU.

Secondary cores are not woken from WFI.

IRQ-based wakeup is not functioning as expected.

## Suggestions for Improvement
Base the implementation on M. Poquet’s lesson03 example for verified behavior.

Consider using the physical timer (CNTP) instead of the virtual timer (CNTV).

Validate the use of IRQ routing under QEMU or test on real hardware.

Use software-generated interrupts (SGIs) to test basic IRQ handling.

CNTV Registers Used
Register	Description
cntfrq_el0	Timer frequency (Hz)
cntvct_el0	Current counter value
cntv_tval_el0	Timer interval (triggers interrupt when 0)
cntv_ctl_el0	Control register (enable/disable, mask)

Related Files
boot.S: Startup assembly and core entry point routing

kernel.c: UART initialization, core management, timer setup

mini_uart.c: UART driver

link.ld: Linker script (must match stack setup in assembly)
