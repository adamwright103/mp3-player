#include "albumselect.h"
#include "../ui.h"
#include "src/types/node.h"
#include "src/fonts/record.h"
#include <string>

using namespace std;

AlbumSelect::AlbumSelect() : Ui(ALBUM_SELECT)
{
  currentAlbum_ = nullptr;
}

void AlbumSelect::onActivate()
{
  currentAlbum_ = growAlbumList();
  drawAlbumArt();
  drawAlbumName();
}

void AlbumSelect::onDeactivate()
{
  deleteAlbumList();
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
  drawMediumText(currentAlbum_ ? currentAlbum_->data : "No Albums", Ui::PAGE_6);
}

Node<string> *AlbumSelect::growAlbumList()
{
  static const string fakeAlbums[5] = {
      "Blue",
      "White",
      "Grey",
      "Pink",
      "Pink"};

  deleteAlbumList();

  Node<string> *head = new Node<string>(fakeAlbums[0]);
  Node<string> *current = head;
  for (uint8_t i = 1; i < 5; i++)
  {
    current->next = new Node<string>(fakeAlbums[i]);
    current->next->prev = current;
    current = current->next;
  }

  current->next = head;
  head->prev = current;

  return head;
}

void AlbumSelect::deleteAlbumList()
{
  if (!currentAlbum_)
    return;

  Node<string> *head = currentAlbum_;
  Node<string> *current = currentAlbum_;

  do
  {
    Node<string> *next = current->next;
    delete current;
    current = next;
  } while (current != head);

  currentAlbum_ = nullptr;
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
