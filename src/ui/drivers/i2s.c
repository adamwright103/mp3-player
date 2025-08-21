#include "i2s.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "i2s.pio.h"
#include <stdio.h>

#define DATA_BUFFER_SIZE = 8 * DMA_BUFFER_SIZE

void i2s_init(I2S *i2s, I2S_Config *config, void (*dmaHandler)(), int16_t output[2 * DMA_BUFFER_SIZE])
{
    // set all i2s values
    i2s->config = config;
    i2s->channel_a = dma_claim_unused_channel(true);
    i2s->channel_b = dma_claim_unused_channel(true);
    i2s->dmaHandler = dmaHandler;
    i2s->output_buffer = output;

    // Initialize DMA for I2S data transfer
    dma_init(i2s);

    pio_init(i2s);
    i2s->initialized = true;
}

void dma_init(I2S *i2s)
{
    i2s->channel_a = dma_claim_unused_channel(true);
    i2s->channel_b = dma_claim_unused_channel(true);

    // Configure DMA for I2S data transfer
    dma_channel_config ca = dma_channel_get_default_config(i2s->channel_a);
    channel_config_set_transfer_data_size(&ca, DMA_SIZE_16);
    channel_config_set_read_increment(&ca, true);
    channel_config_set_write_increment(&ca, true);

    dma_channel_configure(
        i2s->channel_a,
        &ca,
        i2s->buffer_a,   // write address
        i2s->buffer_b,   // read address
        DMA_BUFFER_SIZE, // transfer count (16 bits per transfer)
        false            // don't start yet
    );

    dma_channel_config cb = dma_channel_get_default_config(i2s->channel_b);
    channel_config_set_transfer_data_size(&cb, DMA_SIZE_16);
    channel_config_set_read_increment(&cb, true);
    channel_config_set_write_increment(&cb, true);

    dma_channel_configure(
        i2s->channel_b,
        &cb,
        i2s->buffer_b,   // write address
        i2s->buffer_a,   // read address
        DMA_BUFFER_SIZE, // transfer count (16 bits per transfer)
        false            // don't start yet
    );

    dma_channel_set_irq1_enabled(i2s->channel_a, true);
    dma_channel_set_irq1_enabled(i2s->channel_b, true);
    irq_set_exclusive_handler(DMA_IRQ_1, i2s->dmaHandler);
    irq_set_enabled(DMA_IRQ_1, true);
}

void pio_init(I2S *i2s)
{
    // =============================
    //           BCK INIT
    // =============================
    if (!pio_claim_free_sm_and_add_program_for_gpio_range(
            &bck_program,
            &i2s->pio,
            &i2s->prgm[PRGM_BCK].sm,
            &i2s->prgm[PRGM_BCK].offset,
            i2s->config->bck_pin,
            1,
            true))
    {
        printf("Failed to claim PIO state machine for BCK\n");
        return;
    }
    pio_gpio_init(i2s->pio, i2s->config->bck_pin);
    pio_sm_set_consecutive_pindirs(i2s->pio, i2s->prgm[PRGM_BCK].sm, i2s->config->bck_pin, 1, true);
    pio_sm_config c_bck = bck_program_get_default_config(i2s->prgm[PRGM_BCK].offset);
    sm_config_set_set_pins(&c_bck, i2s->config->bck_pin, 1);
    pio_sm_init(i2s->pio, i2s->prgm[PRGM_BCK].sm, i2s->prgm[PRGM_BCK].offset, &c_bck);

    // =============================
    //          LRCLK INIT
    // =============================
    if (!pio_claim_free_sm_and_add_program_for_gpio_range(
            &lrclk_program,
            &i2s->pio,
            &i2s->prgm[PRGM_LRCLK].sm,
            &i2s->prgm[PRGM_LRCLK].offset,
            i2s->config->lrclk_pin,
            1,
            true))
    {
        printf("Failed to claim PIO state machine for LRCLK\n");
        return;
    }
    pio_gpio_init(i2s->pio, i2s->config->lrclk_pin);
    pio_sm_set_consecutive_pindirs(i2s->pio, i2s->prgm[PRGM_LRCLK].sm, i2s->config->lrclk_pin, 1, true);
    pio_sm_config c_lrclk = lrclk_program_get_default_config(i2s->prgm[PRGM_LRCLK].offset);
    sm_config_set_set_pins(&c_lrclk, i2s->config->lrclk_pin, 1);
    pio_sm_init(i2s->pio, i2s->prgm[PRGM_LRCLK].sm, i2s->prgm[PRGM_LRCLK].offset, &c_lrclk);

    // =============================
    //          DOUT INIT
    // =============================
    if (!pio_claim_free_sm_and_add_program_for_gpio_range(
            &dout_program,
            &i2s->pio,
            &i2s->prgm[PRGM_DOUT].sm,
            &i2s->prgm[PRGM_DOUT].offset,
            i2s->config->data_pin,
            1,
            true))
    {
        printf("Failed to claim PIO state machine for DOUT\n");
        return;
    }
    pio_gpio_init(i2s->pio, i2s->config->data_pin);
    pio_sm_set_consecutive_pindirs(i2s->pio, i2s->prgm[PRGM_DOUT].sm, i2s->config->data_pin, 1, true);
    pio_sm_config c_dout = dout_program_get_default_config(i2s->prgm[PRGM_DOUT].offset);
    sm_config_set_out_pins(&c_dout, i2s->config->data_pin, 1); // data out pin
    sm_config_set_fifo_join(&c_dout, PIO_FIFO_JOIN_TX);        // join FIFOs for streaming samples
    sm_config_set_out_shift(&c_dout, false, false, 32);        // get msb first

    pio_sm_init(i2s->pio, i2s->prgm[PRGM_DOUT].sm, i2s->prgm[PRGM_DOUT].offset, &c_dout);
}

void i2s_start(I2S *i2s)
{
    if (!i2s->initialized)
    {
        printf("I2S not initialized. You must call i2s_init() first.\n");
        return;
    }

    pio_sm_set_enabled(i2s->pio, i2s->prgm[PRGM_BCK].sm, true);
    pio_sm_set_enabled(i2s->pio, i2s->prgm[PRGM_LRCLK].sm, true);
    pio_sm_set_enabled(i2s->pio, i2s->prgm[PRGM_DOUT].sm, true);
    uint freq_hz = 4;
    i2s->pio->txf[i2s->prgm[PRGM_BCK].sm] = (CPU_CLOCK_SPEED_KHZ * 500 / (freq_hz)) - 3;

    printf("blinking at %d Hz\n", freq_hz);

    // Start DMA channel A
    dma_hw->ch[i2s->channel_a].ctrl_trig |= DMA_CH0_CTRL_TRIG_EN_BITS;
}

void i2s_stop(I2S *i2s)
{
    // Stop DMA channels and claer IRQs
    dma_channel_abort(i2s->channel_a);
    dma_channel_abort(i2s->channel_b);
    dma_channel_set_irq1_enabled(i2s->channel_a, false);
    dma_channel_set_irq1_enabled(i2s->channel_b, false);
    dma_hw->ints1 = (1u << i2s->channel_a) | (1u << i2s->channel_b);

    // stop PIO
    pio_sm_set_enabled(i2s->pio, i2s->prgm[PRGM_BCK].sm, false);
    pio_sm_set_enabled(i2s->pio, i2s->prgm[PRGM_LRCLK].sm, false);
    pio_sm_set_enabled(i2s->pio, i2s->prgm[PRGM_DOUT].sm, false);
}
