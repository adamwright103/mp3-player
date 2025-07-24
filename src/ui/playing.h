#ifndef PLAYING_H
#define PLAYING_H

#include "ui.h"
#include <string>

class Playing : public Ui
{
private:
  std::string artist_;
  std::string songName_;
  static const uint8_t sinData_[7][7];

public:
  Playing() : Ui(PLAYING), artist_("Artist Name"), songName_("Song Title") {}
  ~Playing() override {}

  void onActivate() override;
  void onDeactivate() override;

  void drawArtist() const;
  void drawSongName() const;
  void drawVisualizer(const uint8_t data[7]) const;
};

#endif // PLAYING_H