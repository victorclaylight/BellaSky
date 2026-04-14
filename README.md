# BellaSky DALI2/PIR Controller Firmware

This repository contains the firmware for the BellaSky custom DALI2/PIR controller built around the Microchip PIC18F26K83. The firmware drives DALI lighting outputs, monitors PIR inputs, and controls a relay output that follows the same occupancy-based timing sequence as the lighting load.

## Functional Summary

The application starts in `OFF`, waits for the DALI bus to settle, configures the DALI fade parameters, and programs three broadcast scenes used by the installation:

- Scene 1: OFF
- Scene 2: standby level at approximately 20%
- Scene 3: normal level at approximately 89 to 90%

Runtime behavior is occupancy-driven:

- A PIR trigger moves the system to `NORMAL` and drives the DALI load to scene 3.
- If no new PIR activity is detected for 5 minutes, the system moves to `STANDBY` and drives the DALI load to scene 2.
- If no PIR activity is detected for a further 20 minutes, the system moves to `OFF` and sends DALI OFF.
- The relay output is enabled in `NORMAL` and `STANDBY`, and disabled in `OFF`.
- Four local push-buttons provide manual UP and DOWN step control while in `NORMAL` mode.

The implementation is centered in [main.c](main.c), with PIR handling in [pir.c](pir.c), relay control in [relay.c](relay.c), button handling in [buttons.c](buttons.c), and DALI transport/configuration support in [dali.c](dali.c), [dali_init.c](dali_init.c), and [dali_fade.c](dali_fade.c).

## IEC 62386 / DALI Statement

The DALI communication layer is implemented using the Microchip built-in DALI-capable UART functionality in the PIC18F26K83 rather than a software bit-banged stack. In [dali_init.c](dali_init.c), UART1 is configured in `DALI Control Device` mode, with DALI bus timing parameters including:

- 1200 bps signaling
- 2 stop bits
- DALI idle timing between frames
- Hardware-backed DALI transmit mode on UART1

The application transmits standard IEC 62386 forward-frame commands used for control gear operation, including broadcast direct arc power control, OFF, UP, DOWN, scene recall, scene storage, fade-time configuration, and fade-rate configuration. The fade and configuration helpers in [dali_fade.c](dali_fade.c) explicitly follow the DTR0 plus command sequence defined in IEC 62386-102.

For this project, compliance with IEC 62386 timing and frame generation is based on Microchip's hardware DALI implementation in the PIC18F26K83 and the use of standard DALI command semantics in the firmware. Final product compliance for customer delivery remains subject to system-level verification of the complete hardware, bus interface, connected control gear, and installation.

## Source Files Relevant for Review

- [main.c](main.c): main state machine and application timing
- [app_init.c](app_init.c): peripheral and DALI-mode initialization
- [dali_init.c](dali_init.c): Microchip UART1 DALI hardware configuration
- [dali.c](dali.c): DALI forward-frame transmit helpers
- [dali_fade.c](dali_fade.c): IEC 62386 fade and DTR0-based configuration commands
- [pir.c](pir.c): debounced PIR input processing for four channels
- [relay.c](relay.c): relay output control
- [buttons.c](buttons.c): local button interrupt and debounce logic

## Build Environment

The project is intended for MPLAB X with XC8 and MCC-generated support files in [mcc_generated_files](mcc_generated_files). The repository also includes the MPLAB project file [BellaSkyDali5.mc3](BellaSkyDali5.mc3) and generated build configuration under [nbproject](nbproject).

## CE Approval Folder Use

This repository is intended to serve as the firmware reference in the CE approval package for the BellaSky custom controller. For regulatory review, the most relevant supporting material is:

- This README for firmware scope and behavior
- The source files listed above for implementation evidence
- [CE_Approval_Document_DALI2_PIR_Controller_v1.0.md](CE_Approval_Document_DALI2_PIR_Controller_v1.0.md) for the approval checklist and document set
