#ifndef ALBUMSELECT_H
#define ALBUMSELECT_H

#include "../ui.h"
#include "src/types/node.h"
#include <string>

class AlbumSelect : public Ui
{
private:
  static Node<std::string> *currentAlbum_;

public:
  AlbumSelect() : Ui(ALBUM_SELECT) {};
  ~AlbumSelect() {};

  void onActivate() override;
  void onDeactivate() override;

  void drawAlbumArt() const;
  void drawAlbumName() const;

  inline std::string getAlbumName() const { return currentAlbum_ ? currentAlbum_->data : ""; }

  void onLeftButtonPress() override;
  void onRightButtonPress() override;
  void *onHomeButtonPress() override;
};

#endif // ALBUMSELECT_H