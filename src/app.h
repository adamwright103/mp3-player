#ifndef APP_H
#define APP_H

#include "ui/ui.h"
#include "pico/stdlib.h"

class App
{
private:
  Ui *currentMode_;

public:
  App();
  App(Ui::Mode mode);
  ~App();

  void changeMode(Ui::Mode mode, void *data);
  void onButtonPress(uint gpio);
};

#endif // APP_H