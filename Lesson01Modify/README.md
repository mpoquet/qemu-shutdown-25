# Multicore Boot Logic for Raspberry Pi 3B (QEMU)

This document explains the logic used to manage secondary cores (cores 1, 2, 3) in a bare-metal setup for the Raspberry Pi 3B, emulated using QEMU. The goal is to place these secondary cores into a low-power wait state using the WFI (Wait For Interrupt) instruction, and later wake them up from the kernel.

Context:
- On real hardware, all ARMv8 cores begin execution at the same address, typically the `_start` symbol in `boot.S`.
- However, in QEMU's emulation of the Raspberry Pi 3, **only core 0 reaches `boot.S`**.
- Secondary cores are held in a parked state inside QEMU’s internal logic until they are explicitly released by the kernel.
- This is why the wake-up of secondary cores is handled from within `kernel.c`, not `boot.S`.

Execution Flow:
1. Core 0 executes `boot.S`:
    ```asm
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, master   // Core 0 continues; others loop (on real hardware)
    b .              // Secondary cores stuck here if running on real hardware
    ```
    On QEMU, secondary cores never reach this point—they are parked internally.

2. In `kernel.c`, core 0 uses the following logic to wake up other cores:
    ```c
    void wake_up_core(int core_id, void (*entry_point)(void)) {
        volatile uint64_t *core_entry_point_table = (volatile uint64_t *)0x00000000D8;
        core_entry_point_table[core_id] = (uint64_t)entry_point;
    }

    void kernel_main(void) {
        wake_up_core(1, second_startup);
        wake_up_core(2, second_startup);
        wake_up_core(3, second_startup);

        uart_init();
        uart_send_string("Hello, world!\r\n");

        while (1) {
            uart_send(uart_recv());
        }
    }
    ```

3. Each secondary core starts from the address written into the entry point table, and runs:
    ```c
    void second_startup(void) {
        while(1) __asm__ volatile ("wfi");  // Enter low-power wait state
    }
    ```

Why use WFI?
- `WFI` halts the processor until an interrupt is received, reducing power consumption.
- However, it is important to note that:
  - `WFI` **cannot be resumed with a `SEV` instruction alone**; a real interrupt must occur.
  - This is different from QEMU’s simulated behavior for `WFE`, which is not a true emulation—it is implemented as a NOP with some special handling.

Summary:
- In QEMU, **only core 0 executes the assembly bootloader (`boot.S`)**.
- Secondary cores are awakened manually from `kernel.c` by setting their entry points and using QEMU’s internal signaling mechanisms.
- Once active, the secondary cores execute a `WFI` loop to simulate idling.
- This approach is simplified, and inspired by the logic found in the `write_smpboot64` function in `qemu/hw/arm/raspi.c`.

Integration Instructions for Multicore Wake-Up Support:
To integrate this multicore boot logic into your own project, you simply need to add the two provided functions (`wake_up_core` and `second_startup`) to your `kernel.c`, simplify the `boot.S` file from your current lesson to only handle core 0, wake the secondary cores from `kernel_main`, and then compile and run your project as usual.


