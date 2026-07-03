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

set(FPU_COMPILE_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

#################### ObjectAvoidingRobot ####################
add_executable(ObjectAvoidingRobot
    Core/ObjectAvoidingRobot/Src/main.c
    Core/ObjectAvoidingRobot/Src/stm32f4xx_hal_msp.c
    Core/ObjectAvoidingRobot/Src/stm32f4xx_it.c
)

target_include_directories(ObjectAvoidingRobot PRIVATE
    Core/ObjectAvoidingRobot/Inc
)

target_link_libraries(ObjectAvoidingRobot PRIVATE
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

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
