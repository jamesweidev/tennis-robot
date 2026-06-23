
# Tennis Robot Development

This repository documents my process of building the embedded, control, and vision subsystems for a tennis ball retrieving robot.

## Subsystems
- Autonomous Mobile Base
- Vision-Based Tennis Ball Tracking
- Launcher Subsystem
- Ball Feeder
- Integrated Prototype

## Current Focus

I am currently working on the autonomous mobile base. The first version uses ultrasonic obstacle avoidance, and later versions add PWM speed control, encoder feedback, and closed-loop drive correction.

## Firmware

The firmware for 01_obstacle_avoiding_robot uses bare metal custom drivers, while later subsystems use HAL. 