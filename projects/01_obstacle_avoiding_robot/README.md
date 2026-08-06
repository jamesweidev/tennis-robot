
# Autonomous Mobile Base

Basic STM32 based robot that drives around and avoids hitting objects. Used for learning the fundamentals, beyond which it serves no purpose whatsoever.

## Versions
| Version | Feature | Status |
|---|---|---|
| v1 | Obstacle Avoidance w/ HC-SR04 | Done |
| v2 | Relative Positioning w/ Encoders | Done |
| v3 | Collision Avoidance | Done |
---

## Demos
All demos are (not yet) in 'media/' 

## V1: Obstacle Avoidance

1. Drives forward
2. Measures distance using HC-SR04
3. Either turn or reverse when an object is too close

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
Firmware is located at [firmware/STM32_ObjectAvoider/Core/V1_BasicAvoidance](firmware/STM32_ObjectAvoider/Core/V1_BasicAvoidance)

- GPIO for motor direction
- Timer/SysTick for delays
- Ultrasonic trigger/echo measurement
- Basic control logic in main loop

---

## V2: Relative Positioning w/ Encoder

1. Robot can go straight using encoder feedback loop.
2. Can move forward a specific distance by counting encoder ticks

## Hardware Changes From V1

- Wheeltec MG310 motor (With encoder)
- Use MPQ6612A instead of L298N motor driver
  - Better efficiency
  - MG310 motor stall consumes max 2A, but L298N is only rated for 2A continuous, while the MPQ6612A is rated 5A continuous
- Uses a smaller Li-Po battery rather than the massive li-ion that includes BMS
- Use a voltage monitor and alert set to 3.3V per cell
- Updated Chassis
  - Thicker walls since V1 was quite fragile
  - Added a power switch mount.
  - Wider and thicker mount for ultrasonic sensor (ultrasonic sensor is not used for v2)

## Wiring

| Component | MCU Pin | STM32 Function | Purpose |
|---|---|---|---|
| HC-SR04 TRIG | PA8 | GPIO Output | Ultrasonic trigger |
| HC-SR04 ECHO | PA9 | GPIO Input | Ultrasonic echo input |
| MPQ6612A AIN1 | PA0 | TIM2_CH1 PWM | Right motor PWM |
| MPQ6612A AIN2 | PA1 | TIM2_CH2 PWM | Right motor PWM |
| MPQ6612A BIN1 | PB10 | TIM2_CH3 PWM | Left motor PWM |
| MPQ6612A BIN2 | PB2 | TIM2_CH4 PWM | Left motor PWM |
| MPQ6612A GND | STM32 GND | Ground | Common ground |
| Right ENCA | PB6 | TIM4_CH1 IC | Right motor encoder A |
| Right ENCB | PB7 | GPIO Input | Right motor encoder B |
| Left ENCA | PB8 | TIM4_CH3 IC | Left motor encoder A |
| Left ENCB | PB9 | GPIO Input | Left motor encoder B |


## Firmware Changes From V1
This version is overwritten by V3, but it can be accessed at [firmware/STM32_ObjectAvoider/Core/V2_WithEncoders](firmware/STM32_ObjectAvoider/Core/V2_WithEncoders) in tag p1v2.

- Input capture for counting encoder ticks
- 100ms timer that updates PID values and subsequently the motor speeds
- Smooth_Drive function that briefly pauses in between direction switches to avoid jitters
- Offset_Position function that turns by x degrees and moves forward by y meters

---

## V3: Collision Avoidance

1. Everything that V2 does
2. Add back the obstacle avoidance from V1, which can override the position commands in order to avoid collisions

## Harware 

Same as V2

## Wiring

| Component | MCU Pin | STM32 Function | Purpose |
|---|---|---|---|
| HC-SR04 TRIG | PA8 | GPIO Output | Ultrasonic trigger |
| HC-SR04 ECHO | PA9 | GPIO Input | Ultrasonic echo input |
| MPQ6612A AIN1 | PA0 | TIM2_CH1 PWM | Right motor PWM |
| MPQ6612A AIN2 | PA1 | TIM2_CH2 PWM | Right motor PWM |
| MPQ6612A BIN1 | PB10 | TIM2_CH3 PWM | Left motor PWM |
| MPQ6612A BIN2 | PB2 | TIM2_CH4 PWM | Left motor PWM |
| MPQ6612A GND | STM32 GND | Ground | Common ground |
| Right ENCA | PB6 | TIM4_CH1 IC | Right motor encoder A |
| Right ENCB | PB7 | GPIO Input | Right motor encoder B |
| Left ENCA | PB8 | TIM4_CH3 IC | Left motor encoder A |
| Left ENCB | PB9 | GPIO Input | Left motor encoder B |
| RPi Rx | PC6 | UART6 Rx | MCU to RPi UART Rx |
| RPi Tx | PC7 | UART6 Tx | MCU to RPi UART Tx |

## Firmware Changes From V2

Firmware is located at [firmware/STM32_ObjectAvoider/Core/V3_CollisionAvoidance](firmware/STM32_ObjectAvoider/Core/V3_CollisionAvoidance).

- Refactor robot's blocking control logic to use non blocking state machine.
- Add an OBSTACLE_DETECTED state, which stops the robot in whatever it's doing, and reverses until the obstacle is far enough away.