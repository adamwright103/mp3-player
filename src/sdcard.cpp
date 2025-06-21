#include <stdio.h>
#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"
#include "rtc.h"
#include "hw_config.h"
#include <cstring>

void sdTest()
{
    printf("starting test");
    // See FatFs - Generic FAT Filesystem Module, "Application Interface",
    // http://elm-chan.org/fsw/ff/00index_e.html
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr)
        printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    FIL fil;
    const char *const filename = "playlists/Bangers.txt";
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    if (FR_OK != fr && FR_EXIST != fr)
        printf("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    if (f_printf(&fil, "Hello, world!\n") < 0)
    {
        printf("f_printf failed\n");
    }
    fr = f_close(&fil);
    if (FR_OK != fr)
    {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    fr = f_open(&fil, filename, FA_READ);
    if (FR_OK != fr)
    {
        printf("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    }
    // Read one line from the file
    char lineBuffer[256];
    if (f_gets(lineBuffer, sizeof(lineBuffer), &fil) != nullptr)
    {
        // Remove trailing newline character if present
        lineBuffer[strcspn(lineBuffer, "\r\n")] = '\0';
        printf("Read line from %s: %s\n", filename, lineBuffer);
    }
    else
    {
        printf("Failed to read line from %s\n", filename);
    }

    // Close the file
    fr = f_close(&fil);
    if (FR_OK != fr)
    {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    // write the songname to playlists/ex.txt
    fr = f_open(&fil, "playlists/ex.txt", FA_WRITE);
    if (f_printf(&fil, "Hello, world!\n") < 0)
    {
        printf("f_printf failed\n");
    }

    f_unmount(pSD->pcName);

    printf("finishing test");
}

bool getFirstPlaylistFile(const char *playlistPath, char *outputBuffer, size_t bufferSize)
{
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr)
        printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);

    FIL fil;
    fr = f_open(&fil, playlistPath, FA_READ);

    if (FR_OK != fr)
    {
        printf("f_open(%s) error: %s (%d)\n", playlistPath, FRESULT_str(fr), fr);
    }

    if (f_gets(outputBuffer, bufferSize, &fil) != nullptr)
    {
        // Remove trailing newline character if present
        outputBuffer[strcspn(outputBuffer, "\r\n")] = '\0';
        printf("Read line from %s: %s\n", playlistPath, outputBuffer);
    }
    else
    {
        printf("Failed to read line from %s\n", playlistPath);
        return false;
    }

    f_unmount(pSD->pcName);
    return true;
}
