// filepath: /home/adam/Documents/Projects/mp3-player/audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "hardware/pio.h"
#include <stdint.h>

// Define GPIO pins for I2S
#define I2S_BCK_PIN 10
#define I2S_LCK_PIN 11
#define I2S_DIN_PIN 12

// Initializes I2S audio output
void i2s_audio_init(PIO pio, uint sm);

// Sends audio data to the I2S interface
void i2s_audio_send(PIO pio, uint sm, uint32_t audio_sample);

// Plays a WAV file using I2S audio output
void play_wav(const char *file_path, PIO pio, uint sm);

#endif