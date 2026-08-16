
# Mobile Ball Finder

This project combines project 1 and 2, and the goal is to have the robot find the tennis ball, then go towards it.


## Harware 

- STM32 Nucleo-F446RE
- Raspberry Pi 4
- HC-SR04 ultrasonic sensor
- 2 x DC Motor with encoder POLOLU-4865
- 2 x MPQ6612A motor driver
- 2200mAh LiPo battery
- USB2.0 1080P camera - innomaker U20CAM


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
| Left ENCA | PB4 | TIM3_CH1 IC | Left motor encoder A |
| Left ENCB | PB5 | TIM3_CH2 IC | Left motor encoder B |
| Right ENCA | PB6 | TIM4_CH1 IC | Right motor encoder A |
| Right ENCB | PB7 | TIM4_CH2 IC | Right motor encoder B |
| RPi Rx | PC6 | UART6 Rx | MCU to RPi UART Rx |
| RPi Tx | PC7 | UART6 Tx | MCU to RPi UART Tx |
