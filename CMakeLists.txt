# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK and extras (must be before project) 
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(mp3 C CXX ASM)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(mp3 
    main.cpp
    src/oled.cpp
    src/audio.cpp
    src/sdcard.cpp
    src/hw_config.c
    src/fonts/small.cpp
    src/fonts/medium.cpp
    )

# add the FatFs library
add_subdirectory(
    lib/FatFs/FatFs_SPI build
)

pico_set_program_name(mp3 "mp3")
pico_set_program_version(mp3 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(mp3 0)
pico_enable_stdio_usb(mp3 1)

# Add the library to the build
target_link_libraries(mp3
        pico_stdlib
        pico_audio_i2s
        hardware_i2c
        FatFs_SPI
        )

# Add the standard include files to the build
target_include_directories(mp3 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(mp3)

