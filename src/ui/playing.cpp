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

void Playing::onActivate()
{
  drawArtist();
  drawSongName();
  drawVisualizer(sinData_[0]);
}

void Playing::onDeactivate()
{
}

void Playing::drawArtist() const
{
  drawSmallText(artist_, Ui::PAGE_7);
}

void Playing::drawSongName() const
{
  drawMediumText(songName_, Ui::PAGE_5);
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