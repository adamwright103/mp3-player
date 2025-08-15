#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "src/constants.h"
#include "src/app.h"
#include "src/ui/ui.h"
#include <string.h>
#include <map>
#include <stdio.h>
#include <math.h>

#define CPU_CLOCK_SPEED_KHZ 200000 // set to default 200 MHz for now
#define DMA_BUFFER_SIZE 64
#define DATA_BUFFER_SIZE 8 * DMA_BUFFER_SIZE

std::map<uint, volatile bool> buttons = {
    {LEFT_BTN_PIN, true},
    {HOME_BTN_PIN, true},
    {RIGHT_BTN_PIN, true}};

uint16_t data_table[DATA_BUFFER_SIZE];
uint16_t dma_buffer_a[DMA_BUFFER_SIZE];
uint16_t dma_buffer_b[DMA_BUFFER_SIZE];
uint16_t output_buffer[2 * DMA_BUFFER_SIZE] = {0};

int channel_a;
int channel_b;
uint16_t idx_a = 0;
uint16_t idx_b = DMA_BUFFER_SIZE;

App *app = nullptr;

void __isr button_callback(uint gpio, uint32_t events)
{
    if (!buttons[gpio] || !app)
        return;

    app->onButtonPress(gpio);

    buttons[gpio] = false;
    uint *gpio_ptr = new uint(gpio);

    add_alarm_in_ms(DEBOUNCE_TIME_MS, [](alarm_id_t, void *user_data) -> int64_t
                    {
                        uint gpio = *reinterpret_cast<uint *>(user_data);
                        buttons[gpio] = true;
                        delete reinterpret_cast<uint *>(user_data);
                        return 0; }, gpio_ptr, false);
}

void __isr dma_handler()
{
    uint32_t ints = dma_hw->ints1; // Read the interrupt status for channel 1
    dma_hw->ints1 = (1u << channel_a) | (1u << channel_b);

    if (ints & (1u << channel_a))
    {
        // Channel A completed so refilling
        idx_a = (idx_a + DMA_BUFFER_SIZE) % (DATA_BUFFER_SIZE / 2);
        memcpy(dma_buffer_a, &data_table[idx_a], DMA_BUFFER_SIZE * 2);

        // reset channel A r/w addr
        dma_hw->ch[channel_a].read_addr = (uintptr_t)dma_buffer_a;
        dma_hw->ch[channel_a].write_addr = (uintptr_t)output_buffer;

        // start channel B
        dma_hw->ch[channel_b].ctrl_trig |= DMA_CH0_CTRL_TRIG_EN_BITS;
    }
    if (ints & (1u << channel_b))
    {
        // Channel B completed so refilling
        idx_b = ((idx_b + DMA_BUFFER_SIZE) % (DATA_BUFFER_SIZE / 2)) + (DATA_BUFFER_SIZE / 2);
        memcpy(dma_buffer_b, &data_table[idx_b], DMA_BUFFER_SIZE * 2);

        // reset channel B r/w addr
        dma_hw->ch[channel_b].read_addr = (uintptr_t)dma_buffer_b;
        dma_hw->ch[channel_b].write_addr = (uintptr_t)(output_buffer + DMA_BUFFER_SIZE);

        // start channel A
        dma_hw->ch[channel_a].ctrl_trig |= DMA_CH0_CTRL_TRIG_EN_BITS;
    }
}

int main()
{
    set_sys_clock_khz(CPU_CLOCK_SPEED_KHZ, true);
    stdio_init_all();

    while (true)
    {
        printf("press s to start\n");
        char c = getchar_timeout_us(1000 * 1000);
        if (c == 's' || c == 'S')
        {
            break;
        }
    }

    app = new App();

    const uint buttonPins[] = {LEFT_BTN_PIN, HOME_BTN_PIN, RIGHT_BTN_PIN};

    for (uint8_t pin : buttonPins)
    {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
        gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE, true, &button_callback);
    }

    for (uint16_t i = 0; i < DATA_BUFFER_SIZE / 2; i++)
    {
        data_table[i] = i;
    }

    for (uint16_t i = DATA_BUFFER_SIZE / 2; i < DATA_BUFFER_SIZE; i++)
    {
        data_table[i] = i % 3;
    }

    memcpy(dma_buffer_a, &data_table[0], DMA_BUFFER_SIZE * 2);
    memcpy(dma_buffer_b, &data_table[DMA_BUFFER_SIZE], DMA_BUFFER_SIZE * 2);

    channel_a = dma_claim_unused_channel(true);
    channel_b = dma_claim_unused_channel(true);

    // we setup a to read from the sin_table and write to the output_buffer
    // on completion it will chain to channel_b which will read from output_buffer
    dma_channel_config ca = dma_channel_get_default_config(channel_a);
    channel_config_set_transfer_data_size(&ca, DMA_SIZE_16);
    channel_config_set_read_increment(&ca, true);
    channel_config_set_write_increment(&ca, true);

    dma_channel_configure(
        channel_a,
        &ca,
        output_buffer,   // write address
        dma_buffer_a,    // read address
        DMA_BUFFER_SIZE, // transfer count (16 bits per transfer)
        false            // don't start yet
    );

    dma_channel_config cb = dma_channel_get_default_config(channel_b);
    channel_config_set_transfer_data_size(&cb, DMA_SIZE_16);
    channel_config_set_read_increment(&cb, true);
    channel_config_set_write_increment(&cb, true);

    dma_channel_configure(
        channel_b,
        &cb,
        &output_buffer[DMA_BUFFER_SIZE], // write address
        dma_buffer_b,                    // read address
        DMA_BUFFER_SIZE,                 // transfer count (16 bits per transfer)
        false                            // don't start yet
    );

    // irq setup, we enable an irq on line 1 on completion
    irq_set_exclusive_handler(DMA_IRQ_1, dma_handler);
    irq_set_enabled(DMA_IRQ_1, true);
    dma_hw->inte1 |= (1u << channel_a) | (1u << channel_b);
    dma_hw->ch[channel_a].ctrl_trig |= DMA_CH0_CTRL_TRIG_EN_BITS;

    printf("starting Ping-Pong DMA, press q to stop...\n");

    while (true)
    {
        char c = getchar_timeout_us(1000 * 1000);
        if ((c == 'q' || c == 'Q')) // **Stop condition**
        {
            break;
        }
    }

    printf("Stopping DMA\n");

    // stops dma ping pong and clears IRQs
    dma_channel_abort(channel_a);
    dma_channel_abort(channel_b);
    dma_channel_set_irq1_enabled(channel_a, false);
    dma_channel_set_irq1_enabled(channel_b, false);
    dma_hw->ints1 = (1u << channel_a) | (1u << channel_b);

    while (true)
    {
        tight_loop_contents();
    }
}
