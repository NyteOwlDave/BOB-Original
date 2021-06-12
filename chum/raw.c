#include "stdafx.h"
#include "raw.h"

// Image file header (RAW)
typedef struct tagRAWFILEHEADER {
    char magic[4];
    WORD height;
    WORD width;
} RAWFILEHEADER, *LPRAWFILEHEADER;

// Load image from file (RAW)
LPIMAGE LoadImageFileRaw(LPCSTR szPathName) {
    FILE* fp = fopen(szPathName, "rb");
    if (!fp) {
        perror("Can't open raw image file");
        return NULL;
    }
    RAWFILEHEADER hdr;
    size_t c = fread(&hdr, sizeof(hdr), 1, fp);
    if (c != 1) {
        perror("Can't read raw image file header");
        fclose(fp);
        return NULL;
    }
    if ((hdr.magic[0]!='R') ||
        (hdr.magic[1]!='A') ||
        (hdr.magic[2]!='W') ||
        (hdr.magic[3]!=0)) {
            Warning("Wrong raw image file header signature");
            fclose(fp);
            return NULL;
    }
    if (hdr.height < 1) {
        Warning("Raw image file has zero height");
        fclose(fp);
        return NULL;
    }
    if (hdr.width < 1) {
        Warning("Raw image file has zero width");
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
                perror("Failed to read raw image file pixels");
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

// Save image to file (RAW)
BOOL SaveImageFileRaw(LPCSTR szPathName, LPIMAGE pImage) {
    FILE* fp = fopen(szPathName, "wb");
    if (!fp) {
        perror("Can't create raw image file");
        return FALSE;
    }
    if (pImage) {
        if (pImage->buffer) {
            RAWFILEHEADER hdr;
            hdr.magic[0] = 'R';
            hdr.magic[1] = 'A';
            hdr.magic[2] = 'W';
            hdr.magic[3] = 0;
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
                    perror("Failed to write raw image file pixels");
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

