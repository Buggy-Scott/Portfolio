ECE 3436 — Microprocessor Systems
Embedded ARM Cortex-M4 assembly programming on the TI TM4C123 LaunchPad, developed in Keil MDK-ARM / µVision.

Part of the ECE Archive — an open, self-maintained repository of course notes, homework, labs, and exam material from an undergraduate Electrical & Computer Engineering curriculum.

119 Files
10+ Labs & Projects
8 Topic Units
TM4C123 Target MCU

Course Overview
ECE 3436, Microprocessor Systems, is a hands-on embedded systems course focused on ARM Cortex-M4 assembly language programming and register-level hardware control. This folder collects two semesters of coursework — in-class quizzes (ICQs), homework sets, lecture slides, lab starter and completed projects, and full exam materials — all centered on programming the Texas Instruments TM4C123GH6PM microcontroller (the board used with the TI TM4C123 LaunchPad).

The work moves from number representation and basic register operations toward full hardware interaction — GPIO control and timer-driven programs — with each lab building directly on the instruction set covered in the homework immediately before it.
Skills Demonstrated
ARM Cortex-M4 assembly programming (Keil/ARM syntax) — arithmetic, data movement, and control-flow instructions
Register-level microcontroller programming without relying on vendor abstraction libraries
Embedded toolchain use: project setup, build configuration, and flash/debug workflows in Keil µVision
Direct GPIO and SysTick peripheral configuration on a real target device (TM4C123GH6PM, Cortex-M4)
Reading and interpreting build artifacts — map files, listing files, and linker output — to debug memory layout
Topics Covered
Topic
What It Covers
Number Systems & Directives
Binary/hex/BCD conversions, signed representation, and Keil assembler directives (EQU, AREA, EXPORT)
Registers & Special-Purpose Registers
General-purpose register usage, the program status register, and special-purpose register access on the Cortex-M4
Conditional Codes & Compare/Test
NZCV flag behavior, CMP/TST instructions, and conditional execution
Shift, Rotate & Logical Operations
LSL/LSR/ROR and bitwise AND/OR/EOR instructions for bit-level manipulation
Conditional Branching & Execution
Branch instructions, loop constructs, and program flow control in ARM assembly
Data Storage: RAM & ROM
Memory-mapped data placement, LDR/STR addressing modes, and the AREA/DATA memory model
GPIO
Direct register-level control of the TM4C123's GPIO ports for digital I/O (LED blink, switch input)
SysTick Timer
Configuring the Cortex-M4 SysTick peripheral for timed delays and periodic events

Labs & Projects
Each lab is a complete Keil µVision project — source (.s), build logs, linker maps, and (where applicable) the finished .axf binary — rather than just a written report.

Lab
Focus
Description
Lab 00a–c
Toolchain Setup
Installing and configuring Keil MDK-ARM/µVision and getting a first project building and flashing to the TM4C123 LaunchPad
Lab 1
Register-Based Statistics
A from-scratch assembly program computing group means and sum-of-squared-errors entirely in registers — no memory access, pure register arithmetic and SDIV/MUL instructions. Completed with a lab partner
Lab 2
Registers & Directives
Applying EQU/AREA directives and register-to-register data movement patterns
Lab 3 / Directives Project
Assembler Directives
A structured project exercising Keil assembler directives for data and code section layout
Lab 4
Conditional Branching
Implements branch-based control flow, building on the Conditional Branching & Execution unit
Registers 1 / 2
Special-Purpose Registers
Starter and completed projects working with the Cortex-M4's special-purpose registers
RAM/ROM Starter
Memory & Data Storage
Starter project for the RAM/ROM data-storage unit — placing and addressing data in memory versus flash
Shift/Rotate/Logic Activity
Bit Manipulation
In-class activity applying LSL/LSR/ROR and logical instructions
GPIO Blink Starter
GPIO
Register-level GPIO configuration to drive an onboard LED — the entry point for all later hardware I/O labs
Exam 1 Project
Cumulative Assessment
A timed programming exam implemented as a full Keil project, covering numbering, registers, and conditional logic

Featured Lab: Register-Based Statistics Calculator
Lab 1 is a pure-register assembly program (no memory reads or writes) that computes descriptive statistics — per-group means, a grand mean, and the sum of squared errors for each group — entirely using ARM Cortex-M4 arithmetic instructions. Values are loaded as immediates, summed with ADD, divided with the Cortex-M4's hardware SDIV instruction, and squared with MUL, with results tracked across ten dedicated registers (R0–R12) without ever touching RAM.

; Task 1 — Group Mean (register-only arithmetic)

MOV  R0, #AStudent1        ; load each group member's value

MOV  R1, #AStudent2

...

ADD  R5, R0, R1            ; running sum

ADD  R5, R5, R2

ADD  R5, R5, R3

ADD  R5, R5, R4

MOV  R6, #GroupN

SDIV R12, R5, R6           ; R12 = mean of Group A

; Task 2 — Sum of Squared Errors per group

MOV  R0, AStudent1

SUB  R1, R0, R12           ; x - mean

MUL  R1, R1, R1            ; (x - mean)^2

ADD  R8, R8, R1            ; accumulate SSE for Group A

This lab was completed with a partner and reflects the register-allocation discipline the course emphasizes: every intermediate result is assigned to a specific, documented register rather than spilled to memory — a deliberate constraint of the assignment.
Tools & Environment
Keil MDK-ARM / µVision — IDE, assembler, and debugger used for every lab and exam project
Target device: TI TM4C123GH6PM (ARM Cortex-M4F core, 256 KB flash, 32 KB SRAM)
CMSIS device support (TM4C123.h) for register and peripheral definitions
ULINK2/CMSIS-DAP flash/debug driver configuration for on-target programming



Content provided for educational reference. Follow your own institution's academic integrity policy and verify concepts independently.
