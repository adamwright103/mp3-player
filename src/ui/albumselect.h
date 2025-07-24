#ifndef ALBUMSELECT_H
#define ALBUMSELECT_H

#include "ui.h"
#include "../types/node.h"
#include <string>

class AlbumSelect : public Ui
{
private:
  Node<std::string> *currentAlbum_; // circular linked list of albums

public:
  AlbumSelect();
  ~AlbumSelect() override { delete currentAlbum_; }

  void onActivate() override;
  void onDeactivate() override;

  void drawAlbumArt() const;
  void drawAlbumName() const;

  Node<std::string> *growAlbumList();
  void deleteAlbumList();
  std::string getAlbumName() const { return currentAlbum_ ? currentAlbum_->data : ""; }
};

#endif // ALBUMSELECT_H