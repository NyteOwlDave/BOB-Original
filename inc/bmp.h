#ifndef BITMAP_H_DEFINED
#define BITMAP_H_DEFINED

#include "image.h"

// Load image from file (BMP)
LPIMAGE LoadImageFileBmp(LPCSTR szPathName);

// Save image to file (BMP)
BOOL SaveImageFileBmp(LPCSTR szPathName, LPIMAGE pImage);

#endif // !BITMAP_H_DEFINED
