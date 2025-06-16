#ifndef SMALL_H
#define SMALL_H

#include <stdint.h>

// small fant is 6x8 pixels
#define FONT_WIDTH_S 6
#define FONT_HEIGHT_S 8
#define FONT_NUM_CHARS_S 95

// Function to get the bitmap for a character
const uint8_t *getCharBitmapS(char c);

#endif // SMALL_H