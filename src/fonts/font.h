#ifndef FONT_H
#define FONT_H

#include <stdint.h>

typedef struct
{
  uint8_t width;
  uint8_t height;
  const void *data;
  const void *(*get_bitmap)(char c);
} Font;

extern const Font smallFont;
extern const Font mediumFont;

#endif // FONT_H