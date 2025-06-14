#include <stdio.h>
#include "pico/stdlib.h"
#include "audio_i2s.pio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "audio.h"

extern const uint8_t binary_Balance_wav[];
extern const uint32_t binary_Balance_wav_len;

// initialises I2S audio output
void i2s_audio_init(PIO pio, uint sm)
{
  // Load the I2S program into the PIO
  uint offset = pio_add_program(pio, &audio_i2s_program);

  // Configure the state machine
  audio_i2s_program_init(pio, sm, offset, I2S_DIN_PIN, I2S_BCK_PIN);
  pio_sm_set_enabled(pio, sm, true);

  // Set the clock divider for the PIO state machine
  float clk_div = 125000000 / (44100 * 16 * 2); // sys clk freq / (sample rate * bits per sample * channels)
  pio_sm_set_clkdiv(pio, sm, clk_div);
}

// send audio data
void i2s_audio_send(PIO pio, uint sm, uint32_t audio_sample)
{
  // Push the audio sample to the PIO FIFO
  pio_sm_put_blocking(pio, sm, audio_sample);
}

void play_wav(const char *file_path, PIO pio, uint sm)
{
  FILE *wav_file = fopen(file_path, "rb");
  if (!wav_file)
  {
    for (int i = 0; i < 30; i++)
    {
      printf("Failed to open WAV file: %s\n", file_path);
      sleep_ms(1000);
    }
    return;
  }

  // Skip WAV header (typically 44 bytes)
  fseek(wav_file, 44, SEEK_SET);

  int16_t sample;
  while (fread(&sample, sizeof(int16_t), 1, wav_file) == 1)
  {
    // Convert 16-bit signed integer to 32-bit format for I2S
    uint32_t audio_sample = (uint32_t)(sample & 0xFFFF);

    // Send the sample to the I2S interface
    i2s_audio_send(pio, sm, audio_sample);

    // Delay to match the sample rate (44100 Hz)
    sleep_us(23); // 1 second / 44100 samples = ~23 microseconds per sample
  }

  fclose(wav_file);
}