#include "app.h"
#include "ui/modes/playing.h"
#include "ui/modes/albumselect.h"
#include <stdio.h>

App::App() : App(Ui::ALBUM_SELECT) {}

App::App(Ui::Mode mode)
{
  Ui::init();
  currentMode_ = nullptr;
  changeMode(mode, nullptr);
}

App::~App()
{
  if (currentMode_)
  {
    currentMode_->onDeactivate();
    delete currentMode_;
    currentMode_ = nullptr;
  }
}

void App::changeMode(Ui::Mode mode, void *data)
{
  if (currentMode_)
  {
    currentMode_->onDeactivate();
    delete currentMode_;
    currentMode_ = nullptr;
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
    printf("err chaning mode, we dont know where we are\n");
    currentMode_ = nullptr;
    break;
  }
  if (currentMode_)
  {
    currentMode_->onActivate();
    printf("activated\n");
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
    if (currentMode_)
    {
      currentMode_->onHomeButtonPress();
    }
    break;
  case RIGHT_BTN_PIN:
    if (currentMode_)
    {
      currentMode_->onRightButtonPress();
    }
    break;
  }
}
