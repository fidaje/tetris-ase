# Tetris on LPC1768 – LandTiger Board

## Overview

This repository contains an implementation of the **Tetris** game for the **LandTiger Board** based on the **NXP LPC1768** microcontroller, developed using **Keil µVision**.

The project was developed in two phases:
- **Extra Point 1**: core Tetris gameplay implemented and tested using the Keil emulator
- **Extra Point 2**: extension of the game with advanced mechanics, hardware peripherals, and audio support

The final version fully complies with the specifications of both assignments for the course **Computer Architectures**.

⚠️ Some features (ADC-based speed control and audio) require **real hardware** and are not fully supported by the Keil emulator.

---

## Game Features (Extra Point 1)

### Core Gameplay
- Playing field of **20 x 10 blocks**
- Standard Tetris tetrominoes:
  - I, O, T, J, L, S, Z
- Pseudo-random tetromino generation using **LFSR**
- Tetromino movement, rotation, collision detection
- Line clearing with gravity applied to upper blocks
- Game over detection and restart logic

### Game States
- Game starts in **paused state** after reset
- **KEY1**:
  - Starts the game
  - Toggles pause / play during gameplay
- **KEY2**:
  - Activates **hard drop**

### Movement Mechanics
- Default falling speed: **1 block/second**
- **Soft drop**:
  - Holding joystick down increases speed to **2 blocks/second**
- **Hard drop**:
  - Instantly places the tetromino at the lowest valid position

### Scoring System
- Tetromino placement: **+10 points**
- Line clear:
  - 1 line → +100 points
  - N lines → +100 × N points
  - Tetris (4 lines) → +600 points
- High score is saved and preserved between games

### Lose Conditions
- Tetromino exceeds the top boundary after placement
- Newly generated tetromino overlaps existing blocks

---

## Extended Features (Extra Point 2)

### 🎚 Variable Speed Control (ADC)
- Game speed controlled by the **potentiometer**
  - 1 block/second at 0V
  - Up to 5 blocks/second at maximum voltage
- Soft drop doubles the **current** falling speed (not fixed)

---

### ⭐ Power-Ups
- Every **5 cleared lines**, a random power-up replaces an existing block
- Power-up activates when its line is cleared

Implemented power-ups:
- **Clear Half Lines**
  - Clears the bottom half of the playing field
  - Points are awarded as if lines were cleared normally
- **Slow Mode**
  - Forces speed to 1 block/second for 15 seconds
  - Original speed is restored after expiration

---

### ⚠️ Random Malus
- Every **10 cleared lines**, a new line appears at the bottom
- The line contains **7 randomly placed blocks**
- Existing lines are shifted upward
- Overflow above the top boundary results in **Game Over**

---

### 🔊 Audio
- Sound effects and background music via **DAC**
- Implemented using hardware timers
- Available only on real LandTiger Board

---

## Timers, Interrupts and Peripherals

The system relies on multiple hardware peripherals for precise timing and responsiveness:

| Peripheral | Purpose |
|-----------|--------|
| **RIT** | Joystick debouncing and input handling |
| **Timer0** | Controls tetromino falling timing |
| **Timer1** | Manages the duration of the *Slow Mode* power-up |
| **Timer2** | Handles music and sound playback |
| **ADC** | Reads potentiometer value for speed control |
| **DAC** | Audio output |

This architecture ensures clean separation of responsibilities and deterministic behavior.

---

## Controls

| Input | Action |
|------|--------|
| Joystick Left / Right | Move tetromino |
| Joystick Up | Rotate tetromino |
| Joystick Down | Soft drop |
| KEY1 | Start / Pause game |
| KEY2 | Hard drop |
| Potentiometer | Adjust falling speed |
| Reset | Restart game |

---

## Demo Video


The following video demonstrates the project according to the
**Extra Point 1 specifications**.
It shows the core gameplay, debugging session in Keil, and peripheral
interaction as required by the first part of the assignment.

https://github.com/user-attachments/assets/6ffae95c-7225-4528-b4f6-8b6969471061


