#ifndef IMAGE_H_DEFINED
#define IMAGE_H_DEFINED

#include "wintypes.h"

// ARGB Structure
typedef struct tagARGB {
    BYTE b;
    BYTE g;
    BYTE r;
    BYTE a;
} ARGB, *LPARGB;

// IMAGE Structure
typedef struct tagIMAGE {
    WORD w;
    WORD h;
    DWORD id;
    LPARGB buffer;
    LPARGB line[];
} IMAGE, *LPIMAGE;

// Create a new image object
LPIMAGE CreateImage(WORD w, WORD h);

// Destroy an existing image object
BOOL DestroyImage(LPIMAGE pImage);

// Read pixel (slow and safe)
ARGB GetPixel(LPIMAGE pImage, int x, int y);

// Write pixel (slow and safe)
void SetPixel(LPIMAGE pImage, int x, int y, ARGB color);

// Read pixel (unsafe and fast)
inline ARGB a_GetPixel(LPIMAGE pImage, int x, int y)
{
    return (pImage->line[y][x]);
}

// Write pixel (unsafe and fast)
inline void a_SetPixel(LPIMAGE pImage, int x, int y, ARGB color) 
{
    pImage->line[y][x] = color;
}

// Determine if point is inside image
BOOL ImageContains(LPIMAGE pImage, int x, int y);

// Fill image with a color
BOOL FillImage(LPIMAGE pImage, ARGB color);

#endif // !IMAGE_H_DEFINED
