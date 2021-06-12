#include "stdafx.h"
#include "bob.h"

// Picture file
typedef struct tagPIC
{
	FILE*	fp;		    // stream pointer
	int		w;			// width
	int		h;			// height
} PIC, *PPIC;

// Inline functions
extern inline void a_SetPixel(LPIMAGE,int,int,ARGB)
__attribute__((always_inline));
extern inline ARGB a_GetPixel(LPIMAGE,int,int)
__attribute__((always_inline));

// Local functions
static BOOL WriteScanLine(PPIC pic, LPARGB buf);

// Save image to file (BOB)
BOOL SaveImageFileBob(LPCSTR szPathName, LPIMAGE pImage)
{
	PIC tmp;

    // Create file
    tmp.fp = fopen(szPathName, "wb");
    if (!tmp.fp) {
		perror(szPathName);
		return FALSE;
	}
    // Write file content
    if (pImage && pImage->buffer) {

        // Read dimensions
        tmp.w = (int)(pImage->w);
        tmp.h = (int)(pImage->h);

        // Write dimensions (big endian)
        fputc(tmp.w/256, tmp.fp);
        fputc(tmp.w%256, tmp.fp);
        fputc(tmp.h/256, tmp.fp);
        fputc(tmp.h%256, tmp.fp);
        
        // Write scanline range (big endian)
        fputc(0, tmp.fp);
        fputc(0, tmp.fp);
        fputc(tmp.h/256, tmp.fp);
        fputc(tmp.h%256, tmp.fp);

        // Write number of bit planes (big endian)
        fputc(    0, tmp.fp);
        fputc(   24, tmp.fp);

        // Write out scanlines
        for (int y=0; y<tmp.h; y++) {
            if (!WriteScanLine(&tmp, pImage->line[y])) {
                fclose(tmp.fp);
                return FALSE;
            }
        }
        fclose(tmp.fp);
        return TRUE;
    } else {
        fclose(tmp.fp);
        Warning("Image pointer is null");
        return FALSE;
    }
}

// Helper to write a single RLE compressed scanline
static BOOL WriteScanLine(PPIC pic, LPARGB buf)
{
	int	    i;		        // which pixel?
	int	    total;		    // how many left in scan?
	int	    count;		    // current run total
	int	    or, og, ob;	    // current run color
	int	    r, g, b;	    // next pixel color
	double  seconds;        // another helping?

	i = 0;
	total = pic->w;
	or = buf[i].r;
	og = buf[i].g;
	ob = buf[i].b;
	i++;
	do {
		count = 1;
		total--;
		while (TRUE) {
			r = buf[i].r;
			g = buf[i].g;
			b = buf[i].b;
			i++;
			if (r!=or || g!=og || b!=ob || count>=254 || total<=0) {
				break;
			}
			total--;
			count++;
		}
		if (fputc(count, pic->fp) == EOF) {
			perror("Disk write failed");
            return FALSE;
		}
		fputc(ob, pic->fp);
		fputc(og, pic->fp);
		fputc(or, pic->fp);

		or = r;
		og = g;
		ob = b;

        // Special case for final pixel
		if (total==1) {
			fputc(1, pic->fp);
			fputc(buf[pic->w-1].b, pic->fp);
			fputc(buf[pic->w-1].g, pic->fp);
			fputc(buf[pic->w-1].r, pic->fp);
			total--;
		}
	} while (total>0);
	if (fflush(pic->fp) == EOF) {
		perror("Disk flush failed");
        return FALSE;
    }
    return TRUE;
}

// Load image from file (BOB)
LPIMAGE LoadImageFileBob(LPCSTR szPathName) {

    LPIMAGE pImage;
    PIC     tmp;
	int	    i, j;
	int	    count;
    ARGB    color;

	tmp.fp = fopen(szPathName, "rb");
	if (!tmp.fp) {
		perror(szPathName);
		return NULL;
	}

	// Get width and height from header
	tmp.w  = fgetc(tmp.fp)<<8;
	tmp.w += fgetc(tmp.fp);
	tmp.h  = fgetc(tmp.fp)<<8;
	tmp.h += fgetc(tmp.fp);

	// Eat other stuff
	fgetc(tmp.fp); fgetc(tmp.fp);
	fgetc(tmp.fp); fgetc(tmp.fp);
	fgetc(tmp.fp); fgetc(tmp.fp);

	// Create IMAGE object
    pImage = CreateImage((WORD)tmp.w, (WORD)tmp.h);
    if (!pImage) {
        fclose(tmp.fp);
        return NULL;
    }

	// Read in the image
    color.a = 0;
	for (j=0; j<tmp.h; j++) {
		i = 0;
		while (i < tmp.w) {
			count = fgetc(tmp.fp) & 0xff;
			color.b = (BYTE)(fgetc(tmp.fp) & 0xff);
			color.g = (BYTE)(fgetc(tmp.fp) & 0xff);
			color.r = (BYTE)(fgetc(tmp.fp) & 0xff);            
			while (count) {
                a_SetPixel(pImage, i, j, color);
                if (++i == tmp.w) break;
				count--;
			}
		}
	}

    // Clean up & return image
	fclose(tmp.fp);
    return pImage;
}
