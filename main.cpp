#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "src/oled.h"
#include "src/audio.h"
#include "src/sdcard.h"

#define BTN_PIN 7
#define DEBOUNCE_TIME_MS 150

volatile int battery_level = 100;
volatile bool button_enabled = true; // Flag to enable/disable button

void button_callback(uint gpio, uint32_t events)
{
    if (gpio != BTN_PIN || !button_enabled)
        return;

    button_enabled = false;

    battery_level -= 10;
    if (battery_level < 0)
    {
        battery_level = 100;
    }

    add_alarm_in_ms(DEBOUNCE_TIME_MS, [](alarm_id_t, void *) -> int64_t
                    {
        button_enabled = true;
        return 0; }, nullptr, false);
}

int main()
{
    stdio_init_all();

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    sleep_ms(15000);

    oled_init();  // Initialize the OLED display
    oled_clear(); // Clear the display buffer

    printf("starting\n");
    char playlistFile[256];
    if (!getFirstPlaylistFile("playlists/Bangers.txt", playlistFile, sizeof(playlistFile)))
    {
        printf("Failed to retrieve playlist file\n");
        return -1;
    }

    printf("Playing file: %s\n", playlistFile);

    int i = 0;
    uint8_t sin_data[7][7]{
        {100, 81, 39, 5, 5, 39, 81},
        {81, 100, 81, 39, 5, 5, 39},
        {39, 81, 100, 81, 39, 5, 5},
        {5, 39, 81, 100, 81, 39, 5},
        {5, 5, 39, 81, 100, 81, 39},
        {39, 5, 5, 39, 81, 100, 81},
        {81, 39, 5, 5, 39, 81, 100}};

    uint16_t offset = 0;

    sdTest();

    while (true)
    {
        oled_battery(battery_level);
        oled_print_song("WANT NEED LOVE", offset);
        oled_print_artist("Prospa, Dimension", offset);
        oled_print_visualiser(sin_data[offset % 7]);
        offset++;

        sleep_ms(50);
    }
}
