#ifndef CONSTANTS_H
#define CONSTANTS_H

// Pin out
#define HOME_BTN_PIN 7
#define OLED_SDA_PIN 18
#define OLED_SCL_PIN 19

// I2C config
#define OLED_I2C_PORT i2c1
#define OLED_ADR 0x3C
#define OLED_CLK_FREQ 400 * 1000
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_BUFFER_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)

#define DEBOUNCE_TIME_MS 150

#endif // CONSTANTS_H