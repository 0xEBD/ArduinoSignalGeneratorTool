# Arduino Signal Generator Tool

Digital Pattern Generator for Arduino, designed to control 8 channels (D2–D9) with customizable bit patterns, sequence lengths, and delays. This project allows users to generate precise digital signals for various applications.
Perfect for testing digital ICs, or creating customized signal sequences.



---

## Disclaimer
This tool and documentation is fully vibe coded using chatGPT and no optimization has been performed! Additionally, this repository's code is not optimized and may contain inefficiencies.

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Design Principles](#design-principles)
- [Hardware Setup](#hardware-setup)
- [Usage](#usage)
  - [Menu Overview](#menu-overview)
  - [Set Bit Pattern](#set-bit-pattern)
  - [Configure Sequence Length](#configure-sequence-length)
  - [Run/Stop Sequence](#runstop-sequence)
- [Data Structure](#data-structure)
- [Future Extensions](#future-extensions)

---

## Features

- 8 digital output channels (D2 to D9)
- Per-channel bit sequence definition using "1" and "0" strings
- Shared sequence length (1–255 steps)
- Configurable delay between steps
- Interactive serial console menu
- Sequence output mode: one-shot or repeated

---

## Architecture

```
          [Serial Console]
                 |
        [User Command Parser]
                 |
    -------------------------------
    |       Configuration Store   |
    | - sequenceLength (uint8_t) |
    | - delayMs (unsigned int)   |
    | - channelPatterns[8]       |
    -------------------------------
                 |
           [Sequencer Engine]
                 |
       [8x Digital Output Pins]
           (D2 to D9 on Arduino)
```

---

## Design Principles

- **Simplicity**: Fully controlled via serial console, no additional UI
- **Clear State Separation**: Patterns, timing, and run status handled independently
- **Modular Code Structure**: Menu, config, and sequence loop handled in distinct logic blocks
- **Error Handling**: All inputs validated (length, valid characters)

---

## Hardware Setup

| Channel | Pin |
|---------|-----|
| 0       | D2  |
| 1       | D3  |
| 2       | D4  |
| 3       | D5  |
| 4       | D6  |
| 5       | D7  |
| 6       | D8  |
| 7       | D9  |

> All pins are configured as `OUTPUT`. Standard logic level is 5 V (Arduino Uno).

---

## Usage

### Menu Overview

When the Arduino boots, the serial console (9600 baud) displays:

```
=== Digital Pattern Generator ===
1) Set bit pattern for each channel
2) Set sequence length
3) Set delay between steps (ms)
4) Start sequence output
5) Stop output
6) Show current configuration
7) Exit
>
```

### Set Bit Pattern

- You are prompted to choose a channel (0–7)
- Then, input a binary string of exactly `sequenceLength` bits

Example:
```
Select channel (0–7): > 3
Enter pattern (exactly 8 bits, e.g. 10110011): > 1011
Error: Pattern length must be 8. Please try again.
Enter pattern (exactly 8 bits, e.g. 10110011): > 10110011
Pattern for channel 3 updated to: 10110011
```

### Configure Sequence Length

- Sets how many bits from each pattern will be used
- Applies to **all channels**

Example:
```
Enter new sequence length (1–255): > 10
```

Existing patterns will be extended with zeros or truncated accordingly.

### Configure Delay Between Steps

- Time between each output state change (in milliseconds)

Example:
```
Enter delay between steps in ms: > 100
```

### Run/Stop Sequence

- Start: executes the sequence for `sequenceLength` steps
- While running, pressing `5` in the serial console stops output

---

## Data Structure

```cpp
String channelPatterns[8];  // Bit patterns (e.g. "10101010")
uint8_t sequenceLength = 8;
unsigned int delayMs = 100;
bool running = false;
const uint8_t outputPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};
```

---

## Future Extensions

- none planed
---
---

## Example Use Case: SNES Controller Reader Test

This project was used to test interfacing with a Super Nintendo (SNES) controller by generating precise digital patterns that simulate the expected latch and clock signals.

### SNES Controller Interface Details

The SNES controller communicates via a simple 3-wire synchronous serial protocol:

- **LATCH** (output from Arduino): Pulsed HIGH to signal the controller to capture button states  
- **CLOCK** (output from Arduino): Clock pulses shift button data out of the controller  
- **DATA** (input to Arduino): Serial data line carrying one bit per button (active LOW)

Typical timing sequence:

- Set `LATCH` HIGH for 12 μs, then LOW  
- Pulse `CLOCK` LOW–HIGH 16 times to read all buttons  
- Read `DATA` on each rising edge of `CLOCK`

Using the pattern generator, LATCH and CLOCK signals were defined on two channels (e.g., D2 for LATCH and D3 for CLOCK), allowing precise control over timing to validate the controller’s behavior and test a digital reader implementation.
---
## License

MIT License. Use and modify freely.

---

## Author

Designed and built by Andi using Arduino Uno and serial console magic.
