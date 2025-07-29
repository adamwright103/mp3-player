#include "sd.h"
#include "f_util.h"
#include "ff.h"
#include "rtc.h"
#include "src/types/node.h"
#include "src/constants.h"
#include <stdio.h>
#include <cstring>

using namespace std;

void Sd::mount()
{
  if (!mounted_)
  {
    pSd_ = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSd_->fatfs, pSd_->pcName, 1);
    mounted_ = (fr == FR_OK);
    printf("mounted: %d\n", mounted_);
  }
}

void Sd::unmount()
{
  if (mounted_)
  {
    f_unmount(pSd_->pcName);
  }
}

bool Sd::readFile(std::string filename, char *buffer, size_t bufferSize) const
{
  if (!mounted_)
  {
    printf("SD card not mounted. Cannot read file: %s\n", filename.c_str());
    return false;
  }

  FIL fil;
  FRESULT fr = f_open(&fil, filename.c_str(), FA_READ);
  if (fr != FR_OK)
  {
    printf("f_open(%s) error: %s (%d)\n", filename.c_str(), FRESULT_str(fr), fr);
    return false;
  }

  if (FR_OK != fr)
  {
    printf("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
  }

  if (f_gets(buffer, bufferSize, &fil) != nullptr)
  {
    // Remove trailing newline character if present
    buffer[strcspn(buffer, "\r\n")] = '\0';
    printf("Read line from %s: %s\n", filename, buffer);
  }
  else
  {
    printf("Failed to read line from %s\n", filename);
    return false;
  }

  return true;
}

Node<string> *Sd::getAlbums() const
{
  if (!mounted_)
    return nullptr;

  DIR dir;
  FILINFO fno;
  FRESULT fr = f_opendir(&dir, ALBUM_DIR);
  if (fr != FR_OK)
  {
    printf("f_opendir(%s) error: %s (%d)\n", ALBUM_DIR, FRESULT_str(fr), fr);
    return nullptr;
  }

  Node<string> *head = nullptr;
  Node<string> *tail = nullptr;

  while (true)
  {
    fr = f_readdir(&dir, &fno);
    if (fr != FR_OK || fno.fname[0] == 0)
      break; // Break on error or end of dir
    if (fno.fattrib & AM_DIR)
      continue; // Skip directories

    printf("Found file: %s\n", fno.fname);
    Node<string> *node = new Node<string>(fno.fname);

    if (!head)
    {
      head = node;
      tail = node;
    }
    else
    {
      tail->next = node;
      node->prev = tail;
      tail = node;
    }
  }

  // we want a circular linked list for our carasoul
  tail->next = head;
  head->prev = tail;

  return head;
}
