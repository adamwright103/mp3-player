#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "src/oled.h"
#include "src/audio.h"
#include "src/sdcard.h"
#include <string.h>

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

    // Wait for Enter key from USB serial connection
    while (getchar_timeout_us(0) != 's')
    {
        printf(".");
        sleep_ms(500);
    }

    oled_init();  // Initialize the OLED display
    oled_clear(); // Clear the display buffer

    printf("starting\n");

    // Initialize audio system
    // if (!audio_init())
    // {
    //     printf("Failed to initialize audio system\n");
    //     return -1;
    // }

    char playlistFile[256];
    if (!getFirstPlaylistFile("playlists/Bangers.txt", playlistFile, sizeof(playlistFile)))
    {
        printf("Failed to retrieve playlist file\n");
        return -1;
    }

    printf("Playing file: %s\n", playlistFile);

    // // Play the WAV file
    // if (!audio_play_wav(playlistFile))
    // {
    //     printf("Failed to play audio file\n");
    //     return -1;
    // }

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

    char *song_name = strtok(playlistFile, "---");
    char *artist_name = strtok(NULL, "---");

    if (artist_name)
    {
        char *dot_wav = strstr(artist_name, ".wav");
        if (dot_wav)
        {
            *dot_wav = '\0';
        }
    }

    while (true)
    {
        oled_battery(battery_level);

        // Parse song and artist from playlistFile

        oled_print_song(song_name ? song_name : playlistFile, offset);
        oled_print_artist(artist_name ? artist_name : "", offset);
        oled_print_visualiser(sin_data[offset % 7]);
        offset++;

        sleep_ms(50);
    }
}
