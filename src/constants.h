#ifndef CONSTANTS_H
#define CONSTANTS_H

// ---------- Pin out ----------
#define SD_SS_PIN 1
#define SD_SCK_PIN 2
#define SD_MOSI_PIN 3
#define SD_MISO_PIN 4
#define HOME_BTN_PIN 7
#define OLED_SDA_PIN 18
#define OLED_SCL_PIN 19

// --------- I2C config -------
#define OLED_I2C_PORT i2c1
#define OLED_ADR 0x3C
#define OLED_CLK_FREQ 400 * 1000
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_BUFFER_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)

// --------- SD CARD SPI -------
#define SD_SPI_PORT spi0
#define SD_SPI_BAUD_RATE 2500 * 1000
#define SD_PC_NAME "0:"

#define DEBOUNCE_TIME_MS 150

// ----------- Fonts -----------

// small font is 6x8 pixels, so can fit easily on one page
#define FONT_WIDTH_S 6
#define FONT_HEIGHT_S 8
#define FONT_NUM_CHARS_S 96

// medium font is 7x12 pixels and is centered vertically on 2 pages
#define FONT_WIDTH_M 7
#define FONT_HEIGHT_M 12
#define FONT_NUM_CHARS_M 96

// record is a 32x32 bitmap but weve pre centered it vertically on 5 pages
#define RECORD_WIDTH 32
#define RECORD_HIEGHT 40

#endif // CONSTANTS_H