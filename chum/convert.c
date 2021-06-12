#include "stdafx.h"
#include <string.h>
#include "bmp.h"
#include "raw.h"
#include "bob.h"
#include "pic.h"
#include "vga.h"

typedef struct tagRENAMER {
    char szBaseName[MAX_PATH];
    char szInputName[MAX_PATH];
    char szOutputName[MAX_PATH];
    char szInputExt[MAX_PATH];
    char szOutputExt[MAX_PATH];
} RENAMER, *LPRENAMER;

static int rawtest1();
static void Rename(LPRENAMER data);
static int ConvertImageFile(LPCSTR szBaseName);

// Select image convert mode
#define PIC_TO_BMP

// [x] Convert RAW to IMG
#ifdef RAW_TO_IMG
    #define SRC_EXT "raw"
    #define DST_EXT "img"
    #define IMG_LOAD LoadImageFileRaw
    #define IMG_SAVE SaveImageFileBob
#endif // RAW_TO_IMG

// [x] Convert IMG to RAW
#ifdef IMG_TO_RAW
    #define SRC_EXT "img"
    #define DST_EXT "raw"
    #define IMG_LOAD LoadImageFileBob
    #define IMG_SAVE SaveImageFileRaw
#endif // IMG_TO_RAW

// [x] Convert RAW to VGA
#ifdef RAW_TO_VGA
    #define SRC_EXT "raw"
    #define DST_EXT "vga"
    #define IMG_LOAD LoadImageFileRaw
    #define IMG_SAVE SaveImageFileVga
#endif // RAW_TO_VGA

// [x] Convert VGA to RAW
#ifdef VGA_TO_RAW
    #define SRC_EXT "vga"
    #define DST_EXT "raw"
    #define IMG_LOAD LoadImageFileVga
    #define IMG_SAVE SaveImageFileRaw
#endif // !VGA_TO_RAW

// [x] Convert RAW to PIC
#ifdef RAW_TO_PIC
    #define SRC_EXT "raw"
    #define DST_EXT "pic"
    #define IMG_LOAD LoadImageFileRaw
    #define IMG_SAVE SaveImageFilePic
#endif // RAW_TO_PIC

// [x] Convert PIC to RAW
#ifdef PIC_TO_RAW
    #define SRC_EXT "pic"
    #define DST_EXT "raw"
    #define IMG_LOAD LoadImageFilePic
    #define IMG_SAVE SaveImageFileRaw
#endif // !PIC_TO_RAW

// [x] Convert IMG to BMP
#ifdef IMG_TO_BMP
    #define SRC_EXT "img"
    #define DST_EXT "bmp"
    #define IMG_LOAD LoadImageFileBob
    #define IMG_SAVE SaveImageFileBmp
#endif // IMG_TO_BMP

// [x] Convert VGA to BMP
#ifdef VGA_TO_BMP
    #define SRC_EXT "vga"
    #define DST_EXT "bmp"
    #define IMG_LOAD LoadImageFileVga
    #define IMG_SAVE SaveImageFileBmp
#endif // VGA_TO_BMP

// [x] Convert PIC to BMP
#ifdef PIC_TO_BMP
    #define SRC_EXT "pic"
    #define DST_EXT "bmp"
    #define IMG_LOAD LoadImageFilePic
    #define IMG_SAVE SaveImageFileBmp
#endif // PIC_TO_BMP

// [x] Convert RAW to BMP
// (default conversion mode)
#ifndef SRC_EXT
    #define SRC_EXT "raw"
    #define DST_EXT "bmp"
    #define IMG_LOAD LoadImageFileRaw
    #define IMG_SAVE SaveImageFileBmp
#endif // !SRC_EXT

// Helper to convert image file from one format to another
static int cvt(int argc, char** argp) {
    if (argc != 2) {
        Error("Wrong number of arguments");
    }
    return ConvertImageFile(argp[1]);
}

// Application entry point
int main(int argc, char** argp, char** envp) {
    return cvt(argc, argp);
    // return rawtest1();
}

// Convert image file from one format to another
static int ConvertImageFile(LPCSTR szBaseName) {
    RENAMER data;
    memset(&data, 0, sizeof(RENAMER));
    strcpy(data.szInputExt, SRC_EXT);
    strcpy(data.szOutputExt, DST_EXT);
    strcpy(data.szBaseName, szBaseName);
    Rename(&data);
    Info("\nReading input:");
    Info(data.szInputName);
    LPIMAGE pImage = IMG_LOAD(data.szInputName);
    if (pImage) {
        Info("\nWriting output:");
        Info(data.szOutputName);
        if (IMG_SAVE(data.szOutputName, pImage)) {
            return 0;
        }
    }
    return 1;
}

static int rawtest1() {
    Info("Creating IMAGE...");
    LPIMAGE pImage = CreateImage(32, 32);
    if (!pImage) {
        Error("Can't continue without IMAGE object");
    }
    ARGB color = { 0xF0, 0x20, 0x10, 0 };
    Info("Filling IMAGE...");
    FillImage(pImage, color);
    Info("Saving IMAGE...");
    SaveImageFileRaw("test1.raw", pImage);
    Info("Destroying IMAGE...");
    DestroyImage(pImage);
    Info("Done");
    return 0;
}

static void Rename(LPRENAMER data) {
    strcpy(data->szInputName, data->szBaseName);
    strcat(data->szInputName, ".");
    strcat(data->szInputName, data->szInputExt);
    strcpy(data->szOutputName, data->szBaseName);
    strcat(data->szOutputName, ".");
    strcat(data->szOutputName, data->szOutputExt);
}
