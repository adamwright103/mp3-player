#ifndef MEDIUM_H
#define MEDIUM_H

#include <stdint.h>

// small fant is 8x12 pixels
#define FONT_WIDTH_M 8
#define FONT_HEIGHT_M 12
#define FONT_NUM_CHARS_M 95

// Function to get the bitmap for a character
const uint16_t *getCharBitmapM(char c);

#endif // SMALL_H