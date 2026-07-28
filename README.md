
# Tennis Robot Development

This repository documents my process of building the embedded, control, and vision subsystems for a tennis ball retrieving robot.

## Subsystems
Each subsystem will have its own project located in 'projects/', all of which will eventually be combined into an integrated prototype.
- Basic Automonous Mobile Base (using ultrasonic sensor)
- Vision-Based Tennis Ball Tracking
- Mobile Ball Finder
- Roller Ball Collector
- Integrated Prototype

## Current Focus

Currently working on a basic autonomous base using an ultrasonic sensor, cameras will be used later. 

## Firmware

All firmware uses HAL and are located in their respective project directory.

There are two ways to build the stm32 projects:
1. Use the STM32Cube vscode extension, or
2. Manually run the build commands below at any project root (such as [./projects/01_obstacle_avoiding_robot/firmware/STM32_ObjectAvoider](./projects/01_obstacle_avoiding_robot/firmware/STM32_ObjectAvoider))

Generate the build/Debug directory:
```shell
cmake -S . -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
```
Build the executables (this builds every executable created in its project):
```**shell**
cmake --build build/Debug
```

To flash the executables onto the board, first install [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html#section-get-software-table) from STMicroelectronics if haven't already.

Then, either:
1. Use VSCode tasks, which are already setup in the .vscode/ of each project directory, or
2. Manually run the following command:
   
```shell
/PATH/TO/STM32_Programmer_CLI -c port=SWD -w build/Debug/<NAME_OF_THE_ELF>.elf -v -rst
```
For example:
```shell
/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/Resources/bin/STM32_Programmer_CLI -c port=SWD -w build/Debug/V2_WithEncoders.elf -v -rst
```