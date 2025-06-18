#ifndef SDCARD_H
#define SDCARD_H

#include <stdint.h>

// Function to test SD card functionality
void sdTest();

// Function to retrieve the first file from a playlist
bool getFirstPlaylistFile(const char *playlistPath, char *outputBuffer, size_t bufferSize);

#endif // SDCARD_H