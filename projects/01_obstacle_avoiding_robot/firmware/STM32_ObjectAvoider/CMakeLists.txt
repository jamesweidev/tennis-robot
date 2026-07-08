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

#################### ObjectAvoidingRobot ####################
add_executable(V1_BasicAvoidance
    Core/V1_BasicAvoidance/Src/main.c
    Core/V1_BasicAvoidance/Src/stm32f4xx_hal_msp.c
    Core/V1_BasicAvoidance/Src/stm32f4xx_it.c
)

target_include_directories(V1_BasicAvoidance PRIVATE
    Core/V1_BasicAvoidance/Inc
)

target_link_libraries(V1_BasicAvoidance PRIVATE
    STM32_Drivers
)

#################### EncoderTesting ####################
add_executable(EncoderTesting
    Core/EncoderTesting/Src/main.c
    Core/EncoderTesting/Src/stm32f4xx_hal_msp.c
    Core/EncoderTesting/Src/stm32f4xx_it.c
)

target_include_directories(EncoderTesting PRIVATE
    Core/EncoderTesting/Inc
)

target_link_libraries(EncoderTesting PRIVATE
    STM32_Drivers
)

target_compile_options(EncoderTesting PRIVATE
    ${FPU_COMPILE_FLAGS}
)

target_link_options(EncoderTesting PRIVATE
    ${FPU_COMPILE_FLAGS}
    -u _printf_float
)


#################### Testing ####################
add_executable(Testing
    Core/Testing/Src/main.c
    Core/Testing/Src/stm32f4xx_hal_msp.c
    Core/Testing/Src/stm32f4xx_it.c
)

target_include_directories(Testing PRIVATE
    Core/Testing/Inc
)

target_link_libraries(Testing PRIVATE
    STM32_Drivers
)

target_compile_options(Testing PRIVATE
    ${FPU_COMPILE_FLAGS}
)

target_link_options(Testing PRIVATE
    ${FPU_COMPILE_FLAGS}
    -u _printf_float
)


#################### V2_WithEncoders ####################
add_executable(V2_WithEncoders
    Core/V2_WithEncoders/Src/main.c
    Core/V2_WithEncoders/Src/debug_uart.c
    Core/V2_WithEncoders/Src/encoder.c
    Core/V2_WithEncoders/Src/motor.c
    Core/V2_WithEncoders/Src/pid.c
    Core/V2_WithEncoders/Src/ultrasonic.c
    Core/V2_WithEncoders/Src/stm32f4xx_hal_msp.c
    Core/V2_WithEncoders/Src/stm32f4xx_it.c
)

target_include_directories(V2_WithEncoders PRIVATE
    Core/V2_WithEncoders/Inc
)

target_link_libraries(V2_WithEncoders PRIVATE
    STM32_Drivers
)

target_compile_options(V2_WithEncoders PRIVATE
    ${FPU_COMPILE_FLAGS}
)

target_link_options(V2_WithEncoders PRIVATE
    ${FPU_COMPILE_FLAGS}
    -u _printf_float
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
