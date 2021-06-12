<link rel="stylesheet" href="../assets/help.css"/>

[bmp]: <https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv4header>

# Chum Toolkit

The main folder contains a collection of image processing tools. Currently this consists of some image file conversion commands.

# Folders

| Name | Purpose | Notes |
|-|-|-|
| $ | System commands and scripts | |
| inc | Header files | |
| rt | Runtime files | |
| img2bmp | Build/test the img2bmp command | |
| img2raw | Build/test the img2raw command | |
| pic2bmp | Build/test the pic2bmp command | |
| pic2raw | Build/test the pic2raw command | |
| raw2bmp | Build/test the raw2bmp command | |
| raw2img | Build/test the raw2img command | |
| raw2pic | Build/test the raw2pic command | |
| raw2vga | Build/test the raw2vga command | |
| rawtest | Test the RAW functionality | |
| vga2bmp | Build/test the vga2bmp command | |
| vga2raw | Build/test the vga2raw command | |

# Image File Formats

All of the image file formats are 24 or 32 bits-per-pixel. Some are compressed.
The vary mainly by header information and how the pixels, runs and scan lines
are represented.

| Name | Purpose | Notes |
|-|-|-|
| BMP | Windows DIB | |
| IMG | BOB Ray Tracer Image | |
| PIC | NCS Picture | |
| RAW | NCS Raw Image | |
| VGA | NCS VGA Image | |

## BMP - Windows DIB

This one is far too complex to summarize easily. Refer to Microsoft docs for the DIB file format BI_RGB.

* See: [Bitmap Format][bmp]

## IMG - BOB Ray Tracer Image

This run length encoded format comes from the book
_Photorealism and Ray Tracing in C_. It's used by the BOB Ray Tracer provided
by that book, as well as a number of apps I've designed.

The header is 10 bytes long. This is broken up into 5 fields:

1. Width
2. Height
3. Start Line
4. Stop Line
5. Bits per pixel

NOTE: All fields are in BIG-ENDIAN (Motorola) byte order.

Width and height are unsigned (1-65535). Same for Start and Stop Line. Zero for start and stop line indicates full size image. Otherwise, these can be used to determine partial images that span multiple rendering sessions. Bits per pixel must be 24.

Scanlines are RLE encoded, with anywhere from 0 to 255 pixels in a run. Extra pixels are ignored. Missing pixels are undefined. Note that because of the way this is encoded, it's possible for the sum of pixels for a logical scanline to not match the image width. Some tools try to handle this gracefully, but it may cause a load failure for some. Those that handle it gracefully will generally produce mangled results beginning at the point of misalignment.

Each run is a single length byte, followed by B, G then R byte fields.

__NOTE__ For compatibility with the BOB app, run lengths should be in the range 1 to 254. No zero length runs or runs of length 255 are permitted.

## PIC - NCS Picture

This is a planar type based on the one introduced in the book
_Practical Ray Tracing in C_.

The file consists of a simple header, which is two unsigned 16-bit integers
defining the width and height (1-65535). This is immediately followed by a series of scanlines. The (dis)advantage of this format is that each scanline is written in three color planes (R, then G, then B) rather than each pixel containing all three color components. Each scanline also begins with an unsigned 16-bit word, which is the length of the scanline in pixels. Theoretically this could allow for truncated lines, but this is not done in the code. Rather each scanline length should equal the image width from the header.

## RAW - NCS Raw Image

This is a custom format I came up with for use with Linux apps. Silly of me to create yet another format.

This has a short header, followed by a series of scanlines in ARGB format. This is the same format used by Windows DIBs and the HTML5 2D graphics context.

The header is the signature string "RAW" followed by a NULL (zero) byte, then the image dimensions. First height then width as 16-bit unsigned values (1-65535).

## VGA - NCS VGA Image

This is an extremely simple format I came up with to compete with the BOB Ray Tracer image file format. Whereas IMG files are compressed as RLE, and have some extra header info, this just uses raw ARGB pixel scanlines.

The header contains just two signed 32-bit integers, one for each image dimension. This allows for some incredibly large images and also had lots of values that are out of range (0 and negative values). In this format, the width is first followed by the height.

# Conversions

Since there are 5 formats supported, for all possible conversions there should be `n(n-1)=5(4)=20` conversion commands. Rather than go through all that hassle, we just implement commands such that any format can be converted to or from RAW format. You can also convert any format to BMP format (but not the reverse).

To put it another way, you can convert:

* From IMG, PIC, or VGA to RAW or BMP
* From RAW to BMP, IMG, PIC, or VGA

This requires only half the number of conversion commands. It also means that some conversions must be handled using two commands.

For example, to go from PIC to VGA, first convert
to RAW:

```
pic2raw mypic
raw2vga mypic
```

Also: for the sake avoiding lots of complexity, there is no way to convert from a BMP file to any other format. Windows tools exist for that purpose already!

| From\To| BMP | IMG | PIC | VGA | RAW |
|-|-|-|-|-|-|
| IMG | X |   |   |   | X |
| PIC | X |   |   |   | X |
| VGA | X |   |   |   | X |
| RAW | X | X | X | X |   |

You'll notice in the chart above that there is _not a row_ for conversions from BMP to any other format. Conversely, the _only_ format that all others can be converted to is BMP.

# Executables

The following executables are produced by the various build scripts:

| Name | Purpose | Notes |
|-|-|-|
| img2bmp | Convert IMG to BMP | 2 |
| img2raw | Convert IMG to RAW | 3 |
| pic2bmp | Convert PIC to BMP | 4 |
| pic2raw | Convert PIC to RAW | 5 |
| raw2bmp | Convert RAW to BMP | 6 |
| raw2img | Convert RAW to IMG | 7 |
| raw2pic | Convert RAW to PIC | 8 |
| raw2vga | Convert RAW to VGA | 9 |
| rawtest1 | Creates a sample RAW image | 1, 10 |
| vga2bmp | Convert VGA to BMP | 11 |
| vga2raw | Convert VGA to RAW | 12 |

### NOTES

1. Sample image is solid color #1020F0, size = 32 x 32 pixels
2. Define IMG_TO_BMP in ex-02.c
3. Define IMG_TO_RAW in ex-02.c
4. Define PIC_TO_BMP in ex-02.c
5. Define PIC_TO_RAW in ex-02.c
6. Define RAW_TO_BMP in ex-02.c
7. Define RAW_TO_IMG in ex-02.c
8. Define RAW_TO_PIC in ex-02.c
9. Define RAW_TO_VGA in ex-02.c
10. Modify main() to call rawtest1() in ex-02.c
11. Define VGA_TO_BMP in ex-02.c
12. Define VGA_TO_RAW in ex-02.c

# Source Files

## Headers

| Name | Purpose | Notes |
|-|-|-|
| bmp.h | Declares for bmp.c module | |
| bob.h | Declares for bob.c module | |
| clrspace.h | Color space declares | |
| image.h | Declares for image.c module | |
| pic.h | Declares for pic.c module | |
| raw.h | Declares for raw.c module | |
| report.h | Declares for report.c module | |
| stdafx.h | Common header for all modules | 1 |
| vga.h | Declares for vga.c module | |
| wintypes.h | Windows type definitions | |

### Notes

1. Used interally as first include in each module

## Modules

| Name | Purpose | Notes |
|-|-|-|
| bmp.c | BMP file functions | 1 |
| bob.c | IMG file functions | 1 |
| clrspace.c | Color space functions | 5 |
| convert.c | Application | 4 |
| image.c | Image memory object and drawing functions| 2 |
| pic.c | PIC file functions | 1 |
| raw.c | RAW file functions | 1 |
| report.c | Output message reporting functions| 3 |
| vga.c | VGA file functions | 1 |
| wintypes.c | Windows API Data Types | 4 |

### NOTES

1. Image file functions
2. Core graphics functions
3. Support functions
4. Contains main()
5. This filename is reserved to match clrspace.h
6. This filename is reserved to match wintypes.h

# Script Files

| Name | Purpose | Notes |
|-|-|-|
| build | Compiles and links the project | 1 |
| exe-folders | Creates any missing project folders | |

### NOTES

1. Creates a binary file named "exe" in the working folder

# Functions by Module

All functions are global unless otherwise specified. Declarations
for global functions are provided in header (.h) files with the
same name as the module (.c) file.

| Name | Purpose | Module | Notes |
|-|-|-|-|
| LoadImageFileBmp | Load image from file (BMP) | bmp.c | 1 |
| SaveImageFileBmp | Save image to file (BMP) | bmp.c | 2 |
| LoadImageFileBob | Load image from file (BOB) | bob.c | |
| SaveImageFileBob | Save image to file (BOB) | bob.c | |
| WriteScanLine | Scanline filler (helper) | bob.c | 4 |
| rawtest1 | | bob.c | 4 |
| Rename | | bob.c | 4 |
| ConvertImageFile | Convert image file | bob.c | 4 |
| main | App entry point | bob.c | |
| cvt | Helper to convert image file from one format to another | bob.c | 4 |
| CreateImage | Create a new image object | image.c | |
| DestroyImage | Destroy an existing image object | image.c | |
| GetPixel | Read pixel (slow and safe) | image.c | |
| SetPixel | Write pixel (slow and safe) | image.c | |
| a_GetPixel | Read pixel (unsafe and fast) | image.c | |
| a_SetPixel | Write pixel (unsafe and fast) | image.c | |
| ImageContains | Determine if point is inside image | image.c | |
| FillImage | Fill image with a color | image.c | |
| LoadImageFilePic | Load image from file (PIC) | pic.c | |
| SaveImageFilePic | Save image to file (PIC) | pic.c | |
| LoadImageFileRaw | Load image from file (RAW) | raw.c | |
| SaveImageFileRaw | Save image to file (RAW) | raw.c | |
| Info | Display an informational message | report.c | |
| Warning | Display a warning message | report.c | |
| Error | Display an error message | report.c | |
| LoadImageFileVga | Load image from file (VGA) | vga.c | |
| SaveImageFileVga | Save image to file (VGA) | vga.c | |

### NOTES

1. Not currently implemented (dummy call)
2. Only supports 32-bpp BI_RGB format
3. Inline local
4. Static local

# Data Types by Module

| Name | Purpose | Module | Notes |
|-|-|-|-|
| RGBTRIPLE | RGB triplet | bmp.c | |
| RGBQUAD | ARGB quad | bmp.c | |
| BITMAP | Device dependent bitmap | bmp.c | |
| BITMAPCOREHEADER | Bitmap core header | bmp.c | |
| BITMAPCOREINFO | Bitmap core info header | bmp.c | |
| BITMAPINFOHEADER | Bitmap info header | bmp.c | |
| BITMAPV4HEADER | Version 4 bitmap header| bmp.c | |
| BITMAPV5HEADER | Version 5 bitmap header | bmp.c | |
| BITMAPINFO | Bitmap info header w/ palette | bmp.c | |
| BITMAPFILEHEADER | Bitmap file header | bmp.c | |
| FXPT2DOT30 | Fixed point 2.30 | clrspace.c | 1 |
| LCSCSTYPE | ? | clrspace.c | 1, 2 |
| LCSGAMUTMATCH | ? | clrspace.c | 1, 2 |
| CIEXYZTRIPLE | ? | clrspace.c | 1, 2 |
| LOGCOLORSPACEA | Logical color space | clrspace.c | 1 |
| RENAMER | File renaming parameters | ex-02.c | |
| ARGB | Pixel in ARGB format | image.c | 3 |
| IMAGE | Image (picture) in memory | image.c | |
| PICFILEHEADER | Image file header (PIC) | pic.c | |
| RAWFILEHEADER | Image file header (RAW) | raw.c | |
| VGAFILEHEADER | Image file header (VGA) | vga.c | |
| BOOL | Boolean (TRUE or FALSE)| wintypes.h | |
| VOID | Void (untyped) | wintypes.c | |
| CHAR | Signed byte (8 bit) | wintypes.c | |
| SHORT | Signed short integer (16 bit) | wintypes.c | |
| INT | Signed integer (32 bit) | wintypes.c | |
| LONG | Signed long integer (64 bit) | wintypes.c | |
| UCHAR | Unsigned byte (8-bit) | wintypes.c | |
| USHORT | Unsigned short integer (16-bit) | wintypes.c | |
| UINT | Unsigned integer (32-bit) | wintypes.c | |
| ULONG | Unsigned long integer (64-bit) | wintypes.c | |
| BYTE | Unsigned byte (8-bit) | wintypes.c | |
| WORD | Unsigned short integer (16-bit) | wintypes.c | |
| DWORD | Unsigned integer (32-bit) | wintypes.c | |
| QWORD | Unsigned long integer (64-bit) | wintypes.c | |
| LPSTR | Pointer to string | wintypes.c | |
| LPCSTR | Pointer to string constant | wintypes.c | |
| LPVOID | Pointer to any data type | wintypes.c | |
| LPCVOID | Pointer to any data type constant | wintypes.c | |
| MEMO | Descriptor (Symbol table entry) | wintypes.c | |

### NOTES

1. These are extended Windows data types (specialized)
2. Purpose to be determined
3. Packed into a 32-bit unsigned integer

# Macros by Module

| Name | Purpose | Module | Notes |
|-|-|-|-|
| RAW_TO_IMG | Configure for raw2img command | convert.c | 1 |
| IMG_TO_RAW | Configure for img2raw command | convert.c | 1 |
| RAW_TO_VGA | Configure for raw2vga command | convert.c | 1 |
| VGA_TO_RAW | Configure for vga2raw command | convert.c | 1 |
| RAW_TO_PIC | Configure for raw2pic command | convert.c | 1 |
| PIC_TO_RAW | Configure for pic2raw command | convert.c | 1 |
| IMG_TO_BMP | Configure for img2bmp command | convert.c | 1 |
| VGA_TO_BMP | Configure for vga2bmp command | convert.c | 1 |
| PIC_TO_BMP | Configure for pic2bmp command | convert.c | 1 |
| SRC_EXT | Source filename extension for current command | convert.c | 2, 3 |
| DST_EXT | Destination filename extension for current command | convert.c | 2, 3 |
| IMG_LOAD | File load function for current command | convert.c | 2, 4 |
| IMG_SAVE | File save function for current command | convert.c | 2, 4 |
| TRUE | Boolean value | wintypes.c | |
| FALSE | Boolean value | wintypes.c | |
| MAX_PATH | Maximum pathname length (DOS) | wintypes.c | |

### NOTES

1. Define this to chose a command to build
2. Automatically defined for the chosen command
3. Null terminated string constant
4. Maps to an actual function name

# Windows Data Types

As a convenience for portability, this project uses some common
data types from the Windows API. There are three headers that
contain these data types.

| Name | Purpose | Notes |
|-|-|-|
| bmp.h | Bitmaps | |
| clrspace.h | Color space | |
| wintypes.h | Common data types | |

---
