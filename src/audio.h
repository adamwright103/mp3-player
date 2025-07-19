#ifndef AUDIO_H
#define AUDIO_H

#include "pico/audio_i2s.h"
#include <stdint.h>
#include <stdbool.h>
#include "pico/audio.h"
#include "pico/audio_i2s.h"

#define PICO_AUDIO_I2S_DATA_PIN 10
#define I2S_BCK_PIN 11
#define I2S_LRCK_PIN 12

// initilise i2s to prepare for audio
struct audio_buffer_pool *audio_init();

// play a wav file from the SD card
bool audio_play_wav(struct audio_buffer_pool *ap, const char *file_path);

#endif // AUDIO_H
