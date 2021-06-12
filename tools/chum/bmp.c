#include "stdafx.h"
#include "bmp.h"
#include "clrspace.h"

#pragma pack(push)

#pragma pack(1)

typedef struct __attribute__ ((__packed__)) tagRGBTRIPLE {
  BYTE rgbtBlue;
  BYTE rgbtGreen;
  BYTE rgbtRed;
} RGBTRIPLE, *PRGBTRIPLE, *NPRGBTRIPLE, *LPRGBTRIPLE;

typedef struct __attribute__ ((__packed__)) tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD, *PRGBQUAD, *NPRGBQUAD, *LPRGBQUAD;

typedef struct __attribute__ ((__packed__)) tagBITMAP {
  LONG   bmType;
  LONG   bmWidth;
  LONG   bmHeight;
  LONG   bmWidthBytes;
  WORD   bmPlanes;
  WORD   bmBitsPixel;
  LPVOID bmBits;
} BITMAP, *PBITMAP, *NPBITMAP, *LPBITMAP;

typedef struct __attribute__ ((__packed__)) tagBITMAPCOREHEADER {
  DWORD bcSize;
  WORD  bcWidth;
  WORD  bcHeight;
  WORD  bcPlanes;
  WORD  bcBitCount;
} BITMAPCOREHEADER, *LPBITMAPCOREHEADER, *PBITMAPCOREHEADER;

typedef struct __attribute__ ((__packed__)) tagBITMAPCOREINFO {
  BITMAPCOREHEADER bmciHeader;
  RGBTRIPLE        bmciColors[1];
} BITMAPCOREINFO, *LPBITMAPCOREINFO, *PBITMAPCOREINFO;

typedef struct __attribute__ ((__packed__)) tagBITMAPINFOHEADER {
  DWORD biSize;
  INT   biWidth;
  INT   biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  INT   biXPelsPerMeter;
  INT   biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct __attribute__ ((__packed__)) tagBITMAPV4HEADER {
  DWORD        bV4Size;
  LONG         bV4Width;
  LONG         bV4Height;
  WORD         bV4Planes;
  WORD         bV4BitCount;
  DWORD        bV4V4Compression;
  DWORD        bV4SizeImage;
  LONG         bV4XPelsPerMeter;
  LONG         bV4YPelsPerMeter;
  DWORD        bV4ClrUsed;
  DWORD        bV4ClrImportant;
  DWORD        bV4RedMask;
  DWORD        bV4GreenMask;
  DWORD        bV4BlueMask;
  DWORD        bV4AlphaMask;
  DWORD        bV4CSType;
  CIEXYZTRIPLE bV4Endpoints;
  DWORD        bV4GammaRed;
  DWORD        bV4GammaGreen;
  DWORD        bV4GammaBlue;
} BITMAPV4HEADER, *LPBITMAPV4HEADER, *PBITMAPV4HEADER;

typedef struct __attribute__ ((__packed__)) tagBITMAPV5HEADER {
  DWORD        bV5Size;
  LONG         bV5Width;
  LONG         bV5Height;
  WORD         bV5Planes;
  WORD         bV5BitCount;
  DWORD        bV5Compression;
  DWORD        bV5SizeImage;
  LONG         bV5XPelsPerMeter;
  LONG         bV5YPelsPerMeter;
  DWORD        bV5ClrUsed;
  DWORD        bV5ClrImportant;
  DWORD        bV5RedMask;
  DWORD        bV5GreenMask;
  DWORD        bV5BlueMask;
  DWORD        bV5AlphaMask;
  DWORD        bV5CSType;
  CIEXYZTRIPLE bV5Endpoints;
  DWORD        bV5GammaRed;
  DWORD        bV5GammaGreen;
  DWORD        bV5GammaBlue;
  DWORD        bV5Intent;
  DWORD        bV5ProfileData;
  DWORD        bV5ProfileSize;
  DWORD        bV5Reserved;
} BITMAPV5HEADER, *LPBITMAPV5HEADER, *PBITMAPV5HEADER;

typedef struct __attribute__ ((__packed__)) tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

typedef struct __attribute__ ((__packed__)) tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

#pragma pack(pop)

// Load image from file (BMP)
LPIMAGE LoadImageFileBmp(LPCSTR szPathName) {
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    // TODO...
    Error("LoadImageFileBmp() function not implemented");
    // To keep the compiler from complaining
    return NULL;
}

// Save image to file (BMP)
BOOL SaveImageFileBmp(LPCSTR szPathName, LPIMAGE pImage) {
    // Open file for o/p
    FILE* fp = fopen(szPathName, "wb");
    if (!fp) {
        perror("Can't create bmp image file");
        return FALSE;
    }
    size_t n = sizeof (DWORD);
    if (pImage) {
        if (pImage->buffer) {
            char sz[128];
            BITMAPFILEHEADER bmfh;
            BITMAPINFOHEADER bmih;
            // Calculate metrics
            DWORD cbFileHeader = sizeof (bmfh);
            DWORD cbInfoHeader = sizeof (bmih);
            DWORD cbPixel = sizeof (RGBQUAD);
            DWORD cbScanLine = (DWORD)(pImage->w) * cbPixel;
            DWORD cbPixelMap = (DWORD)(pImage->h) * cbScanLine;
            DWORD cbTotal = cbPixelMap + cbInfoHeader + cbFileHeader;
/*
            sprintf(sz, "File Header Bytes: %u", cbFileHeader); Info(sz);
            sprintf(sz, "Info Header Bytes: %u", cbInfoHeader); Info(sz);
            sprintf(sz, "Pixel Bytes: %u", cbPixel); Info(sz);
            sprintf(sz, "ScanLine Bytes: %u", cbScanLine); Info(sz);
            sprintf(sz, "PixelMap Bytes: %u", cbPixelMap); Info(sz);
            sprintf(sz, "Total Bytes: %u", cbTotal); Info(sz);
*/
            // Initialize file header
            bmfh.bfType = ((WORD)'M'<<8) + (WORD)'B';
            bmfh.bfOffBits = cbFileHeader + cbInfoHeader;
            bmfh.bfReserved1 = 0;
            bmfh.bfReserved2 = 0;
            bmfh.bfSize = cbPixelMap;
            size_t c = fwrite(&bmfh, cbFileHeader, 1, fp);
            if (c != 1) {
                perror("Failed to write file header");
                fclose(fp);
                return FALSE;
            }
            // Initialize info header
            bmih.biBitCount = 32;
            bmih.biClrImportant = 0;
            bmih.biClrUsed = 0;
            bmih.biCompression = 0L; // BI_RGB
            bmih.biHeight = pImage->h;
            bmih.biPlanes = 1;
            bmih.biSize = cbInfoHeader;
            bmih.biSizeImage = cbPixelMap;
            bmih.biWidth = pImage->w;
            bmih.biXPelsPerMeter = 2835; // 72 dpi
            bmih.biYPelsPerMeter = 2835; // 72 dpi
            c = fwrite(&bmih, cbInfoHeader, 1, fp);
            if (c != 1) {
                perror("Failed to write info header");
                fclose(fp);
                return FALSE;
            }
            DWORD cx = (DWORD)(pImage->w);
            DWORD cy = (DWORD)(pImage->h);
            for (DWORD y=0; y<cy; y++) {
                DWORD yy = cy - (y + 1); // Bottom up mode
                LPVOID p = pImage->line[yy];
                c = fwrite(p, sizeof(ARGB), cx, fp);
                if (c != cx) {
                    char sz[32];
                    perror("Failed to write bmp image file pixels");
                    sprintf(sz, "Line: %u", y);
                    Warning(sz);
                    fclose(fp);
                    return FALSE;
                }
            }
            fclose(fp);
            return TRUE;
        } else {
            Warning("Image has no pixel buffer");
            fclose(fp);
            return FALSE;
        }        
    } else {
        Warning("Image pointer is null");
        fclose(fp);
        return FALSE;
    }
}

