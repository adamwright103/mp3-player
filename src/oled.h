#ifndef OLED_H
#define OLED_H

#include <stdint.h>

// Enum for OLED pages
enum OledPage : uint8_t
{
  PAGE_0 = 0,
  PAGE_1,
  PAGE_2,
  PAGE_3,
  PAGE_4,
  PAGE_5,
  PAGE_6,
  PAGE_7,
};

// Initializes I2C for OLED display
void oled_init();

// Sends a command to the OLED display
void oled_cmd(uint8_t cmd);

// Sends data to the OLED display in 16-byte chunks
void oled_data(uint8_t *data, size_t len);

// Pushes the entire buffer to the OLED display
void oled_display();

// Pushes a single page (8 rows) to the OLED display
void oled_display(OledPage page);

// Clears the display buffer
void oled_clear();

// Clears a specific page in the display buffer
void oled_clear(OledPage page);

// takes a battery % and updates the OLED display with a battery icon
void oled_battery(uint8_t percent);

// prints the artist name on the OLED display
void oled_print_artist(const char *str, uint16_t offset);

// prints the song name on the OLED display
void oled_print_song(const char *str, uint16_t offset);

// prints a visualiser on the OLED display
void oled_print_visualiser(uint8_t data[7]);

#endif
