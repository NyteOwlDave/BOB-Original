#ifndef VGA_H_DEFINED
#define VGA_H_DEFINED

#include "image.h"

// Load image from file (VGA)
LPIMAGE LoadImageFileVga(LPCSTR szPathName);

// Save image to file (VGA)
BOOL SaveImageFileVga(LPCSTR szPathName, LPIMAGE pImage);

#endif // !VGA_H_DEFINED

