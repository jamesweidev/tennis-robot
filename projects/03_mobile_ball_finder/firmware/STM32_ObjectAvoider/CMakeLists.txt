cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME ObjectAvoider)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME} C ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

add_subdirectory(cmake/stm32cubemx)

# Enable floating point
set(FPU_COMPILE_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

#################### Testing ####################
# add_executable(Testing
#     Core/Testing/Src/main.c
#     Core/Testing/Src/stm32f4xx_hal_msp.c
#     Core/Testing/Src/stm32f4xx_it.c
# )

# target_include_directories(Testing PRIVATE
#     Core/Testing/Inc
# )

# target_link_libraries(Testing PRIVATE
#     STM32_Drivers
# )

# target_compile_options(Testing PRIVATE
#     ${FPU_COMPILE_FLAGS}
# )

# target_link_options(Testing PRIVATE
#     ${FPU_COMPILE_FLAGS}
#     -u _printf_float
# )

#################### BallFinder ####################
add_executable(BallFinder
    Core/BallFinder/Src/main.c
    Core/BallFinder/Src/debug_uart.c
    Core/BallFinder/Src/encoder.c
    Core/BallFinder/Src/motor.c
    Core/BallFinder/Src/pid.c
    Core/BallFinder/Src/ultrasonic.c
    Core/BallFinder/Src/stm32f4xx_hal_msp.c
    Core/BallFinder/Src/stm32f4xx_it.c
    Core/BallFinder/Src/state.c
    Core/BallFinder/Src/command_receiver.c
)

target_include_directories(BallFinder PRIVATE
    Core/BallFinder/Inc
)

target_link_libraries(BallFinder PRIVATE
    STM32_Drivers
)

target_compile_options(BallFinder PRIVATE
    ${FPU_COMPILE_FLAGS}
)

target_link_options(BallFinder PRIVATE
    ${FPU_COMPILE_FLAGS}
    -u _printf_float
)



# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
