/*
	pic.h

	Output Image file

*/

#ifndef PIC_DEFINED
#define PIC_DEFINED

#include <stdio.h>

// Picture file structure
typedef struct Pic {
	char*	filename;		// filename
	FILE*	filep;			// ptr to file stream
	int		x;				// image width
	int		y;				// image height
} Pic, *PPIC;

#endif // !PIC_DEFINED

