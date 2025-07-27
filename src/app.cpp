#include "app.h"
#include "ui/modes/playing.h"
#include "ui/modes/albumselect.h"
#include <stdio.h>

App::App()
{
  currentMode_ = nullptr;
  changeMode(Ui::ALBUM_SELECT);
}

App::App(Ui::Mode mode)
{
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
    currentMode_ = new Playing();
    break;

  case Ui::ALBUM_SELECT:
    currentMode_ = new AlbumSelect();
    break;

  default:
    currentMode_ = nullptr;
    break;
  }

  if (currentMode_)
  {
    currentMode_->clearBuffer();
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
