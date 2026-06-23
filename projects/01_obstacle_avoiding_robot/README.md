# Autonomous Mobile Base

## Goal

Build a basic STM32-based robot that drives forward and avoids obstacles using an ultrasonic sensor.

## Milestones
| Version | Feature | Status |
|---|---|---|
| v1 | Obstacle avoidance with ultrasonic sensor | Done |
| v2 | Distance-based PWM speed control | In progress |
| v3 | Encoder speed measurement | Planned |
| v4 | Closed-loop drive-straight control | Planned |

---

## V1: Obstacle Avoidance

## What It Does

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
| L298N IN1 | PA0 | GPIO Output | Right motor direction |
| L298N IN2 | PA1 | GPIO Output | Right motor direction |
| L298N IN3 | PA4 | GPIO Output | Left motor direction |
| L298N IN4 | PA5 | GPIO Output | Left motor direction |
| L298N ENA | PB6 | TIM4_CH1 PWM | Right motor speed |
| L298N ENB | PA6 | TIM3_CH1 PWM | Left motor speed |
| L298N GND | STM32 GND | Ground | Common ground |

## Firmware
Firmware is located in `firmware/obstacle_avoidance_stm32/`.

- GPIO for motor direction
- Timer/SysTick for delays
- Ultrasonic trigger/echo measurement
- Basic control logic in main loop

## Problems I Ran Into

- HC-SR04 echo is 5V and needs protection for STM32 input
- Timer configuration/debugging took time
- Motor supply ground must be shared with STM32 ground

## Media

- Robot Photo:
- Demo Video: 

## V2: Distance Based PWM Speed Control

## Goal

Improve the robot's movement by changing motor speed based on obstacle distance instead of only using simple forward/turn/reverse behavior.

## Planned Behavior

| Distance | Speed |
| --- | --- |
| Far | Fast |
| Medium | Slow | 
| Close | reverse or turn |

## Hardware Changes From V1

No Major Changes

## Firmware Changes From V1

- Map ultrasonic reading to speed level
- Adjust speed using pwm
- Overall make robot response less abrupt

## Media 

PWM Waveform: 
Demo: 