#include "i2s.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"

#define DATA_BUFFER_SIZE = 8 * DMA_BUFFER_SIZE

void i2s_int(I2S *i2s, I2S_Config *config, void (*dmaHandler)(), int16_t output[2 * DMA_BUFFER_SIZE])
{
    // set all i2s values
    i2s->config = config;
    i2s->channel_a = dma_claim_unused_channel(true);
    i2s->channel_b = dma_claim_unused_channel(true);
    i2s->dmaHandler = dmaHandler;
    i2s->output_buffer = output;

    // Initialize DMA for I2S data transfer
    dma_init(i2s);
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

void i2s_start(I2S *i2s)
{
    // Configure I2S pins
    // gpio_set_function(i2s->config->data_pin, GPIO_FUNC_I2S);
    // gpio_set_function(i2s->config->lrck_pin, GPIO_FUNC_I2S);
    // gpio_set_function(i2s->config->bck_pin, GPIO_FUNC_I2S);

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
}
