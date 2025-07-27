#ifndef SD_H
#define SD_H

#include "pico/stdlib.h"
#include <string>

class Sd
{
private:
  bool mounted_;

public:
  Sd() : mounted_(false) {};
  ~Sd() = default;

  void mount();
  void unmount();
  inline bool isMounted() const { return mounted_; }
  bool readFile(std::string filename, uint8_t *buffer, size_t size);
};

#endif // SD_H