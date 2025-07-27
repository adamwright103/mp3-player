#include "sd.h"
#include "f_util.h"
#include "ff.h"
#include "rtc.h"
#include "hw_config.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string>

void Sd::mount()
{
  if (!mounted_)
  {
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    mounted_ = (fr == FR_OK);
  }
}