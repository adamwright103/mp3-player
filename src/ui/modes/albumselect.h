#ifndef ALBUMSELECT_H
#define ALBUMSELECT_H

#include "../ui.h"
#include "src/types/node.h"
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
  inline std::string getAlbumName() const { return currentAlbum_ ? currentAlbum_->data : ""; }

  void onLeftButtonPress() override;
  void onRightButtonPress() override;
};

#endif // ALBUMSELECT_H