#include "playing.h"
#include "ui.h"
#include "../fonts/medium.h"
#include "../fonts/small.h"
#include <string.h>

const uint8_t Playing::sinData_[7][7] = {
    {100, 81, 39, 5, 5, 39, 81},
    {81, 100, 81, 39, 5, 5, 39},
    {39, 81, 100, 81, 39, 5, 5},
    {5, 39, 81, 100, 81, 39, 5},
    {5, 5, 39, 81, 100, 81, 39},
    {39, 5, 5, 39, 81, 100, 81},
    {81, 39, 5, 5, 39, 81, 100}};

void Playing::onActivate() const
{
  drawArtist(0);
  drawSongName(0);
  drawVisualizer(sinData_[0]);
}

void Playing::onDeactivate() const
{
}

void Playing::drawArtist(uint16_t offset) const
{
  const int max_chars = OLED_WIDTH / FONT_WIDTH_S;
  const unsigned int len = artist_.size();

  // Early return if the string fits without scrolling
  if (len <= max_chars)
  {
    uint8_t start_col = (OLED_WIDTH - len * FONT_WIDTH_S) / 2;

    for (size_t i = 0; i < len; i++)
    {
      const uint8_t *char_bitmap = getCharBitmapS(artist_[i]);
      if (!char_bitmap)
        continue;

      for (uint8_t col = 0; col < FONT_WIDTH_S; col++)
      {
        buffer[OLED_WIDTH * Ui::PAGE_7 + start_col + i * FONT_WIDTH_S + col] = char_bitmap[col];
      }
    }

    display(Ui::PAGE_7);
    return;
  }

  // Handle scrolling
  uint16_t modulo = len * FONT_WIDTH_S - OLED_WIDTH;
  offset = offset % (modulo + 30);
  offset = offset < 20 ? 0 : offset - 20;
  if (offset > modulo)
    offset = modulo;

  for (size_t i = 0; i < len; i++)
  {
    const uint8_t *char_bitmap = getCharBitmapS(artist_[i]);
    if (!char_bitmap)
      continue;

    int base_col = i * FONT_WIDTH_S - offset;
    for (uint8_t col = 0; col < FONT_WIDTH_S; col++)
    {
      int col_pos = base_col + col;
      if (col_pos >= 0 && col_pos < OLED_WIDTH)
      {
        buffer[OLED_WIDTH * Ui::PAGE_7 + col_pos] = char_bitmap[col];
      }
    }
  }

  display(Ui::PAGE_7);
}

void Playing::drawSongName(uint16_t offset) const
{
  const int max_chars = OLED_WIDTH / FONT_WIDTH_M;
  uint len = songName_.size();

  // string is short enough so printing centered
  if (len <= max_chars)
  {
    uint8_t start_col = (OLED_WIDTH - len * FONT_WIDTH_M) / 2; // Center the text

    for (size_t i = 0; i < len; i++)
    {
      const uint16_t *char_bitmap = getCharBitmapM(songName_[i]);
      if (char_bitmap)
      {
        for (uint8_t col = 0; col < FONT_WIDTH_M; col++)
        {
          // writing top half of the character to page 5
          buffer[OLED_WIDTH * Ui::PAGE_5 + start_col + i * FONT_WIDTH_M + col] = char_bitmap[col];

          // writing the bottom half of the character to page 6
          buffer[OLED_WIDTH * Ui::PAGE_6 + start_col + i * FONT_WIDTH_M + col] = char_bitmap[col] >> 8;
        }
      }
    }

    display(Ui::PAGE_5);
    display(Ui::PAGE_6);
    return;
  }

  // for scrolling, we let the screen stay at start and end for 20 cycles (20 * 50ms = 1s)
  uint16_t modulo = len * FONT_WIDTH_M - OLED_WIDTH;
  offset = offset % (modulo + 30);
  offset = offset < 20 ? 0 : offset - 20;
  offset = offset < modulo ? offset : modulo;

  for (size_t i = 0; i < len; i++)
  {
    const uint16_t *char_bitmap = getCharBitmapM(songName_[i]);
    if (char_bitmap)
    {
      for (uint8_t col = 0; col < FONT_WIDTH_M; col++)
      {
        int col_pos = i * FONT_WIDTH_M + col - offset;
        if (col_pos >= 0 && col_pos < OLED_WIDTH)
        {
          // writing top half of the character to page 5
          buffer[OLED_WIDTH * Ui::PAGE_5 + col_pos] = char_bitmap[col];

          // writing the bottom half of the character to page 6
          buffer[OLED_WIDTH * Ui::PAGE_6 + col_pos] = char_bitmap[col] >> 8;
        }
      }
    }
  }

  display(Ui::PAGE_5);
  display(Ui::PAGE_6);
}

void Playing::drawVisualizer(const uint8_t data[7]) const
{
  for (uint8_t i = 0; i < 7; i++)
  {
    uint8_t height = data[i] * 30 / 100;
    height = height < 4 ? 4 : height;

    uint8_t start_col = 4 + i * (14 + 4); // 14px wide bar + 4px gap

    for (uint8_t col = 0; col < 14; col++)
    {
      for (uint8_t row = 0; row < height; row++)
      {
        uint8_t from_bottom = 31 - row; // 32 pixels total, so index 0 = bottom
        uint8_t page = from_bottom / 8;
        buffer[OLED_WIDTH * (Ui::PAGE_1 + page) + start_col + col] |= 1 << (from_bottom % 8);
      }
    }
  }

  display(Ui::PAGE_1);
  display(Ui::PAGE_2);
  display(Ui::PAGE_3);
  display(Ui::PAGE_4);
}