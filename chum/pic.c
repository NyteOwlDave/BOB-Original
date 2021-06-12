#include "stdafx.h"
#include "pic.h"

// Image file header (PIC)
typedef struct tagPICFILEHEADER {
    int width;
    int height;
} PICFILEHEADER, *LPPICFILEHEADER;

// Load image from file (PIC)
LPIMAGE LoadImageFilePic(LPCSTR szPathName) {
    FILE* fp = fopen(szPathName, "rb");
    if (!fp) {
        perror("Can't open pic image file");
        return NULL;
    }
    PICFILEHEADER hdr;
    size_t c = fread(&hdr, sizeof(hdr), 1, fp);
    if (c != 1) {
        perror("Can't read vga image file header");
        fclose(fp);
        return NULL;
    }
    if ((hdr.width > 65535) || (hdr.width < 1) ||
        (hdr.height > 65535) || (hdr.height < 1)) {
        Warning("Invalid vga image size");
        fclose(fp);
        return NULL;
    }
    LPIMAGE pImage = CreateImage((WORD)hdr.width, (WORD)hdr.height);
    if (pImage) {
        for (int y=0; y<hdr.height; y++) {
            // Read row index (WORD little-endian)
            int index = fgetc(fp);
            index += (fgetc(fp)<<8);
            // Verify row index matches counter
            if (index != y) {
                char sz[64];
                Warning("Invalid row index in pic image file");
                sprintf(sz, "Line: %i != %i", y, index);
                Warning(sz);
                fclose(fp);
                return NULL;
            }
            // Read in red channel for scanline
            for (int x=0; x<hdr.width; x++) {
                pImage->line[index][x].r = (BYTE)(fgetc(fp));
            }
            // Read in green channel for scanline
            for (int x=0; x<hdr.width; x++) {
                pImage->line[index][x].g = (BYTE)(fgetc(fp));
            }
            // Read in blue channel for scanline
            for (int x=0; x<hdr.width; x++) {
                pImage->line[index][x].b = (BYTE)(fgetc(fp));
            }
        }
        fclose(fp);
        return pImage;
    } else {
        fclose(fp);
        return NULL;
    }
}

// Save image to file (PIC)
BOOL SaveImageFilePic(LPCSTR szPathName, LPIMAGE pImage) {
    FILE* fp = fopen(szPathName, "wb");
    if (!fp) {
        perror("Can't create pic image file");
        return FALSE;
    }
    if (pImage) {
        if (pImage->buffer) {
            PICFILEHEADER hdr;
            // Prepare and write file header
            hdr.width = pImage->w;
            hdr.height = pImage->h;
            size_t c = fwrite(&hdr, sizeof(hdr), 1, fp);
            if (c != 1) {
                perror("Failed to write file header");
                fclose(fp);
                return FALSE;
            }
            // Write all scanlines
            for (int y=0; y<(int)(hdr.height); y++) {
                // Write row index
                fputc(y%256, fp);
                fputc(y/256, fp);
                // Check for i/o error
                if (fflush(fp) == EOF) {
                    char sz[64];
                    perror("Row index pic image file");
                    sprintf(sz, "Line: %i", y);
                    Warning(sz);
                    fclose(fp);
                    return FALSE;
                }
                // Read in red channel for scanline
                for (int x=0; x<hdr.width; x++) {
                    fputc(pImage->line[y][x].r, fp);
                }
                // Read in green channel for scanline
                for (int x=0; x<hdr.width; x++) {
                    fputc(pImage->line[y][x].g, fp);
                }
                // Read in blue channel for scanline
                for (int x=0; x<hdr.width; x++) {
                    fputc(pImage->line[y][x].b, fp);
                }
                // Check for i/o error
                if (fflush(fp) == EOF) {
                    char sz[64];
                    perror("Scanline pic image file");
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
