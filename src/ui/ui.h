#ifndef UI_H
#define UI_H

#include "drivers/sd.h"
#include "pico/stdlib.h"
#include "../constants.h"
#include <string>

class Ui
{
public:
  enum Mode
  {
    ALBUM_SELECT,
    SONG_SELECT,
    PLAYING,
  };

  enum Page : uint8_t
  {
    PAGE_0 = 0,
    PAGE_1,
    PAGE_2,
    PAGE_3,
    PAGE_4,
    PAGE_5,
    PAGE_6,
    PAGE_7,
  };

private:
  Mode mode_;
  bool charging_;
  uint offset_;

protected:
  static uint8_t buffer[OLED_BUFFER_SIZE];
  static Sd *sd_;
  uint charge_;

public:
  Ui(Mode mode);
  virtual ~Ui() {}
  virtual void onActivate() = 0;
  virtual void onDeactivate() = 0;

  inline Mode getMode() const { return mode_; }
  void init() const;
  void sendCmd(uint8_t cmd) const;
  void sendData(uint8_t *data, uint len) const;

  void display() const;
  void display(Page page) const;
  void clearBuffer() const;
  void clearBuffer(Page page) const;

  void drawBattery() const;
  void drawSmallText(std::string text, Page page) const;
  void drawMediumText(std::string text, Page page) const;

  virtual void onLeftButtonPress() = 0;
  virtual void onRightButtonPress() = 0;
};

#endif // UIMODE_H