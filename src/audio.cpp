#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "audio.h"
#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"
#include "hw_config.h"

// Audio state
static audio_state_t audio_state = {0};
static struct audio_buffer_pool *audio_pool = NULL;
static const audio_format_t *audio_format = NULL;

// I2S configuration
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024

bool audio_init(void)
{
  printf("Initializing I2S audio...\n");

  // Define audio format: 44.1kHz, 16-bit, mono
  static const struct audio_format format = {
      .sample_freq = SAMPLE_RATE,
      .format = AUDIO_BUFFER_FORMAT_PCM_S16,
      .channel_count = 1,
  };

  // Define audio buffer format
  static const struct audio_buffer_format buffer_format = {
      .format = &format,
      .sample_stride = 2, // 16-bit = 2 bytes
  };

  // Initialize audio buffer pool
  audio_pool = audio_new_producer_pool((audio_buffer_format_t *)&buffer_format, 3, BUFFER_SIZE);
  if (!audio_pool)
  {
    printf("Failed to create audio buffer pool\n");
    return false;
  }

  // Initialize I2S
  static const struct audio_i2s_config config = {
      .data_pin = I2S_DIN_PIN,
      .clock_pin_base = I2S_BCK_PIN,
      .dma_channel = 0,
      .pio_sm = 0,
  };

  audio_format = audio_i2s_setup(&format, &config);
  if (!audio_format)
  {
    printf("Failed to setup I2S audio\n");
    return false;
  }

  // Connect the producer pool to I2S
  if (!audio_i2s_connect(audio_pool))
  {
    printf("Failed to connect audio to I2S\n");
    return false;
  }

  printf("I2S audio initialized successfully\n");
  return true;
}

bool audio_play_wav(const char *file_path)
{
  if (!audio_pool)
  {
    printf("Audio not initialized\n");
    return false;
  }

  printf("Playing WAV file: %s\n", file_path);

  // Mount SD card
  sd_card_t *pSD = sd_get_by_num(0);
  printf("0");
  FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
  printf("1.1");
  if (FR_OK != fr)
  {
    printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  printf("1");

  // Open WAV file
  FIL fil;
  fr = f_open(&fil, file_path, FA_READ);
  if (FR_OK != fr)
  {
    printf("f_open(%s) error: %s (%d)\n", file_path, FRESULT_str(fr), fr);
    f_unmount(pSD->pcName);
    return false;
  }
  printf("2");

  // Read WAV header
  wav_header_t header;
  UINT bytes_read;
  fr = f_read(&fil, &header, sizeof(wav_header_t), &bytes_read);
  if (fr != FR_OK || bytes_read != sizeof(wav_header_t))
  {
    printf("Failed to read WAV header\n");
    f_close(&fil);
    f_unmount(pSD->pcName);
    return false;
  }
  printf("3");

  // Validate WAV header
  if (memcmp(header.riff, "RIFF", 4) != 0 ||
      memcmp(header.wave, "WAVE", 4) != 0 ||
      memcmp(header.fmt, "fmt ", 4) != 0 ||
      memcmp(header.data, "data", 4) != 0)
  {
    printf("Invalid WAV file format\n");
    f_close(&fil);
    f_unmount(pSD->pcName);
    return false;
  }
  printf("4");
  printf("WAV Info: %d channels, %d Hz, %d bits\n",
         header.num_channels, header.sample_rate, header.bits_per_sample);

  // Check if we support this format
  if (header.audio_format != 1 || header.bits_per_sample != 16)
  {
    printf("Unsupported WAV format (only 16-bit PCM supported)\n");
    f_close(&fil);
    f_unmount(pSD->pcName);
    return false;
  }
  printf("5");
  // Start audio playback
  audio_state.playing = true;

  // Read and play audio data
  int16_t *sample_buffer = (int16_t *)malloc(BUFFER_SIZE * sizeof(int16_t));
  if (!sample_buffer)
  {
    printf("Failed to allocate sample buffer\n");
    f_close(&fil);
    f_unmount(pSD->pcName);
    return false;
  }
  printf("6");
  size_t total_samples = header.data_size / (header.bits_per_sample / 8);
  size_t samples_played = 0;
  printf("7");
  while (samples_played < total_samples && audio_state.playing)
  {
    // Get audio buffer from pool
    struct audio_buffer *buffer = take_audio_buffer(audio_pool, true);
    if (!buffer)
    {
      printf("Failed to get audio buffer\n");
      break;
    }
    printf("8");
    // Calculate how many samples to read
    size_t samples_to_read = BUFFER_SIZE;
    if (samples_played + samples_to_read > total_samples)
    {
      samples_to_read = total_samples - samples_played;
    }
    printf("9");
    // Read samples from file
    UINT bytes_to_read = samples_to_read * sizeof(int16_t);
    fr = f_read(&fil, sample_buffer, bytes_to_read, &bytes_read);
    if (fr != FR_OK)
    {
      printf("Failed to read audio data\n");
      break;
    }
    printf("10");
    // Convert stereo to mono if needed (simple averaging)
    if (header.num_channels == 2)
    {
      for (size_t i = 0; i < samples_to_read; i++)
      {
        int16_t left = sample_buffer[i * 2];
        int16_t right = sample_buffer[i * 2 + 1];
        sample_buffer[i] = (left + right) / 2;
      }
    }
    printf("11");
    // Copy samples to audio buffer
    memcpy(buffer->buffer->bytes, sample_buffer, bytes_read);
    buffer->sample_count = bytes_read / sizeof(int16_t);

    // Give buffer to audio system
    give_audio_buffer(audio_pool, buffer);
    printf("12");
    samples_played += samples_to_read / header.num_channels;
    audio_state.position = samples_played;
    printf("13");
    // Small delay to prevent overwhelming the audio system
    sleep_ms(10);
  }
  printf("14");
  // Cleanup
  free(sample_buffer);
  f_close(&fil);
  f_unmount(pSD->pcName);

  audio_state.playing = false;
  printf("Audio playback finished\n");

  return true;
}

void audio_stop(void)
{
  audio_state.playing = false;
  printf("Audio playback stopped\n");
}

bool audio_is_playing(void)
{
  return audio_state.playing;
}

audio_state_t *audio_get_state(void)
{
  return &audio_state;
}