
# Autonomous Mobile Base

Basic STM32 based robot that drives around and avoids hitting objects. Serves no purpose whatsoever.

## Versions
| Version | Feature | Status |
|---|---|---|
| v1 | Obstacle Avoidance with HC-SR04 | Done |
| v2 | Distance Based PWM | In progress |
---

## Demos
All demos are in 'media/' (not yet)

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


## V2: Distance Based PWM Speed Control

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