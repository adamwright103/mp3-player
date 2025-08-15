# MP3 Player

This project is an MP3 player built using the RP2040-based Raspberry Pi Pico microcontroller. It features an OLED screen for display and control, a PCM5102 DAC for high-quality audio output, and a microSD card reader for storage.

## What I am learning

### Embedded Systems Development

- **Microcontroller Programming**: I am delving much deeper into programming on the Raspberry Pi Pico compared to some of my other projects using an arduino. This project is written in C++ using the pico SDK, where as I was using MicroPython and lots of premade librarys before. I minimised my use of libraries to learn how each peripheral communicates, I used I2C, I2S, and a custom programable I/O. I also learnt more about real time opperating systems, expanding on what I had learnt at university.

### Real-Time Systems

- **Audio Playback**: Implementing smooth and high-quality audio playback required understanding real-time constraints and optimizing data flow between the microcontroller and peripherals.
- **Display Updates**: Managing real-time updates to the OLED screen taught me how to balance performance and responsiveness in embedded systems.

### Problem Solving
- **Debugging Embedded Systems**: I developed skills in debugging hardware and software issues, including troubleshooting communication protocols and optimizing performance. My multimeter was a life saver here.
- **Power Management**: Implementing a battery level indicator taught me how to measure and display power levels effectively in embedded devices.

## Features

- **Microcontroller**: Raspberry Pi Pico (RP2040)
- **Display**: SSD1306 OLED screen
- **Audio Output**: PCM5102 DAC
- **Storage**: MicroSD card reader
- **Battery Level Indicator**: Displays battery percentage on the OLED screen
- **Song and Artist Display**: Shows the current song title and artist name on the OLED screen

## Setup

1. **Pin Out**

   - **MicroSD Reader**:
     - GPIO 1 -> CS
     - GPIO 2 -> CLK
     - GPIO 3 -> MOSI
     - GPIO 4 -> MISO

   - **User Input Buttons**:
     - GPIO 6 -> Left Button
     - GPIO 7 -> Home Button
     - GPIO 8 -> Right Button

   - **DAC (PCM5102)**:
     - GPIO 10 -> DIN
     - GPIO 11 -> LRCLK
     - GPIO 12 -> BCK

   - **OLED Display**:
     - GPIO 18 -> SDA
     - GPIO 19 -> SCL

1. **Software**:
   - Install the Pico SDK.
   - Clone this repository and configure the project using CMake.
   - Flash the .uf2 to the pico in bootloader mode over usb cable

## License

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.
