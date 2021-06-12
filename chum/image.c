#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "report.h"

inline ARGB a_GetPixel(LPIMAGE pImage, int x, int y)
__attribute__((always_inline));

inline void a_SetPixel(LPIMAGE pImage, int x, int y, ARGB color) 
__attribute__((always_inline));

// Create new image
LPIMAGE CreateImage(WORD w, WORD h) {
    char sz[128];
    DWORD wi = (DWORD)w;
    // sprintf(sz, "Image width: %u", wi); Info(sz);
    DWORD hi = (DWORD)h;
    // sprintf(sz, "Image height: %u", hi); Info(sz);
    DWORD si = wi*hi;
    // sprintf(sz, "Image pixel count: %u", si); Info(sz);
    if (si<1) {
        Warning("Image is too small");
        return NULL;
    }
    if (si>(65535U*65535U)) {
        Warning("Image is too large");
        return NULL;
    }
    DWORD cbFull = sizeof(LPARGB) * hi + sizeof(IMAGE);
    // sprintf(sz, "Image object size: %u bytes", cbFull); Info(sz);
    LPIMAGE pImage = (LPIMAGE)malloc(cbFull);
    if (pImage) {
        pImage->w = w;
        pImage->h = h;
        // sprintf(sz, "Image object pointer %p", pImage); Info(sz);
        DWORD cbBuffer = sizeof(ARGB) * si;
        pImage->buffer = (LPARGB)malloc(cbBuffer);
        if (pImage->buffer) {
            // sprintf(sz, "Pixel buffer %u bytes, pointer %p", 
            //         cbBuffer, pImage->buffer); Info(sz);
            for (DWORD n=0; n<hi; n++) {
                LPARGB p = pImage->buffer + n * wi;
                // sprintf(sz, "Line %u pointer %p", n, p);
                pImage->line[n] = p;
            }
            return pImage;
        }
        free(pImage);
    }
    Warning("Out of memory");
    return NULL;
}

// Destroy existing image
BOOL DestroyImage(LPIMAGE pImage) {
    if (pImage) {
        if (pImage->buffer) {
            free(pImage->buffer);
        } else {
            Warning("No pixel buffer to free");
        }
        free(pImage);
        return TRUE;
    } else {
        Warning("Attempted to free null pointer to image");
        return FALSE;
    }
}

// Determine if point is within image
BOOL ImageContains(LPIMAGE pImage, int x, int y) {
    if (pImage && pImage->buffer) {
        if ((x<0)||(y<0)) return FALSE;
        if (x>=(int)(pImage->w)) return FALSE;
        if (y>=(int)(pImage->h)) return FALSE;
        return TRUE;
    }
    return FALSE;
}

// Read pixel (safe and slow)
ARGB GetPixel(LPIMAGE pImage, int x, int y) {
    if (ImageContains(pImage, x, y)) {
        return a_GetPixel(pImage, x, y);
    } else {
        ARGB color = { 0, 0, 0, 0 };
        return color;
    }
}

// Write pixel (safe and slow)
void SetPixel(LPIMAGE pImage, int x, int y, ARGB color) {
    if (ImageContains(pImage, x, y)) {
        a_SetPixel(pImage, x, y, color);
    }
}

// Fill entire image with a color
BOOL FillImage(LPIMAGE pImage, ARGB color) {
    if (pImage) {
        if (pImage->buffer) {
            for (int y=0; y<pImage->h; y++) {
                for (int x=0; x<pImage->w; x++) {
                    a_SetPixel(pImage, x, y, color);
                }
            }
            return TRUE;
        } else {
            Warning("Missing pixel buffer");
        }
    } else {
        Warning("Image pointer is null");
    }
    return FALSE;
}

