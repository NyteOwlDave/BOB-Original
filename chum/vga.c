#include "stdafx.h"
#include "vga.h"

// Image file header (VGA)
typedef struct tagVGAFILEHEADER {
    WORD width;
    WORD height;
} VGAFILEHEADER, *LPVGAFILEHEADER;

// Load image from file (Vga)
LPIMAGE LoadImageFileVga(LPCSTR szPathName) {
    FILE* fp = fopen(szPathName, "rb");
    if (!fp) {
        perror("Can't open vga image file");
        return NULL;
    }
    VGAFILEHEADER hdr;
    size_t c = fread(&hdr, sizeof(hdr), 1, fp);
    if (c != 1) {
        perror("Can't read vga image file header");
        fclose(fp);
        return NULL;
    }
    LPIMAGE pImage = CreateImage(hdr.width, hdr.height);
    if (pImage) {
        for (int y=0; y<(int)hdr.height; y++) {
            LPVOID p = pImage->line[y];
            c = fread(p, sizeof(ARGB), hdr.width, fp);
            if (c != hdr.width) {
                char sz[32];
                perror("Failed to read vga image file pixels");
                sprintf(sz, "Line: %i", y);
                Warning(sz);
                DestroyImage(pImage);
                fclose(fp);
                return NULL;
            }
        }
        fclose(fp);
        return pImage;
    } else {
        fclose(fp);
        return NULL;
    }
}

// Save image to file (VGA)
BOOL SaveImageFileVga(LPCSTR szPathName, LPIMAGE pImage) {
    FILE* fp = fopen(szPathName, "wb");
    if (!fp) {
        perror("Can't create vga image file");
        return FALSE;
    }
    if (pImage) {
        if (pImage->buffer) {
            VGAFILEHEADER hdr;
            hdr.width = pImage->w;
            hdr.height = pImage->h;
            size_t c = fwrite(&hdr, sizeof(hdr), 1, fp);
            if (c != 1) {
                perror("Failed to write file header");
                fclose(fp);
                return FALSE;
            }
            for (int y=0; y<(int)(hdr.height); y++) {
                LPVOID p = pImage->line[y];
                c = fwrite(p, sizeof(ARGB), hdr.width, fp);
                if (c != hdr.width) {
                    char sz[32];
                    perror("Failed to write vga image file pixels");
                    sprintf(sz, "Line: %i", y);
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

