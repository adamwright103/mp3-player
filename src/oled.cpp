#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oled.h"
#include "fonts/small.h"
#include "fonts/medium.h"

using namespace std;

#define I2C_PORT i2c1
#define OLED_SDA_PIN 18
#define OLED_SCL_PIN 19
#define OLED_ADR 0x3C
#define OLED_CLK_FREQ 400 * 1000
#define WIDTH 128
#define HEIGHT 64

// Display buffer: 8 pages of 128 bytes
uint8_t buffer[WIDTH * HEIGHT / 8];

// sends command to OLED display
void oled_cmd(uint8_t cmd)
{
  uint8_t data[2] = {0x00, cmd}; // 0x00 indicates command mode
  i2c_write_blocking(I2C_PORT, OLED_ADR, data, sizeof(data), false);
}

void oled_init()
{
  i2c_init(I2C_PORT, OLED_CLK_FREQ);

  gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);

  gpio_pull_up(OLED_SDA_PIN);
  gpio_pull_up(OLED_SCL_PIN);

  // from datasheet
  // https://www.electronicwings.com/components/ssd1306-oled-display/1/datasheet
  static const uint8_t init_cmds[] = {
      0xAE, 0xD5, 0x80, 0xA8, 0x3F,
      0xD3, 0x00, 0x40, 0x8D, 0x14,
      0x20, 0x00, 0xA1, 0xC8, 0xDA,
      0x12, 0x81, 0xCF, 0xD9, 0xF1,
      0xDB, 0x40, 0xA4, 0xA6, 0xAF};

  for (uint8_t cmd : init_cmds)
  {
    oled_cmd(cmd);
  }
}

// Send a data in 16 byte chunks to the OLED display.
void oled_data(uint8_t *data, size_t len)
{
  uint8_t buf[17];
  buf[0] = 0x40; // Control byte for data
  for (size_t i = 0; i < len; i += 16)
  {
    size_t chunk = (len - i > 16) ? 16 : len - i;
    memcpy(&buf[1], &data[i], chunk);
    i2c_write_blocking(I2C_PORT, OLED_ADR, buf, chunk + 1, false);
  }
}

// Push whole buffer to OLED
void oled_display()
{
  for (uint8_t page = 0; page < 8; page++)
  {
    oled_cmd(0xB0 + page);
    oled_cmd(0x00);
    oled_cmd(0x10);
    oled_data(&buffer[WIDTH * page], WIDTH);
  }
}

// Push one page (8 rows) to OLED
void oled_display(OledPage page)
{
  oled_cmd(0xB0 + page);
  oled_cmd(0x00);
  oled_cmd(0x10);
  oled_data(&buffer[WIDTH * page], WIDTH);
}

// sets all bits in buffer to 0
void oled_clear()
{
  memset(buffer, 0, sizeof(buffer));
}

// sets all bits in buffer to 0 for a specific page
void oled_clear(OledPage page)
{
  if (page < 0 || page >= 8)
    return; // Invalid page

  // Clear the specified page in the buffer
  memset(&buffer[WIDTH * page], 0, WIDTH);
}

void oled_battery(uint8_t percent)
{
  int filled_pixels = (percent * 15) / 100; // battery display is 20px wide

  // left and right sides of battery outline
  buffer[WIDTH - 1] = 0x3c;
  buffer[WIDTH - 2] = 0x3C;
  buffer[WIDTH - 3] = 0xFF;
  buffer[WIDTH - 4] = 0x81;
  buffer[WIDTH - 21] = 0xFF;
  buffer[WIDTH - 20] = 0x81;

  for (uint8_t i = 0; i < 15; i++)
  {
    // either draw a full column or just the outline
    buffer[WIDTH - 19 + i] = i < filled_pixels
                                 ? 0xBD
                                 : 0x81;
  }

  oled_display(PAGE_0);
}

// prints artists name centered on page 7
void oled_print_artist(const char *str)
{
  // if a string is too long, we trucate with an elipse overiding ascii 127 (del)
  const int max_chars = WIDTH / FONT_WIDTH_S;
  char truncated_str[max_chars + 1];

  if (strlen(str) > max_chars)
  {
    strncpy(truncated_str, str, max_chars - 1);
    truncated_str[max_chars - 1] = '\x7F';
    truncated_str[max_chars] = '\0';
    str = truncated_str;
  }

  int start_col = (WIDTH - strlen(str) * FONT_WIDTH_S) / 2; // Center the text

  oled_clear(PAGE_7);

  const char *ptr = str;
  while (*ptr)
  {
    const uint8_t *char_bitmap = getCharBitmapS(*ptr);
    if (char_bitmap)
    {
      for (int col = 0; col < FONT_WIDTH_S; col++)
      {
        buffer[WIDTH * PAGE_7 + start_col + (ptr - str) * FONT_WIDTH_S + col] = char_bitmap[col];
      }
    }
    ptr++;
  }

  oled_display(PAGE_7);
}

// prints song name centered on pages 5 and 6
void oled_print_song(const char *str)
{
  // if a string is too long, we trucate with an elipse overiding ascii 127 (del)
  const int max_chars = WIDTH / FONT_WIDTH_M;
  char truncated_str[max_chars + 1];

  if (strlen(str) > max_chars)
  {
    strncpy(truncated_str, str, max_chars - 1);
    truncated_str[max_chars - 1] = '\x7F';
    truncated_str[max_chars] = '\0';
    str = truncated_str;
  }

  int start_col = (WIDTH - strlen(str) * FONT_WIDTH_M) / 2; // Center the text

  oled_clear(PAGE_5);
  oled_clear(PAGE_6);

  const char *ptr = str;
  while (*ptr)
  {
    const uint16_t *char_bitmap = getCharBitmapM(*ptr);
    if (char_bitmap)
    {
      for (int col = 0; col < FONT_WIDTH_M; col++)
      {
        // writing top half of the character to page 5
        buffer[WIDTH * PAGE_5 + start_col + (ptr - str) * FONT_WIDTH_M + col] = char_bitmap[col];

        // writing the bottom half of the character to page 6
        buffer[WIDTH * PAGE_6 + start_col + (ptr - str) * FONT_WIDTH_M + col] = char_bitmap[col] >> 8;
      }
    }
    ptr++;
  }

  oled_display(PAGE_5);
  oled_display(PAGE_6);
}

// prints 7 vertical bars on pages 1 2 3 4 with heights based on data
void oled_print_visualiser(uint8_t data[7])
{
  for (uint8_t page = PAGE_1; page <= PAGE_4; page++)
  {
    oled_clear((OledPage)page);
  }

  for (uint8_t i = 0; i < 7; i++)
  {
    // height is in range 4-30
    uint8_t height = data[i] * 30 / 100;
    height = height < 4 ? 4 : height;

    uint8_t start_col = 4 + i * (14 + 4); // 14px wide bar + 4px gap

    for (uint8_t col = 0; col < 14; col++) // Draw 14px wide bar
    {
      for (uint8_t row = 0; row < height; row++)
      {
        uint8_t from_bottom = 31 - row; // 32 pixels total, so index 0 = bottom
        uint8_t page = from_bottom / 8;
        uint8_t bit = 1 << (from_bottom % 8);
        buffer[WIDTH * (PAGE_1 + page) + start_col + col] |= bit;
      }
    }
  }

  // Update pages 1 to 4
  oled_display(PAGE_1);
  oled_display(PAGE_2);
  oled_display(PAGE_3);
  oled_display(PAGE_4);
}
