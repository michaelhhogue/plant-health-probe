# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/Users/michaelhogue/Developer/SDKs/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(plant-health-probe C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(include)

# Add executable. Default name is the project name, version 0.1

add_executable(plant-health-probe 
  src/main.c 
  src/viewmode_select.c
  src/lcd.c
  src/graphics.c
  src/bh1750_light_sensor.c
  src/ds18b20.c
  src/soil_moisture_seesaw.c
)

pico_set_program_name(plant-health-probe "plant-health-probe")
pico_set_program_version(plant-health-probe "0.1")

pico_enable_stdio_uart(plant-health-probe 0)
pico_enable_stdio_usb(plant-health-probe 1)

# Add the standard library to the build
target_link_libraries(plant-health-probe
        pico_stdlib
        pico_multicore
        hardware_spi
        hardware_pio
        hardware_i2c)

# Add the standard include files to the build
target_include_directories(plant-health-probe PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

pico_add_extra_outputs(plant-health-probe)

