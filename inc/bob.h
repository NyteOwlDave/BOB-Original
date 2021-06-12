#ifndef BOB_H_DEFINED
#define BOB_H_DEFINED

#include "image.h"

// Load image from file (BOB)
LPIMAGE LoadImageFileBob(LPCSTR szPathName);

// Save image to file (BOB)
BOOL SaveImageFileBob(LPCSTR szPathName, LPIMAGE pImage);

#endif // !BOB_H_DEFINED
