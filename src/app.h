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
  ~App() { delete currentMode_; }

  void changeMode(Ui::Mode mode);
};

#endif // APP_H