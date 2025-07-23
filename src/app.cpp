#include "app.h"
#include "ui/playing.h"
#include <stdio.h>

App::App()
{
  currentMode_ = nullptr;
  changeMode(Ui::PLAYING);
}

App::App(Ui::Mode mode)
{
  currentMode_ = nullptr;
  changeMode(mode);
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

  default:
    currentMode_ = nullptr;
    break;
  }

  if (currentMode_)
  {
    currentMode_->onActivate();
  }
}