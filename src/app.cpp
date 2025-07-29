#include "app.h"
#include "ui/modes/playing.h"
#include "ui/modes/albumselect.h"
#include <stdio.h>

App::App() : App(Ui::ALBUM_SELECT) {}

App::App(Ui::Mode mode)
{
  Ui::init();
  currentMode_ = nullptr;
  changeMode(mode);
}

App::~App()
{
  if (currentMode_)
  {
    currentMode_->onDeactivate();
    delete currentMode_;
  }
}

void App::changeMode(Ui::Mode mode)
{
  if (currentMode_)
  {
    currentMode_->onDeactivate();
    delete currentMode_;
  }

  switch (mode)
  {
  case Ui::PLAYING:
    printf("Changing mode to Playing\n");
    currentMode_ = new Playing();
    break;

  case Ui::ALBUM_SELECT:
    printf("Changing mode to AlbumSelect\n");
    currentMode_ = new AlbumSelect();

    printf("Current album: %s\n",
           static_cast<AlbumSelect *>(currentMode_)->getAlbumName().c_str());

    break;

  default:
    printf("err chaning mode, we dont know where we are\n");
    currentMode_ = nullptr;
    break;
  }

  if (currentMode_)
  {
    currentMode_->onActivate();
  }
}

void App::onButtonPress(uint gpio)
{
  switch (gpio)
  {
  case LEFT_BTN_PIN:
    if (currentMode_)
    {
      currentMode_->onLeftButtonPress();
    }
    break;
  case HOME_BTN_PIN:
    changeMode(
        currentMode_ && currentMode_->getMode() == Ui::ALBUM_SELECT
            ? Ui::PLAYING
            : Ui::ALBUM_SELECT);
    break;
  case RIGHT_BTN_PIN:
    if (currentMode_)
    {
      currentMode_->onRightButtonPress();
    }
    break;
  }
}
