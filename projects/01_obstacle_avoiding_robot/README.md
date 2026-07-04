
# Autonomous Mobile Base

Basic STM32 based robot that drives around and avoids hitting objects. Meant to get used to HAL, beyond which it serves no purpose whatsoever.

## Versions
| Version | Feature | Status |
|---|---|---|
| v1 | Obstacle Avoidance with HC-SR04 | Done |
| v2 | With Encoder Feedback | In progress |
---

## Demos
All demos are (not yet) in 'media/' 

## V1: Obstacle Avoidance

1. Drives forward
2. Measures distance using HC-SR04
3. Turns or reverses when an object is too close

## Hardware

- STM32 Nucleo-F446RE
- HC-SR04 ultrasonic sensor
- L298N motor driver
- 4 DC motors
- Li-ion battery

## Wiring

| Component | MCU Pin | STM32 Function | Purpose |
|---|---|---|---|
| HC-SR04 TRIG | PA8 | GPIO Output | Ultrasonic trigger |
| HC-SR04 ECHO | PA9 | GPIO Input | Ultrasonic echo input |
| L298N IN1 | PA6 | GPIO Output | Right motor direction |
| L298N IN2 | PA7 | GPIO Output | Right motor direction |
| L298N IN3 | PA4 | GPIO Output | Left motor direction |
| L298N IN4 | PA5 | GPIO Output | Left motor direction |
| L298N ENA | PA0 | TIM4_CH1 PWM | Right motor speed |
| L298N ENB | PA1 | TIM3_CH1 PWM | Left motor speed |
| L298N GND | STM32 GND | Ground | Common ground |

## Firmware
Firmware is located at `firmware/STM32_ObjectAvoider/Core/V1_BasicAvoidance`

- GPIO for motor direction
- Timer/SysTick for delays
- Ultrasonic trigger/echo measurement
- Basic control logic in main loop


## V2: Distance Based PWM Speed Control

Incorporate encoder feedback loop and allow the robot to go in straight line


## Hardware Changes From V1

- New motor with encoder
- Chassis for the motors

## Firmware Changes From V1
Firmware is located at `firmware/STM32_ObjectAvoider/Core/V2_`

- Add a timer and feedback loop for the motor encoders