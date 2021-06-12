
/*
	tracer-screen.c

	Screen scanning and anti-aliasing

	[x] Done
*/

/*
	Screen

		Contains the scan functions which determine what
		antialiasing is done and the function shoot() which
		handle the projection mode, depth of field and
		actually calling the trace function.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "inc/bob.h"

static void	ScrInit(Viewpoint* view, int xres, int yres, char* picfile);
static void	Scan0(void);
static void	Scan1(void);
static void	Scan2(void);
static void	Scan3(void);
static void	Adapt(int i, int j, Flt x, Flt y, Color color, int step);
static void	Shoot(Flt x, Flt y, Color color);
static BOOL comp(unsigned int a, unsigned int b);

static	Flt		frustrumwidth, frustrumheight;
static  Point	viewvec, leftvec, looking_up, viewpoint;
static	int		x_res;
static	int		y_res;
static	Pic*	pic = NULL;
static	Ray		ray = { 0 };

void Screen(Viewpoint* view, char* picfile, int xres, int yres) {

    if (picfile[0]==0) {
        picfile = "blank.img";
    }

	ScrInit(view, xres, yres, picfile);

	switch (antialias) {
		case A_NONE:
            LogPrint("Scan0");
			Scan0();
			break;
		case A_CORNERS:
            LogPrint("Scan1");
			Scan1();
			break;
		case A_QUICK:
            LogPrint("Scan2");
			Scan2();
			break;
		case A_ADAPTIVE:
            LogPrint("Scan3");
			Scan3();
			break;
	}
	PicClose(pic);
}

static void	ScrInit(Viewpoint* view, int xres, int yres, char* picfile) {

	/*
	 * open the picture file...
	 */

	pic = PicOpen(picfile, xres, yres);

	/*
	 * determine the viewing frustrum
	 */

	x_res = xres;
	y_res = yres;

	VecNormalize(view->view_up);
	VecSub(view->view_at, view->view_from, viewvec);
	VecNormalize(viewvec);
	VecCross(view->view_up, viewvec, leftvec);
	VecNormalize(leftvec);
	VecS((-1), leftvec, leftvec);   // convert to right handed

	// make view_up vector perpendicular to veiwvec and leftvec

	if (camera.projection != P_NO_PARALLAX) {
		VecCross(leftvec, viewvec, view->view_up);
		VecNormalize(view->view_up);
	}
	VecCopy(view->view_up, looking_up);

	// set up camera stuff

	if (camera.aperture > 0.0) {	// calc lens vectors
		VecCopy(leftvec, camera.lens_i);
		VecCross(viewvec, camera.lens_i, camera.lens_j);
		VecNormalize(camera.lens_j);
		VecS(camera.aperture, camera.lens_i, camera.lens_i);
		VecS(camera.aperture, camera.lens_j, camera.lens_j);
	}

	VecCopy(view->view_from, ray.P);
	VecCopy(view->view_from, viewpoint);
	VecCopy(viewvec, ray.D);

	frustrumwidth  = tan(view->view_angle_x);
	frustrumheight = tan(view->view_angle_y);

}

/*
	NO ANTIALIAS MODE
	-----------------
	Scan0 -- Basic one ray per pixel, right through the center.
			 Jitter value +- 0.5
*/

static void	Scan0(void) {

	Pixel	*buf;
	Color	color;
	int     i, j;
	Flt     x, y;

	// - - - - - - - - - - - - - - - -
	// Global variables
	BOOL	use_jitter = jitter;
	int		yStart = start_line;
	int		yStop = stop_line;
	int		width = x_res;
	// - - - - - - - - - - - - - - - -

	buf = (Pixel *)malloc(x_res * sizeof(Pixel));

	for (j=yStart; j<yStop; j++) {

		for (i=0; i<width; i++) {

			if (use_jitter) {
				x = i + rnd();	// jitter mode
				y = j + rnd();
			} 
			
			else {
				x = i + 0.5;    // hit center of pixel
				y = j + 0.5;
			}

			Shoot(x, y, color);

			COLOR_TO_PIXEL(color, buf[i]);
		}

		PicWriteLine(pic, buf);
		
		if (tickflag)
			statistics(j);

	}

	if (tickflag)
		fprintf(stderr, "\n");

	free(buf);

}

/*
	CORNERS ANTIALIAS MODE
	----------------------
	Scan1 -- Shoot a ray at each corner of the pixel and average
			 the results.
			 Jitter value +- 0.5
*/

static void	Scan1(void) {

	Pixel   *buf, *oldbuf, *curbuf, *tmp;
	int	    red, green, blue;
	Color	color;		// color of current traced ray
	int     i, j;
	Flt     x, y;

	/*
	 * allocate enough space for an entire row of pixels...
	 * plus one more for the average...
	 */

	oldbuf = NULL;
	curbuf = (Pixel *)malloc ((x_res+1) * sizeof (Pixel));
	buf = (Pixel *)malloc ((x_res+1) * sizeof (Pixel));

	for (j=start_line; j<stop_line; j++) {
		for (i=0; i<x_res+1; i++) {

			if (jitter) {
				x = i + rnd();  // jitter mode
				y = j + rnd();
			} 
			else {
				x = i + 0.5;    // hit center of pixel
				y = j + 0.5;
			}

			Shoot(x, y, color);

			COLOR_TO_PIXEL(color, curbuf[i]);
		}
		if (oldbuf) {
			// average the pixels, and write 'em out
			for (i=0; i<x_res; i++) {
				red = ((int)curbuf[i].r + 
					   (int)curbuf[i+1].r  + 
					   (int)oldbuf[i].r + 
				       (int)oldbuf[i+1].r);
				green = ((int)curbuf[i].g + 
					     (int)curbuf[i+1].g + 
					     (int)oldbuf[i].g + 
					     (int)oldbuf[i+1].g);
				blue = ((int)curbuf[i].b + 
					    (int)curbuf[i+1].b + 
					    (int)oldbuf[i].b + 
					    (int)oldbuf[i+1].b);
				buf[i].r = (BYTE) (red / 4);
				buf[i].g = (BYTE) (green / 4);
				buf[i].b = (BYTE) (blue / 4);
			}
			PicWriteLine(pic, buf);
			tmp = oldbuf;
			oldbuf = curbuf;
			curbuf = tmp;
		} else {
			oldbuf = curbuf;
			curbuf = (Pixel *) malloc ((x_res + 1) * sizeof (Pixel));
		}

		if (tickflag)
			statistics(j);
	}
	if (tickflag)
		fprintf(stderr, "\n");

	free(buf);
	free(curbuf);
	if (oldbuf) free(oldbuf);
}

/*
	QUICK ANTIALIAS MODE
	--------------------
	Scan2 -- Undersampling, possible 18 to 1 speedup.
		No jitter option.

*/

static void	Scan2(void) {

	/*
	
		IMPORTANT!

		This algorithm is broken.
		I've outlined part of the problem in a commented area
		below.
	
		Need to analyse what it's trying to accomplish and how
		it SHOULD work, then fix it.

		Until then, we'll default to CORNERS mode instead.
	
	*/

	// Default to CORNERS mode (for now)
	Scan1();
	return;

	/*
	Color   color;
	Pixel*	buf[7];
	Pixel*	buff;
	int*	flags[7];
	int     x, y, xx, yy, i, j;
	DWORD	r, g, b;

	// Fill to next mod 6 scan line
	if(start_line%6) {
		buff = (Pixel *)malloc(x_res * sizeof(Pixel));
		ptrchk(buff, "pixel buffer");

		// Calc stop line for single fill
		yy = start_line + 6 - (start_line%6);
		for(j=start_line; j<yy; j++) {
			for(i=0; i<x_res; i++) {
				Shoot((Flt)i+0.5, (Flt)j+0.5, color);
				COLOR_TO_PIXEL(color, buff[i]);
			}
			PicWriteLine(pic, buff);
			if(tickflag)
				statistics(j);
		}
		start_line = yy;
		free(buff);
	}

	// Allocate pixel and flag buffers

	for(i=0; i<7; i++) {
		buf[i] = (Pixel *)malloc((x_res+5) * sizeof(Pixel));
		ptrchk(buf[i], "sampling buffer");
		flags[i] = (int *)malloc((x_res+5) * sizeof(int));
		ptrchk(flags[i], "sampling flag buffer");
	}

	// Start actual sub-sampling

	for(i=0; i<x_res+5; i++) {      // Clear bottom row of flags
		flags[6][i] = 0;
	}

	for(y=start_line; y<stop_line; y+=6) {

		// Copy bottom line to top
		for(i=0; i<x_res+5; i++) {
			if(flags[0][i] = flags[6][i]) { // only copy if valid
				buf[0][i].r = buf[6][i].r;
				buf[0][i].g = buf[6][i].g;
				buf[0][i].b = buf[6][i].b;
			}
			// Clear rest of buf
			for(j=1; j<7; j++) {
				flags[j][i] = 0;
			}
		}

		// for(x=0; x<x_res+5; x+=6) {
		for(x=0; x<x_res; x+=6) {
			// shoot corners and middle
			i = x; j = 0;
			if(!flags[j][i]) {
				flags[j][i] = 1;
				Shoot((Flt)i+0.5, (Flt)y+0.5, color);
				COLOR_TO_PIXEL(color, buf[j][i]);
			}

			i = x+6; j = 0;
			if(!flags[j][i]) {
				flags[j][i] = 1;
				Shoot((Flt)i+0.5, (Flt)y+0.5, color);
				COLOR_TO_PIXEL(color, buf[j][i]);
			}

			i = x; j = 6;
			if(!flags[j][i]) {
				flags[j][i] = 1;
				Shoot((Flt)i+0.5, (Flt)y+0.5+6, color);
				COLOR_TO_PIXEL(color, buf[j][i]);
			}

			i = x+6; j = 6;
			if(!flags[j][i]) {
				flags[j][i] = 1;
				Shoot((Flt)i+0.5, (Flt)y+0.5+6, color);
				COLOR_TO_PIXEL(color, buf[j][i]);
			}

			i = x+3; j = 3;         // middle ray
			if(!flags[j][i]) {
				flags[j][i] = 1;
				Shoot((Flt)i+0.5, (Flt)y+0.5+3, color);
				COLOR_TO_PIXEL(color, buf[j][i]);
			}

			// the corners are shot, now fill in if needed

			// check upper left quad first
			i = x+3; j = (y%6)+3;   // middle ray

			if(comp(buf[j][i].r, buf[j-3][i-3].r) &&
			   comp(buf[j][i].g, buf[j-3][i-3].g) &&
			   comp(buf[j][i].b, buf[j-3][i-3].b)) { // close enough so fill
				if(!flags[j-1][i]) {
					buf[j-1][i].r = buf[j][i].r;
					buf[j-1][i].g = buf[j][i].g;
					buf[j-1][i].b = buf[j][i].b;
				}
				if(!flags[j][i-1]) {
					buf[j][i-1].r = buf[j][i].r;
					buf[j][i-1].g = buf[j][i].g;
					buf[j][i-1].b = buf[j][i].b;
				}
				if(!flags[j-1][i-1]) {
					buf[j-1][i-1].r = buf[j][i].r;
					buf[j-1][i-1].g = buf[j][i].g;
					buf[j-1][i-1].b = buf[j][i].b;
				}
				if(!flags[j-2][i-2]) {
					buf[j-2][i-2].r = buf[j-3][i-3].r;
					buf[j-2][i-2].g = buf[j-3][i-3].g;
					buf[j-2][i-2].b = buf[j-3][i-3].b;
				}
				if(!flags[j-3][i-2]) {
					buf[j-3][i-2].r = buf[j-3][i-3].r;
					buf[j-3][i-2].g = buf[j-3][i-3].g;
					buf[j-3][i-2].b = buf[j-3][i-3].b;
				}
				if(!flags[j-2][i-3]) {
					buf[j-2][i-3].r = buf[j-3][i-3].r;
					buf[j-2][i-3].g = buf[j-3][i-3].g;
					buf[j-2][i-3].b = buf[j-3][i-3].b;
				}
				//==============================================================
				//==============================================================
				//==============================================================
				//
				// THIS SECTION DEMONSTRATES THE FLAW IN LOGIC
				//
				// NOTE HOW IT READS r, g, b TWICE WITHOUT SAVING!!!
				//
				r = ((DWORD)buf[j-3][i-3].r + (DWORD)buf[j-3][i+3].r) >> 1;
				g = ((DWORD)buf[j-3][i-3].g + (DWORD)buf[j-3][i+3].g) >> 1;
				b = ((DWORD)buf[j-3][i-3].b + (DWORD)buf[j-3][i+3].b) >> 1;
				r = buf[j-3][i-3].r;
				g = buf[j-3][i-3].g;
				b = buf[j-3][i-3].b;
				//==============================================================
				//==============================================================
				//==============================================================
				if(!flags[j-3][i-1]) {
					buf[j-3][i-1].r = r; 
					buf[j-3][i-1].g = g; 
					buf[j-3][i-1].b = b;
				}
				if(!flags[j-2][i-1]) {
					buf[j-2][i-1].r = r; 
					buf[j-2][i-1].g = g; 
					buf[j-2][i-1].b = b;
				}
				if(!flags[j-3][i]) {
					buf[j-3][i].r = r; buf[j-3][i].g = g; buf[j-3][i].b = b;
				}
				if(!flags[j-2][i]) {
					buf[j-2][i].r = r; buf[j-2][i].g = g; buf[j-2][i].b = b;
				}
				r = ((DWORD)buf[j-3][i-3].r + (DWORD)buf[j+3][i-3].r) >> 1;
				g = ((DWORD)buf[j-3][i-3].g + (DWORD)buf[j+3][i-3].g) >> 1;
				b = ((DWORD)buf[j-3][i-3].b + (DWORD)buf[j+3][i-3].b) >> 1;
				r = buf[j-3][i-3].r;
				g = buf[j-3][i-3].g;
				b = buf[j-3][i-3].b;
				if(!flags[j-1][i-3]) {
					buf[j-1][i-3].r = r; buf[j-1][i-3].g = g; buf[j-1][i-3].b = b;
				}
				if(!flags[j-1][i-2]) {
					buf[j-1][i-2].r = r; buf[j-1][i-2].g = g; buf[j-1][i-2].b = b;
				}
				if(!flags[j][i-3]) {
					buf[j][i-3].r = r; buf[j][i-3].g = g; buf[j][i-3].b = b;
				}
				if(!flags[j-1][i-2]) {
					buf[j][i-2].r = r; buf[j][i-2].g = g; buf[j][i-2].b = b;
				}
			} else {        // else have to calc upper-left quad
				for(i=x; i<x+4; i++) {
					for(j=0; j<4; j++) {
						if(!flags[j][i]) {
							flags[j][i] = 1;
							Shoot((Flt)i+0.5, (Flt)y+0.5+j, color);
							COLOR_TO_PIXEL(color, buf[j][i]);
						}
					}
				}
			}

			// check upper right quad
			i = x+3; j = (y%6)+3;   // middle ray

			if(comp(buf[j][i].r, buf[j-3][i+3].r) &&
			   comp(buf[j][i].g, buf[j-3][i+3].g) &&
			   comp(buf[j][i].b, buf[j-3][i+3].b)) { // close enough so fill
				if(!flags[j][i+1]) {
					buf[j][i+1].r = buf[j][i].r;
					buf[j][i+1].g = buf[j][i].g;
					buf[j][i+1].b = buf[j][i].b;
				}
				if(!flags[j-1][i+1]) {
					buf[j-1][i+1].r = buf[j][i].r;
					buf[j-1][i+1].g = buf[j][i].g;
					buf[j-1][i+1].b = buf[j][i].b;
				}
				if(!flags[j-2][i+2]) {
					buf[j-2][i+2].r = buf[j-3][i+3].r;
					buf[j-2][i+2].g = buf[j-3][i+3].g;
					buf[j-2][i+2].b = buf[j-3][i+3].b;
				}
				if(!flags[j-3][i+2]) {
					buf[j-3][i+2].r = buf[j-3][i+3].r;
					buf[j-3][i+2].g = buf[j-3][i+3].g;
					buf[j-3][i+2].b = buf[j-3][i+3].b;
				}
				if(!flags[j-2][i+3]) {
					buf[j-2][i+3].r = buf[j-3][i+3].r;
					buf[j-2][i+3].g = buf[j-3][i+3].g;
					buf[j-2][i+3].b = buf[j-3][i+3].b;
				}
				r = ((DWORD)buf[j-3][i-3].r + (DWORD)buf[j-3][i+3].r) >> 1;
				g = ((DWORD)buf[j-3][i-3].g + (DWORD)buf[j-3][i+3].g) >> 1;
				b = ((DWORD)buf[j-3][i-3].b + (DWORD)buf[j-3][i+3].b) >> 1;
				r = buf[j-3][i+3].r;
				g = buf[j-3][i+3].g;
				b = buf[j-3][i+3].b;
				if(!flags[j-3][i+1]) {
					buf[j-3][i+1].r = r; buf[j-3][i+1].g = g; buf[j-3][i+1].b = b;
				}
				if(!flags[j-2][i+1]) {
					buf[j-2][i+1].r = r; buf[j-2][i+1].g = g; buf[j-2][i+1].b = b;
				}
				r = ((DWORD)buf[j-3][i+3].r + (DWORD)buf[j+3][i+3].r) >> 1;
				g = ((DWORD)buf[j-3][i+3].g + (DWORD)buf[j+3][i+3].g) >> 1;
				b = ((DWORD)buf[j-3][i+3].b + (DWORD)buf[j+3][i+3].b) >> 1;
				r = buf[j-3][i+3].r;
				g = buf[j-3][i+3].g;
				b = buf[j-3][i+3].b;
				if(!flags[j-1][i+3]) {
					buf[j-1][i+3].r = r; buf[j-1][i+3].g = g; buf[j-1][i+3].b = b;
				}
				if(!flags[j-1][i+2]) {
					buf[j-1][i+2].r = r; buf[j-1][i+2].g = g; buf[j-1][i+2].b = b;
				}
				if(!flags[j][i+3]) {
					buf[j][i+3].r = r; buf[j][i+3].g = g; buf[j][i+3].b = b;
				}
				if(!flags[j-1][i+2]) {
					buf[j][i+2].r = r; buf[j][i+2].g = g; buf[j][i+2].b = b;
				}
			} else {        // else have to calc upper-right quad
				for(i=x+3; i<x+7; i++) {
					for(j=0; j<4; j++) {
						if(!flags[j][i]) {
							flags[j][i] = 1;
							Shoot((Flt)i+0.5, (Flt)y+0.5+j, color);
							COLOR_TO_PIXEL(color, buf[j][i]);
						}
					}
				}
			}

			// handle lower left quad third
			i = x+3; j = (y%6)+3;   // middle ray

			if(comp(buf[j][i].r, buf[j+3][i-3].r) &&
			   comp(buf[j][i].g, buf[j+3][i-3].g) &&
			   comp(buf[j][i].b, buf[j+3][i-3].b)) { // close enough so fill
				if(!flags[j+1][i]) {
					buf[j+1][i].r = buf[j][i].r;
					buf[j+1][i].g = buf[j][i].g;
					buf[j+1][i].b = buf[j][i].b;
				}
				if(!flags[j+1][i-1]) {
					buf[j+1][i-1].r = buf[j][i].r;
					buf[j+1][i-1].g = buf[j][i].g;
					buf[j+1][i-1].b = buf[j][i].b;
				}
				if(!flags[j+2][i-2]) {
					buf[j+2][i-2].r = buf[j+3][i-3].r;
					buf[j+2][i-2].g = buf[j+3][i-3].g;
					buf[j+2][i-2].b = buf[j+3][i-3].b;
				}
				if(!flags[j+3][i-2]) {
					buf[j+3][i-2].r = buf[j+3][i-3].r;
					buf[j+3][i-2].g = buf[j+3][i-3].g;
					buf[j+3][i-2].b = buf[j+3][i-3].b;
				}
				if(!flags[j+2][i-3]) {
					buf[j+2][i-3].r = buf[j+3][i-3].r;
					buf[j+2][i-3].g = buf[j+3][i-3].g;
					buf[j+2][i-3].b = buf[j+3][i-3].b;
				}
				r = ((DWORD)buf[j+3][i-3].r + (DWORD)buf[j+3][i+3].r) >> 1;
				g = ((DWORD)buf[j+3][i-3].g + (DWORD)buf[j+3][i+3].g) >> 1;
				b = ((DWORD)buf[j+3][i-3].b + (DWORD)buf[j+3][i+3].b) >> 1;
				r = buf[j+3][i-3].r;
				g = buf[j+3][i-3].g;
				b = buf[j+3][i-3].b;
				if(!flags[j+3][i-1]) {
					buf[j+3][i-1].r = r; buf[j+3][i-1].g = g; buf[j+3][i-1].b = b;
				}
				if(!flags[j+2][i-1]) {
					buf[j+2][i-1].r = r; buf[j+2][i-1].g = g; buf[j+2][i-1].b = b;
				}
				if(!flags[j+3][i]) {
					buf[j+3][i].r = r; buf[j+3][i].g = g; buf[j+3][i].b = b;
				}
				if(!flags[j+2][i]) {
					buf[j+2][i].r = r; buf[j+2][i].g = g; buf[j+2][i].b = b;
				}
				r = ((DWORD)buf[j-3][i-3].r + (DWORD)buf[j+3][i-3].r) >> 1;
				g = ((DWORD)buf[j-3][i-3].g + (DWORD)buf[j+3][i-3].g) >> 1;
				b = ((DWORD)buf[j-3][i-3].b + (DWORD)buf[j+3][i-3].b) >> 1;
				r = buf[j+3][i-3].r;
				g = buf[j+3][i-3].g;
				b = buf[j+3][i-3].b;
				if(!flags[j+1][i-3]) {
					buf[j+1][i-3].r = r; buf[j+1][i-3].g = g; buf[j+1][i-3].b = b;
				}
				if(!flags[j+1][i-2]) {
					buf[j+1][i-2].r = r; buf[j+1][i-2].g = g; buf[j+1][i-2].b = b;
				}
			} 
			else {        // else have to calc lower-left quad
				for(i=x; i<x+4; i++) {
					for(j=3; j<7; j++) {
						if(!flags[j][i]) {
							flags[j][i] = 1;
							Shoot((Flt)i+0.5, (Flt)y+0.5+j, color);
							COLOR_TO_PIXEL(color, buf[j][i]);
						}
					}
				}
			}

			// finally finish with lower right quad
			i = x+3; j = (y%6)+3;   // middle ray

			if(comp(buf[j][i].r, buf[j+3][i+3].r) &&
			   comp(buf[j][i].g, buf[j+3][i+3].g) &&
			   comp(buf[j][i].b, buf[j+3][i+3].b)) { // close enough so fill
				if(!flags[j+1][i+1]) {
					buf[j+1][i+1].r = buf[j][i].r;
					buf[j+1][i+1].g = buf[j][i].g;
					buf[j+1][i+1].b = buf[j][i].b;
				}
				if(!flags[j+2][i+2]) {
					buf[j+2][i+2].r = buf[j+3][i+3].r;
					buf[j+2][i+2].g = buf[j+3][i+3].g;
					buf[j+2][i+2].b = buf[j+3][i+3].b;
				}
				if(!flags[j+3][i+2]) {
					buf[j+3][i+2].r = buf[j+3][i+3].r;
					buf[j+3][i+2].g = buf[j+3][i+3].g;
					buf[j+3][i+2].b = buf[j+3][i+3].b;
				}
				if(!flags[j+2][i-3]) {
					buf[j+2][i+3].r = buf[j+3][i+3].r;
					buf[j+2][i+3].g = buf[j+3][i+3].g;
					buf[j+2][i+3].b = buf[j+3][i+3].b;
				}
				r = ((DWORD)buf[j+3][i-3].r + (DWORD)buf[j+3][i+3].r) >> 1;
				g = ((DWORD)buf[j+3][i-3].g + (DWORD)buf[j+3][i+3].g) >> 1;
				b = ((DWORD)buf[j+3][i-3].b + (DWORD)buf[j+3][i+3].b) >> 1;
				r = buf[j+3][i+3].r;
				g = buf[j+3][i+3].g;
				b = buf[j+3][i+3].b;
				if(!flags[j+3][i+1]) {
					buf[j+3][i+1].r = r; buf[j+3][i+1].g = g; buf[j+3][i+1].b = b;
				}
				if(!flags[j+2][i+1]) {
					buf[j+2][i+1].r = r; buf[j+2][i+1].g = g; buf[j+2][i+1].b = b;
				}
				r = ((DWORD)buf[j-3][i+3].r + (DWORD)buf[j+3][i+3].r) >> 1;
				g = ((DWORD)buf[j-3][i+3].g + (DWORD)buf[j+3][i+3].g) >> 1;
				b = ((DWORD)buf[j-3][i+3].b + (DWORD)buf[j+3][i+3].b) >> 1;
				r = buf[j+3][i+3].r;
				g = buf[j+3][i+3].g;
				b = buf[j+3][i+3].b;
				if(!flags[j+1][i+3]) {
					buf[j+1][i+3].r = r; buf[j+1][i+3].g = g; buf[j+1][i+3].b = b;
				}
				if(!flags[j+1][i+2]) {
					buf[j+1][i+2].r = r; buf[j+1][i+2].g = g; buf[j+1][i+2].b = b;
				}
			} 
			else {        // else have to calc lower-right quad
				for(i=x+3; i<x+7; i++) {
					for(j=3; j<7; j++) {
						if(!flags[j][i]) {
							flags[j][i] = 1;
							Shoot((Flt)i+0.5, (Flt)y+0.5+j, color);
							COLOR_TO_PIXEL(color, buf[j][i]);
						}
					}
				}
			}
		}

		// output scans
		for(j=0; j<6; j++) {
			PicWriteLine(pic, buf[j]);
		}

		if(tickflag)
			statistics(y+6);

	}

	if(tickflag)
		fprintf(stderr, "\n");

	*/

	//=============================
	//=============================
	// TODO: Free ALL buffers here!
	//=============================
	//=============================
}

/*
	comp - compares two numbers, returns TRUE if close enough
*/

static BOOL comp(unsigned int a, unsigned int b) {

	int diff;

	diff = a - b;
	if (diff > adapt_dist)
		return FALSE;
	else if (diff < -adapt_dist)
		return FALSE;
	else
		return TRUE;

}

/*
	ADAPTIVE ANTIALIAS MODE
	-----------------------
	Scan3 
		Adaptive supersampling with optional jitter.
		Jitter +- 0.125
*/

#define SIDE			(4)

#define RAW				(0)
#define COOKED          (1)
#define FUZZY           (2)

// Used by Adapt() and Scan3()
static BYTE		win[SIDE + 1][SIDE + 1][4];		// r,g,b,flag
static BYTE*	buff[4] = { NULL, NULL, NULL, NULL };

static void	Scan3(void) {

	Pixel*	buf;

	buf = (Pixel *)malloc(x_res * sizeof(Pixel));
	ptrchk(buf, "output buffer.");

	for (int i=0; i<4; i++) {
		buff[i] = (unsigned char *)malloc(SIDE*x_res+1);
		ptrchk(buff[i], "antialiasing buffer.");
	}

	// clear win flags
	for (int i=0; i<SIDE+1; i++)	{
		for (int j=0; j<SIDE+1; j++) {
			win[i][j][3] = RAW;
		}
	}

	// clear buff flags
	for (int i=0; i<SIDE*x_res+1; i++) {
		buff[3][i] = RAW;
	}

	for (int y=start_line; y<stop_line; y++) {

		Color	color;
							
		// clear left edge of win for starting a new row
		for (int j=0; j<SIDE+1; j++) {
			win[0][j][3] = RAW;
		}

		for (int x = 0; x < x_res; x++) {

			for (int i = 1; i < SIDE + 1; i++) {	// buff to top row of win
				if (win[i][0][3] = buff[3][x*SIDE + i]) {	// if cooked
					win[i][0][0] = buff[0][x*SIDE + i];
					win[i][0][1] = buff[1][x*SIDE + i];
					win[i][0][2] = buff[2][x*SIDE + i];
				}
			}

			for (int i = 1; i < SIDE + 1; i++) {	// clear rest of win
				for (int j = 1; j < SIDE + 1; j++) {
					win[i][j][3] = RAW;
				}
			}

			Adapt(0, 0, (Flt)x, (Flt)y, color, SIDE);

			buf[x].r = (BYTE) (color[0]);
			buf[x].g = (BYTE) (color[1]);
			buf[x].b = (BYTE) (color[2]);

			for (int i=0; i<SIDE+1; i++)	// bottom row of win to buff
				if (buff[3][x*SIDE+i] = win[i][SIDE][3]) {	// if cooked
					buff[0][x*SIDE+i] = win[i][SIDE][0];
					buff[1][x*SIDE+i] = win[i][SIDE][1];
					buff[2][x*SIDE+i] = win[i][SIDE][2];
				}

			for (int j=0; j<SIDE+1; j++)	{	// right edge of win to left
				if (win[0][j][3] = win[SIDE][j][3]) {	// if cooked
					win[0][j][0] = win[SIDE][j][0];
					win[0][j][1] = win[SIDE][j][1];
					win[0][j][2] = win[SIDE][j][2];
				}
			}
		}

		PicWriteLine(pic, buf);

		if(tickflag)
			statistics(y);
	}

	if(tickflag)
		fprintf(stderr, "\n");

	free(buf);

	for (int i = 0; i < 4; i++) {
		if (buff[i]) {
			free(buff[i]);
			buff[i] = NULL;
		}
	}

}

//
// Adapt() - helper for Scan3()
//
// i, j		where in win to put results
// x, y		upper left hand of pixel
// color	return pixel color here in [0 .. 255] range
// step		what level we're at
//
static void	Adapt(int i, int j, Flt x, Flt y, Color color, int step) {

#define ARAND() ((rnd()*0.25)-0.125)

	int     k, fuzzed;
	int     ave[3], c0[3], c1[3], c2[3], c3[3];

	if (win[i][j][3] == RAW) {
		if (jitter) {
			Shoot(x+(Flt)i/SIDE+ARAND(), y+(Flt)j/SIDE+ARAND(), color);
		} else {
			Shoot(x+(Flt)i/SIDE, y+(Flt)j/SIDE, color);
		}

		c0[0] = win[i][j][0] = (BYTE) (255.0 * color[0]);
		c0[1] = win[i][j][1] = (BYTE) (255.0 * color[1]);
		c0[2] = win[i][j][2] = (BYTE) (255.0 * color[2]);
		if (fuzzy_ray) {
			win[i][j][3] = COOKED | FUZZY;
		} else {
			win[i][j][3] = COOKED;
		}
	} else {
		c0[0] = win[i][j][0];
		c0[1] = win[i][j][1];
		c0[2] = win[i][j][2];
	}

	if (win[i+step][j][3] == RAW) {
		if (jitter) {
			Shoot((Flt)x+(Flt)(i+step)/SIDE+ARAND(), (Flt)y+(Flt)j/SIDE+ARAND(), color);
		} else {
			Shoot((Flt)x+(Flt)(i+step)/SIDE, (Flt)y+(Flt)j/SIDE, color);
		}

		c1[0] = win[i+step][j][0] = (BYTE) (255.0 * color[0]);
		c1[1] = win[i+step][j][1] = (BYTE) (255.0 * color[1]);
		c1[2] = win[i+step][j][2] = (BYTE) (255.0 * color[2]);
		if (fuzzy_ray) {
			win[i+step][j][3] = COOKED | FUZZY;
		} else {
			win[i+step][j][3] = COOKED;
		}
	} else {
		c1[0] = win[i+step][j][0];
		c1[1] = win[i+step][j][1];
		c1[2] = win[i+step][j][2];
	}

	if (win[i][j+step][3] == RAW) {
		if (jitter) {
			Shoot((Flt)x+(Flt)i/SIDE+ARAND(), (Flt)y+(Flt)(j+step)/SIDE+ARAND(), color);
		} else {
			Shoot((Flt)x+(Flt)i/SIDE, (Flt)y+(Flt)(j+step)/SIDE, color);
		}

		c2[0] = win[i][j+step][0] = (BYTE) (255.0 * color[0]);
		c2[1] = win[i][j+step][1] = (BYTE) (255.0 * color[1]);
		c2[2] = win[i][j+step][2] = (BYTE) (255.0 * color[2]);
		if (fuzzy_ray) {
			win[i][j+step][3] = COOKED | FUZZY;
		} else {
			win[i][j+step][3] = COOKED;
		}
	} else {
		c2[0] = win[i][j+step][0];
		c2[1] = win[i][j+step][1];
		c2[2] = win[i][j+step][2];
	}

	if (win[i+step][j+step][3] == RAW) {
		if (jitter) {
			Shoot((Flt)x+(Flt)(i+step)/SIDE+ARAND(), (Flt)y+(Flt)(j+step)/SIDE+ARAND(), color);
		} else {
			Shoot((Flt)x+(Flt)(i+step)/SIDE, (Flt)y+(Flt)(j+step)/SIDE, color);
		}

		c3[0] = win[i+step][j+step][0] = (BYTE) (255.0 * color[0]);
		c3[1] = win[i+step][j+step][1] = (BYTE) (255.0 * color[1]);
		c3[2] = win[i+step][j+step][2] = (BYTE) (255.0 * color[2]);
		if(fuzzy_ray) {
			win[i+step][j+step][3] = COOKED | FUZZY;
		} else {
			win[i+step][j+step][3] = COOKED;
		}
	} else {
		c3[0] = win[i+step][j+step][0];
		c3[1] = win[i+step][j+step][1];
		c3[2] = win[i+step][j+step][2];
	}

	for(k=0; k<3; k++) {
		ave[k] = c0[k] + c1[k] + c2[k] + c3[k];
		ave[k] /= 4;
	}

	fuzzed = win[i][j][3] &
		win[i+step][j][3] &
		win[i][j+step][3] &
		win[i+step][j+step][3] &
		FUZZY;

	if (step==1 || fuzzed ||
		ABS(ave[0] - c0[0]) < adapt_dist &&
		ABS(ave[1] - c0[1]) < adapt_dist &&
		ABS(ave[2] - c0[2]) < adapt_dist &&
		ABS(ave[0] - c1[0]) < adapt_dist &&
		ABS(ave[1] - c1[1]) < adapt_dist &&
		ABS(ave[2] - c1[2]) < adapt_dist &&
		ABS(ave[0] - c2[0]) < adapt_dist &&
		ABS(ave[1] - c2[1]) < adapt_dist &&
		ABS(ave[2] - c2[2]) < adapt_dist &&
		ABS(ave[0] - c3[0]) < adapt_dist &&
		ABS(ave[1] - c3[1]) < adapt_dist &&
		ABS(ave[2] - c3[2]) < adapt_dist) {     // close enough
		color[0] = ave[0];
		color[1] = ave[1];
		color[2] = ave[2];

		return;
	}

	// not close, so we have to subdivide

	step /= 2;
	ave[0] = 0;
	ave[1] = 0;
	ave[2] = 0;

	Adapt(i, j, (Flt)x, (Flt)y, color, step);
	ave[0] += (BYTE) (color[0]);
	ave[1] += (BYTE) (color[1]);
	ave[2] += (BYTE) (color[2]);

	Adapt(i+step, j, (Flt)x, (Flt)y, color, step);
	ave[0] += (BYTE) (color[0]);
	ave[1] += (BYTE) (color[1]);
	ave[2] += (BYTE) (color[2]);

	Adapt(i, j+step, (Flt)x, (Flt)y, color, step);
	ave[0] += (BYTE) (color[0]);
	ave[1] += (BYTE) (color[1]);
	ave[2] += (BYTE) (color[2]);

	Adapt(i+step, j+step, (Flt)x, (Flt)y, color, step);
	ave[0] += (BYTE) (color[0]);
	ave[1] += (BYTE) (color[1]);
	ave[2] += (BYTE) (color[2]);

	// Return average color
	color[0] = ave[0]>>2;
	color[1] = ave[1]>>2;
	color[2] = ave[2]>>2;

	return;

}

//
// Shoot ray out into space
//
// x, y		pixel location
// color	resulting pixel color
//
static void	Shoot(Flt x, Flt y, Color color) {

	Vec		dir;

	// Deal with projection mode
	switch (camera.projection) {
		case P_FLAT:
		case P_NO_PARALLAX:
		{
			// No perspective correction
			// Updates: ray.D, dir
			VecComb(-frustrumheight*(2.0*y / (Flt)y_res - 1.0),
				looking_up,
				frustrumwidth*(2.0*x / (Flt)x_res - 1.0),
				leftvec, dir);
			VecAdd(dir, viewvec, ray.D);
			VecNormalize(ray.D);
			break;
		}
		case P_FISHEYE:
		{
			// Weird roundy stuff
			// Updates: ray.D, dir
			Flt tx = (x - x_res / 2.0) / (Flt)x_res*Eye.view_angle_x*2.0;
			Flt ty = -(y - y_res / 2.0) / (Flt)y_res*Eye.view_angle_y*2.0;
			VecComb(sin(ty), looking_up, sin(tx), leftvec, dir);
			VecAddS(cos(tx)*cos(ty), viewvec, dir, ray.D);
			VecNormalize(ray.D);
			break;
		}
		case P_ORTHOGRAPHIC:
		{
			// Good ole perspective correction
			// Weird roundy stuff
			// Updates: ray.P, dir
			VecComb(-Eye.view_angle_y*(2.0*y / (Flt)y_res - 1.0),
				looking_up,
				Eye.view_angle_x*(2.0*x / (Flt)x_res - 1.0),
				leftvec, dir);
			VecAdd(dir, viewpoint, ray.P);
			break;
		}
		default:
		{
			fprintf(stderr, "Unknown camera projection mode %u, dying...\n", camera.projection);
			last_call(1);
		}
	}

	// WHY IS THIS HERE ?!?
	fuzzy_ray = 0;

	// Non-pinhole lens (multiple samples)
	if(camera.aperture > 0.0) {

		// Random [-1.0 .. +1.0]
#define RAND2() (2*rnd()-1)

		int		sample;
		Color   sum_color;
		Flt		random;
		Ray		ray2;
		
		// Clear summed color
		MakeVector(0, 0, 0, sum_color);

		for (sample=0; sample<camera.samples; sample++) {

			// Project ray by focal length
			VecAddS(camera.focal_length, ray.D, ray.P, dir);
			
			// Copy to secondary ray
			VecCopy(ray.P, ray2.P);
			
			// Jitter along lens horizontal vector
			random = RAND2();
			VecAddS(random, camera.lens_i, ray2.P, ray2.P);

			// Jitter along lens vertical vector
			random = RAND2();
			VecAddS(random, camera.lens_j, ray2.P, ray2.P);
			
			// Make relative to original direction
			VecSub(dir, ray2.P, ray2.D);
			VecNormalize(ray2.D);

			// Go trace that ray
			Trace(0, 1.0, &ray2, color, 1.0, NULL);

			// Clamp color to legal range
			if (color[0] > 1.0) color[0] = 1.0;
			if (color[1] > 1.0) color[1] = 1.0;
			if (color[2] > 1.0) color[2] = 1.0;

			// Add to summed color
			VecAdd(color, sum_color, sum_color);

		}

		// Average samples
		VecS((1.0/(Flt)camera.samples), sum_color, color);
	} 
	
	// Pinhole lens (single sample)
	else {
		// Go trace that ray
		Trace(0, 1.0, &ray, color, 1.0, NULL);
	}

	// Clamp color to legal range
	if (color[0] > 1.0) color[0] = 1.0;
	if (color[1] > 1.0) color[1] = 1.0;
	if (color[2] > 1.0) color[2] = 1.0;
}

