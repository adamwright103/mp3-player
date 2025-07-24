#include "app.h"
#include "ui/playing.h"
#include "ui/albumselect.h"
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
    currentMode_->onActivate();
  }
}