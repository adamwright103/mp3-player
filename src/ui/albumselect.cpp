#include "albumselect.h"
#include "ui.h"
#include "../types/node.h"
#include "../fonts/medium.h"
#include <string>

using namespace std;

AlbumSelect::AlbumSelect() : Ui(ALBUM_SELECT)
{
  currentAlbum_ = growAlbumList();
}

void AlbumSelect::onActivate()
{
  drawAlbumArt();
  drawAlbumName();
}

void AlbumSelect::onDeactivate()
{
  deleteAlbumList();
}

void AlbumSelect::drawAlbumArt() const
{
  // TODO add record symbol and left/right select arrows on pages 1-5
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
      "Beige",
      "Pink"};

  deleteAlbumList();

  Node<string> *head = new Node<string>(fakeAlbums[0]);
  Node<string> *current = head;
  for (uint8_t i = 1; i < 5; i++)
  {
    current->next = new Node<string>(fakeAlbums[i]);
    current = current->next;
  }

  current->next = head;

  return head;
}

void AlbumSelect::deleteAlbumList()
{
  Node<string> *current = currentAlbum_;
  while (current)
  {
    Node<string> *next = current->next;
    delete current;
    current = next;
  }
  currentAlbum_ = nullptr;
}