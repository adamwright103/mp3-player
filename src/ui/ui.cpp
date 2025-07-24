#include "ui.h"
#include "../constants.h"
#include "../app.h"
#include "../fonts/small.h"
#include "../fonts/medium.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <string>
#include <cstring>

using namespace std;

uint8_t Ui::buffer[OLED_BUFFER_SIZE] = {0};

Ui::Ui(Mode mode) : mode_(mode), charging_(false), charge_(100), offset_(0)
{
  memset(buffer, 0, sizeof(buffer));
  // every mode will show the battery, so we set up the outline now
  buffer[OLED_WIDTH - 1] = 0x3c;
  buffer[OLED_WIDTH - 2] = 0x3C;
  buffer[OLED_WIDTH - 3] = 0xFF;
  buffer[OLED_WIDTH - 4] = 0x81;
  buffer[OLED_WIDTH - 21] = 0xFF;
  buffer[OLED_WIDTH - 20] = 0x81;

  init();
  drawBattery();
}

void Ui::init() const
{
  i2c_init(OLED_I2C_PORT, OLED_CLK_FREQ);

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
    sendCmd(cmd);
  }
}

void Ui::sendCmd(uint8_t cmd) const
{
  uint8_t data[2] = {0x00, cmd}; // 0x00 indicates command mode
  i2c_write_blocking(OLED_I2C_PORT, OLED_ADR, data, sizeof(data), false);
}

void Ui::sendData(uint8_t *data, uint len) const
{
  uint8_t buf[17];
  buf[0] = 0x40; // Control byte for data
  for (uint i = 0; i < len; i += 16)
  {
    uint8_t chunk = (len - i > 16) ? 16 : len - i;
    memcpy(&buf[1], &data[i], chunk);
    i2c_write_blocking(OLED_I2C_PORT, OLED_ADR, buf, chunk + 1, false);
  }
}

// Push whole buffer to OLED
void Ui::display() const
{
  for (uint8_t page = PAGE_0; page < Ui::PAGE_7; page++)
  {
    sendCmd(0xB0 + page);
    sendCmd(0x00);
    sendCmd(0x10);
    sendData(&buffer[OLED_WIDTH * page], OLED_WIDTH);
  }
}

// Push one page (8 rows) to OLED
void Ui::display(Page page) const
{
  sendCmd(0xB0 + page);
  sendCmd(0x00);
  sendCmd(0x10);
  sendData(&buffer[OLED_WIDTH * page], OLED_WIDTH);
}

void Ui::drawBattery() const
{
  uint8_t filled_pixels = (charge_ * 15) / 100; // battery display is 20px wide

  for (uint8_t i = 0; i < 15; i++)
  {
    // either draw a full column or just the outline
    buffer[OLED_WIDTH - 19 + i] = i < filled_pixels
                                      ? 0xBD
                                      : 0x81;
  }

  display(Ui::PAGE_0);
}

void Ui::drawSmallText(std::string text, Page page) const
{
  const int max_chars = OLED_WIDTH / FONT_WIDTH_S;
  const unsigned int len = text.size();

  // Early return if the string fits without scrolling
  if (len <= max_chars)
  {
    uint8_t start_col = (OLED_WIDTH - len * FONT_WIDTH_S) / 2;

    for (size_t i = 0; i < len; i++)
    {
      const uint8_t *char_bitmap = getCharBitmapS(text[i]);
      if (!char_bitmap)
        continue;

      for (uint8_t col = 0; col < FONT_WIDTH_S; col++)
      {
        buffer[OLED_WIDTH * page + start_col + i * FONT_WIDTH_S + col] = char_bitmap[col];
      }
    }

    display(page);
    return;
  }

  // Handle scrolling
  uint16_t modulo = len * FONT_WIDTH_S - OLED_WIDTH;
  uint16_t offset = offset_ % (modulo + 30);
  offset = offset < 20 ? 0 : offset - 20;
  if (offset > modulo)
    offset = modulo;

  for (size_t i = 0; i < len; i++)
  {
    const uint8_t *char_bitmap = getCharBitmapS(text[i]);
    if (!char_bitmap)
      continue;

    int base_col = i * FONT_WIDTH_S - offset;
    for (uint8_t col = 0; col < FONT_WIDTH_S; col++)
    {
      int col_pos = base_col + col;
      if (col_pos >= 0 && col_pos < OLED_WIDTH)
      {
        buffer[OLED_WIDTH * page + col_pos] = char_bitmap[col];
      }
    }
  }

  display(page);
}

// prints text to page and page + 1
void Ui::drawMediumText(string text, Page page) const
{
  if (page == PAGE_7)
    return;
  const int max_chars = OLED_WIDTH / FONT_WIDTH_M;
  uint len = text.size();

  // string is short enough so printing centered
  if (len <= max_chars)
  {
    uint8_t start_col = (OLED_WIDTH - len * FONT_WIDTH_M) / 2; // Center the text

    for (size_t i = 0; i < len; i++)
    {
      const uint16_t *char_bitmap = getCharBitmapM(text[i]);
      if (char_bitmap)
      {
        for (uint8_t col = 0; col < FONT_WIDTH_M; col++)
        {
          // writing top half of the character to page 5
          buffer[OLED_WIDTH * page + start_col + i * FONT_WIDTH_M + col] = char_bitmap[col];

          // writing the bottom half of the character to page 6
          buffer[OLED_WIDTH * (page + 1) + start_col + i * FONT_WIDTH_M + col] = char_bitmap[col] >> 8;
        }
      }
    }

    display(page);
    display(static_cast<Page>(page + 1));
    return;
  }

  // for scrolling, we let the screen stay at start and end for 20 cycles (20 * 50ms = 1s)
  uint16_t modulo = len * FONT_WIDTH_M - OLED_WIDTH;
  uint offset = offset_ % (modulo + 30);
  offset = offset < 20 ? 0 : offset - 20;
  offset = offset < modulo ? offset : modulo;

  for (size_t i = 0; i < len; i++)
  {
    const uint16_t *char_bitmap = getCharBitmapM(text[i]);
    if (char_bitmap)
    {
      for (uint8_t col = 0; col < FONT_WIDTH_M; col++)
      {
        int col_pos = i * FONT_WIDTH_M + col - offset;
        if (col_pos >= 0 && col_pos < OLED_WIDTH)
        {
          // writing top half of the character to page 5
          buffer[OLED_WIDTH * page + col_pos] = char_bitmap[col];

          // writing the bottom half of the character to page 6
          buffer[OLED_WIDTH * (page + 1) + col_pos] = char_bitmap[col] >> 8;
        }
      }
    }
  }

  display(page);
  display(static_cast<Page>(page + 1));
}
