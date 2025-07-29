#ifndef SD_H
#define SD_H

#include "hw_config.h"
#include "pico/stdlib.h"
#include <string>

class Sd
{
private:
  bool mounted_;
  sd_card_t *pSd_;

public:
  Sd() : mounted_(false) {};
  ~Sd() = default;

  void mount();
  void unmount();
  inline bool isMounted() const { return mounted_; }
  bool readFile(std::string filename, char *buffer, size_t bufferSize) const;
};

#endif // SD_H