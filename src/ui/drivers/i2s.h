#ifndef I2S_H
#define I2S_H

#include "pico/stdlib.h"
#include "src/constants.h"
#include "hardware/pio.h"

#define DMA_BUFFER_SIZE 64

struct I2S_Program_t;
typedef enum {
    PRGM_BCK = 0,   // Bit clock
    PRGM_LRCLK,     // Left/Right clock

    PRGM_COUNT      // Must be last, array size
} I2S_ProgramType;
typedef struct I2S_Program_t
{
  uint offset; // Offset in PIO instruction memory
  uint sm;     // State machine number
} I2S_Program;

typedef struct I2S_Config_t
{
  int sample_rate;     // Sample rate in Hz
  int bits_per_sample; // Bits per sample only 16 supported
  int channels;        // Number of audio channels (e.g., 1 for mono, 2 for stereo)

  uint data_pin; // Data GPIO pin for I2S
  uint lrclk_pin; // Left/Right clock GPIO pin for I2S
  uint bck_pin;  // Bit clock pin for GPIO I2S
} I2S_Config;

typedef struct I2S_t
{
  I2S_Config *config;
  bool initialized; // Flag to check if I2S is initialized with i2s_init()

  int channel_a; // DMA channel for audio data transfer
  int channel_b; // DMA channel for audio data transfer

  int16_t buffer_a[DMA_BUFFER_SIZE]; // first buffer for audio data
  int16_t buffer_b[DMA_BUFFER_SIZE]; // second buffer for audio data

  int16_t *output_buffer; // TODO temp buffer to Write data to

  void (*dmaHandler)(void); // DMA handler function to handle double buffer

  PIO pio; // PIO instance for I2S
  I2S_Program prgm[PRGM_COUNT]; // PIO programs for I2S
} I2S;


#ifdef __cplusplus
extern "C"
{
#endif

  void i2s_init(I2S *i2s, I2S_Config *config, void (*dmaHandler)(), int16_t output[2 * DMA_BUFFER_SIZE]);
  void dma_init(I2S *i2s);
  void pio_init(I2S *i2s);
  void i2s_start(I2S *i2s);
  void i2s_stop(I2S *i2s);

#ifdef __cplusplus
}
#endif

#endif