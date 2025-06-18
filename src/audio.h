// filepath: /home/adam/Documents/Projects/mp3-player/audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "hardware/pio.h"
#include <stdint.h>

// Define GPIO pins for I2S
#define I2S_BCK_PIN 10
#define I2S_LCK_PIN 11
#define I2S_DIN_PIN 12

// Plays a WAV file using I2S audio output
bool audio_play_wav(const char *file_path);

#endif