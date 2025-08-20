#include "albumselect.h"
#include "../ui.h"
#include "src/types/node.h"
#include "src/fonts/record.h"
#include <string>
#include <stdio.h>

using namespace std;

Node<string> *AlbumSelect::currentAlbum_ = nullptr;

void AlbumSelect::onActivate()
{
  if (!currentAlbum_)
  {
    // currentAlbum_ = sd_->getAlbums();
    currentAlbum_ = new Node<string>("White.txt");
    currentAlbum_->next = currentAlbum_;
    currentAlbum_->prev = currentAlbum_;
  }
  drawBattery();
  drawAlbumArt();
  drawAlbumName();
}

void AlbumSelect::onDeactivate()
{
}

void AlbumSelect::drawAlbumArt() const
{
  uint8_t offset = OLED_WIDTH / 2 - RECORD_WIDTH / 2;
  for (uint8_t page = PAGE_1; page <= PAGE_5; page++)
  {
    for (uint8_t col = 0; col < RECORD_WIDTH; col++)
    {
      Ui::buffer[OLED_WIDTH * page + col + offset] = recordBitmap[page - 1][col];
    }
    display(static_cast<Ui::Page>(page));
  }
}

void AlbumSelect::drawAlbumName() const
{
  string name = currentAlbum_ ? currentAlbum_->data : "No Albums.txt";
  name = name.substr(0, name.size() - 4);

  drawMediumText(name, Ui::PAGE_6);
}

void AlbumSelect::onLeftButtonPress()
{
  if (!currentAlbum_)
    return;

  currentAlbum_ = currentAlbum_->prev;
  drawAlbumName();
}

void AlbumSelect::onRightButtonPress()
{
  if (!currentAlbum_)
    return;

  currentAlbum_ = currentAlbum_->next;
  drawAlbumName();
}

void *AlbumSelect::onHomeButtonPress()
{
  Node<string> *currentSong = sd_->getSongs(currentAlbum_ ? currentAlbum_->data : "");

  // while (currentSong)
  // {
  //   printf("Song: %s\n", currentSong->data.c_str());
  //   currentSong = currentSong->next;
  //   if (currentSong)
  //     delete currentSong->prev;
  // }

  return nullptr;
}
