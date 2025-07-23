#include "ui.h"
#include "../constants.h"
#include "../app.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstring>

uint8_t Ui::buffer[OLED_BUFFER_SIZE] = {0};

Ui::Ui(Mode mode) : mode_(mode), charging_(false), charge_(100)
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