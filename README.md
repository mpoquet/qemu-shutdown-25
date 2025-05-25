# Project 

This project was carried out by **Aldebert Clément** and **Laurent Polzin**.

It contains four main folders, each focusing on a specific aspect of bare-metal development on the Raspberry Pi 3B:

### 1. `Lesson01Modify`
A modified version of [Lesson01 by M. Poquet](https://github.com/mpoquet/raspberry-pi-os-qemu/tree/master/src/lesson01).  
In this version, the secondary CPU cores (cores 1, 2, 3) are explicitly put into a wfi (Wait For Interrupt) state instead of staying in Qemu's raspi3 boot code (wfe loop).

### 2. `timer`
An attempt to wake up secondary CPU cores from the `wfi` instruction using per-core generic virtual timer interrupts.  
This version is inspired by [eggman's timer01 code](https://github.com/eggman/raspberrypi/tree/master/qemu-raspi3/timer01), but is currently **non-functional** under QEMU.  
We recommend using [M. Poquet’s Lesson03](https://github.com/mpoquet/raspberry-pi-os-qemu/tree/master/src/lesson03) as a more reliable base for timer and multicore experiments.

### 3. `cpu_charge`
A benchmark to illustrate the difference in CPU load between using `wfi` and QEMU’s default core behavior.  
It measures how the CPU usage changes when secondary cores are actively waiting (wfe) versus when they are in low-power `wfi` state.

### 4. `tysptTemplate`
A [Typst](https://typst.app/) template used for writing the final IRIT report.  
This folder contains formatting and style files designed for academic or technical documentation.

## License
- Code: Apache-2.0
- Everything else, in particular documentation and measurements: CC-BY-SA-4.0