#ifndef PIC_H_DEFINED
#define PIC_H_DEFINED

#include "image.h"

// Load image from file (PIC)
LPIMAGE LoadImageFilePic(LPCSTR szPathName);

// Save image to file (PIC)
BOOL SaveImageFilePic(LPCSTR szPathName, LPIMAGE pImage);

#endif // !PIC_H_DEFINED
