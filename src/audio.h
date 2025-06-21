// filepath: /home/adam/Documents/Projects/mp3-player/audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/audio_i2s.h"
#include <stdint.h>
#include <stdbool.h>

// Define GPIO pins for I2S
#define I2S_BCK_PIN 10
#define I2S_LCK_PIN 11
#define I2S_DIN_PIN 12

// WAV file header structure
typedef struct
{
  char riff[4];             // "RIFF"
  uint32_t file_size;       // File size - 8
  char wave[4];             // "WAVE"
  char fmt[4];              // "fmt "
  uint32_t fmt_size;        // Format chunk size (16 for PCM)
  uint16_t audio_format;    // Audio format (1 for PCM)
  uint16_t num_channels;    // Number of channels
  uint32_t sample_rate;     // Sample rate
  uint32_t byte_rate;       // Byte rate
  uint16_t block_align;     // Block align
  uint16_t bits_per_sample; // Bits per sample
  char data[4];             // "data"
  uint32_t data_size;       // Data chunk size
} wav_header_t;

// Audio state
typedef struct
{
  bool playing;
  size_t position;
} audio_state_t;

// Initialize I2S audio system
bool audio_init(void);

// Play a WAV file using I2S audio output
bool audio_play_wav(const char *file_path);

// Stop audio playback
void audio_stop(void);

// Check if audio is currently playing
bool audio_is_playing(void);

// Get current audio state
audio_state_t *audio_get_state(void);

#endif