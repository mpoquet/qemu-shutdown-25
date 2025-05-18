This document explains only the logic used to put the secondary cores (cores 1, 2, 3)
into a low-power wait state using the WFI (Wait For Interrupt) instruction in a 
bare-metal setup for the Raspberry Pi 3B emulated with QEMU.
Context:
- On boot, all ARMv8 cores start at the same entry point (_start).
- Only core 0 (the primary core) proceeds with system initialization.
- Other cores should be put into an idle/waiting state.

Execution Flow:
1. Each core identifies itself using:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, master   ; core 0 continues, others branch to an idle loop

2. Secondary cores are awakened from core 0 via:
    - kernel_main() calls wake_up_core(core_id, second_startup)
    - The function address is written into a known location (0xD8)
    - A SEV (Send Event) instruction is issued to wake them

3. Code executed by secondary cores:
    void second_startup(void) {
        while (1) {
            __asm__ volatile ("wfi");  // enter low-power wait loop
        }
    }

Why use WFI?
- Reduces power usage by halting the core until needed
- Core stays in sleep until it receives an event (e.g., SEV)
- Standard practice in ARM multicore systems when idle

Summary:
- Secondary cores are initially stuck (e.g., in a loop)
- Core 0 sets their entry point and sends SEV
- Once active, second_startup() puts them into an infinite WFI loop
