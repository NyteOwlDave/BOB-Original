
/*
	tracer-pic.c

	Output image file

*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "inc/bob.h"

#define TIME_OUT        (60)    // # seconds to trigger paranoid mode

static time_t old_time, new_time;

/*
    Open an .img file for o/p
*/

Pic* PicOpen(char* filename, int x, int y) {

	Pic	*tmp;
	int     line;           // line to start on
	int     i, c;

	time(&old_time);        // get current time

	tmp = (Pic *)malloc(sizeof(Pic));
	ptrchk(tmp, "Pic structure");
	tmp->filename = (char *)malloc(strlen(filename)+1);
	strcpy(tmp->filename, filename);
	tmp->x = x;
	tmp->y = y;

	if (resume) {            // finish a partial image
		// find line where interrupted
		line = start_line;
		if (((tmp->filep)=fopen(filename, "rb"))==NULL) {
			fprintf(stderr, "Error.  Trying to resume generation of %s.\n", filename);
			fprintf(stderr, "        Can't open %s for reading.\n", filename);
			last_call(1);
		}
		// skip header
		for (i=0; i<10; i++) {
			fgetc(tmp->filep);
		}
		i = 0;
		while (TRUE) {
			c = fgetc(tmp->filep);
			if (c == EOF) {
				break;
			}
			fgetc(tmp->filep);
			fgetc(tmp->filep);
			fgetc(tmp->filep);
			i += c;
			if (i >= x) {
				i = 0;
				line++;
			}
		}
		fclose(tmp->filep);

		// re-open and set to end
		if (((tmp->filep)=fopen(filename, "ab"))==NULL) {
			fprintf(stderr, "Error.  Trying to resume generation of %s.\n", filename);
			fprintf(stderr, "        Can't open %s for appending.\n", filename);
			last_call(1);
		}
		fseek(tmp->filep, 0L, SEEK_END);
		start_line = line;              // fake start line
	} else {                // start a new image
		if (((tmp->filep)=fopen(filename, "wb"))==NULL) {
			perror(filename);
			last_call(1);
		}

		fputc(x/256, tmp->filep);               // image size
		fputc(x%256, tmp->filep);
		fputc(y/256, tmp->filep);
		fputc(y%256, tmp->filep);
	
		fputc(start_line/256, tmp->filep);      // image range
		fputc(start_line%256, tmp->filep);
		fputc(stop_line/256, tmp->filep);
		fputc(stop_line%256, tmp->filep);

		fputc(    0, tmp->filep);
		fputc(   24, tmp->filep);               // # bitplanes
	}

	return (tmp);

}


/*
    Write a single line out to an .img file
*/

void PicWriteLine(Pic* pic, Pixel* buf) {

	int	i,		        // which pixel?
		total,		    // how many left in scan?
		count,		    // current run total
		or, og, ob,	    // current run color
		r, g, b;	    // next pixel color
	double  seconds;    // another helping?

	i = 0;
	total = pic->x;
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
		if (fputc(count, pic->filep) == EOF) {
			fprintf(stderr, "Error writing to disk.  May be out of space.\n");
			last_call(1);
		}
		fputc(ob, pic->filep);
		fputc(og, pic->filep);
		fputc(or, pic->filep);

		or = r;
		og = g;
		ob = b;

		if (total==1) {		// if at last pixel
			fputc(1, pic->filep);
			fputc(buf[pic->x-1].b, pic->filep);
			fputc(buf[pic->x-1].g, pic->filep);
			fputc(buf[pic->x-1].r, pic->filep);
			total--;
		}
	} while (total>0);
	fflush(pic->filep);

	// check time for paranoid mode
	time(&new_time);
	seconds = difftime(new_time, old_time);
	if (seconds > TIME_OUT) {
		old_time = new_time;
		// close, re-open, and set to end
		fclose(pic->filep);
		if (((pic->filep)=fopen(pic->filename, "ab"))==NULL) {
			fprintf(stderr, "Error opening %s for appending.\n", pic->filename);
			last_call(1);
		}
		fseek(pic->filep, 0L, SEEK_END);
	}

}


/*
    Close an .img file
*/

void PicClose(Pic* pic) {

	fclose(pic->filep);
}

