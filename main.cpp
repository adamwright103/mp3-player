#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "src/constants.h"
#include "src/app.h"
#include "src/ui/ui.h"
#include "src/ui/drivers/i2s.h"
#include <string.h>
#include <map>
#include <stdio.h>
#include <math.h>

#define DATA_BUFFER_SIZE 8 * DMA_BUFFER_SIZE

std::map<uint, volatile bool> buttons = {
    {LEFT_BTN_PIN, true},
    {HOME_BTN_PIN, true},
    {RIGHT_BTN_PIN, true}};

int16_t data_table[DATA_BUFFER_SIZE];
int16_t output_buffer[2 * DMA_BUFFER_SIZE] = {0};
uint16_t idx_a = 0;
uint16_t idx_b = DMA_BUFFER_SIZE;

I2S_Config i2sConfig = {
    .sample_rate = 44100,       // Default sample rate
    .bits_per_sample = 16,      // Only 16 bits per sample supported
    .channels = 1,              // mono audio
    .data_pin = I2S_DATA_PIN,   // data pin
    .lrclk_pin = I2S_LRCLK_PIN, // Left/Right clock pin
    .bck_pin = I2S_BCK_PIN      // Bit clock pin
};
I2S i2s;

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

void __isr i2s_dma_handler()
{
    uint32_t ints = dma_hw->ints1; // Read the interrupt status for channel 1
    dma_hw->ints1 = (1u << i2s.channel_a) | (1u << i2s.channel_b);

    if (ints & (1u << i2s.channel_a))
    {
        // Channel A completed so refilling
        idx_a = (idx_a + DMA_BUFFER_SIZE) % (DATA_BUFFER_SIZE / 2);
        memcpy(i2s.buffer_a, &data_table[idx_a], DMA_BUFFER_SIZE * 2);

        // reset channel A r/w addr
        dma_hw->ch[i2s.channel_a].read_addr = (uintptr_t)i2s.buffer_a;
        dma_hw->ch[i2s.channel_a].write_addr = (uintptr_t)output_buffer;

        // start channel B
        dma_hw->ch[i2s.channel_b].ctrl_trig |= DMA_CH0_CTRL_TRIG_EN_BITS;
    }
    else if (ints & (1u << i2s.channel_b))
    {
        // Channel B completed so refilling
        idx_b = ((idx_b + DMA_BUFFER_SIZE) % (DATA_BUFFER_SIZE / 2)) + (DATA_BUFFER_SIZE / 2);
        memcpy(i2s.buffer_b, &data_table[idx_b], DMA_BUFFER_SIZE * 2);

        // reset channel B r/w addr
        dma_hw->ch[i2s.channel_b].read_addr = (uintptr_t)i2s.buffer_b;
        dma_hw->ch[i2s.channel_b].write_addr = (uintptr_t)(output_buffer + DMA_BUFFER_SIZE);

        // start channel A
        dma_hw->ch[i2s.channel_a].ctrl_trig |= DMA_CH0_CTRL_TRIG_EN_BITS;
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

    i2s_init(&i2s, &i2sConfig, i2s_dma_handler, output_buffer);
    i2s_start(&i2s);

    printf("starting Ping-Pong DMA, press q to stop...\n");

    while (true)
    {
        char c = getchar_timeout_us(1000 * 1000);
        if ((c == 'q' || c == 'Q')) // **Stop condition**
        {
            break;
        }
        else if (c >= '0' && c <= '9')
        {
            i2s.pio->txf[i2s.prgm[PRGM_DOUT].sm] = 0x81010101;
        }
    }

    printf("Stopping DMA\n");

    i2s_stop(&i2s);

    printf("Output buffer contents:\n");
    for (size_t i = 0; i < 2 * DMA_BUFFER_SIZE; ++i)
    {
        printf("%d ", output_buffer[i]);
        if ((i + 1) % 16 == 0)
            printf("\n"); // Print 16 values per line for readability
    }

    while (true)
    {
        tight_loop_contents();
    }
}
