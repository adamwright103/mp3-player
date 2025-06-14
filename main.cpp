#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "src/oled.h"
#include "src/audio.h"

#define BTN_PIN 7

volatile int battery_level = 100;

void button_callback(uint gpio, uint32_t events)
{
    if (gpio != BTN_PIN)
        return;

    battery_level -= 10;
    if (battery_level < 0)
    {
        battery_level = 100;
    }
}

int main()
{
    stdio_init_all();

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    sleep_ms(500);

    oled_init();  // Initialize the OLED display
    oled_clear(); // Clear the display buffer

    // PIO pio = pio0; // Use PIO0
    // uint sm = 0;    // Use state machine 0

    // // Initialize I2S
    // i2s_audio_init(pio, sm);

    // // Play the WAV file
    // play_wav("Balance.wav", pio, sm);

    int i = 0;
    uint8_t sin_data[7][7]{
        {100, 81, 39, 5, 5, 39, 81},
        {81, 100, 81, 39, 5, 5, 39},
        {39, 81, 100, 81, 39, 5, 5},
        {5, 39, 81, 100, 81, 39, 5},
        {5, 5, 39, 81, 100, 81, 39},
        {39, 5, 5, 39, 81, 100, 81},
        {81, 39, 5, 5, 39, 81, 100}};

    while (true)
    {
        oled_battery(battery_level);
        oled_print_artist("Artist Name");
        oled_print_song("Song Title");
        oled_print_visualiser(sin_data[i]);
        i = (i + 1) % 7;

        sleep_ms(50);
    }
}
