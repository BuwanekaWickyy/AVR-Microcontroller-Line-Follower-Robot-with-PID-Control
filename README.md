# AVR Microcontroller Line Follower Robot

A line-following robot project developed in **Embedded C** for an **AVR ATmega328P** microcontroller. This project demonstrates low-level hardware control, custom PID implementation, and serial communication for real-time debugging.

## Features

* **Line Tracking:** Follows a black line on a white surface using a custom error-sensing mechanism.
* **PID Control:** Utilizes a custom-implemented PID control loop to calculate the necessary motor speed adjustments for precise steering.
* **Motor Control:** Implements **Fast PWM** on Timer/Counter0 for smooth, variable speed control of two DC motors.
* **UART Communication:** Supports serial output for debugging, logging the real-time sensor error (`ERR`) to a terminal.

## Hardware Setup

The project is designed for an AVR ATmega328P (common in Arduino Uno boards, but programmed without the Arduino framework).

| Peripheral | AVR Pin/Port | Purpose |
| :--- | :--- | :--- |
| **Line Sensors (5)** | PC0 - PC4 (PORTC) | Digital inputs for reading line position. |
| **Left Motor Direction**| PD3 (D3), PD4 (D4) | Control direction of the Left Motor (via H-bridge). |
| **Left Motor PWM** | PD5 (D5) / OC0B | PWM input for speed control of the Left Motor. |
| **Right Motor Direction**| PB1 (D9), PB2 (D10) | Control direction of the Right Motor (via H-bridge). |
| **Right Motor PWM** | PD6 (D6) / OC0A | PWM input for speed control of the Right Motor. |
| **UART TX** | PD1 (TXD) | Transmit data for debugging. |

## ⚙️ Software & Control Logic

### 1. Sensor Reading (`read_sensor()`)

The line sensors (PC0-PC4) are read directly from the `PINC` register. The function assigns a weighted error (`err`) based on the sensor pattern:
| Pattern (S1-S5) | Example | `err` Value | Interpretation |
| :--- | :--- | :--- | :--- |
| `~S1~S2~S3S4S5` | `00011` | $-2$ | Slightly right (Robot needs to turn left) |
| `S1S2~S3S4S5` | `11011` | $0$ | Centered on the line |
| `S1S2S3~S4~S5` | `11100` | $+3$ | Far left (Robot needs to turn right) |

***Note:*** *The logic currently assumes '1' is black (line) and '0' is white (off-line), which is common for many reflective sensors, but should be verified based on the hardware.*

### 2. PID Control

The robot uses a standard PID algorithm to calculate motor adjustments (`dif`):

$$\text{dif} = (ERR \cdot K_P) + ((ERR - \text{last\_ERR}) \cdot K_D) + (\text{t\_err} \cdot K_I)$$

* $K_P$ (Proportional): Set to **0.05**. Corrects error based on the current position.
* $K_D$ (Derivative): Set to **0** (Disabled in the current configuration). Would be used to damp oscillations based on the rate of change of the error.
* $K_I$ (Integral): Set to **0** (Disabled in the current configuration). Would be used to eliminate steady-state error (bias).

### 3. Motor Actuation (`motor()`)

The calculated `dif` is added to the left motor base speed (`m1`) and subtracted from the right motor base speed (`m2`) to steer the robot:

* **Turn Right:** If `dif` is positive, `m1` increases and `m2` decreases.
* **Turn Left:** If `dif` is negative, `m1` decreases and `m2` increases.

Speed is controlled by setting the **Output Compare Registers** (`OCR0A` and `OCR0B`) for the Fast PWM mode.

## 💻 Compilation and Flashing

This code is intended to be compiled using the **AVR-GCC toolchain** and flashed using an ISP programmer (e.g., USBasp) via `avrdude`.
