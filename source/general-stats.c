/*
	general-stats.c

	Statistics reporting

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "inc/bob.h"

void statistics(int line) {

	Cls();

	printf("\n\t\t%s\t\t       %s\n\t\t%s\n", _Program, _Version, _Copyright);

	printf("\ninput file \"%s\"  memory %u  resolution %1dx%1d  ", Infilename, MemAllocated, Xresolution, Yresolution);
	printf("line %1d\n", line);

	printf("total rays cast\t\t%8u\n", nRays);
	printf("\teye rays\t%8u\n", nRays-nReflected-nRefracted);
	printf("\treflected rays\t%8u\n", nReflected);
	printf("\trefracted rays\t%8u\n\n", nRefracted);

	printf("shadow rays\t\t%8u\n", nShadows);
	printf("cache hits\t\t%8u\n", nShadowCacheHits);
	
	if (nShadows>0) {
		printf("cache percent\t%16.6f\n\n", 100.0*(Flt)nShadowCacheHits/(Flt)nShadows);
	} 

	else {
		printf("cache percent\n\n");
	}

	printf("avg rays/pixel\t%16.6f\n", (Flt)nRays / ((Flt)(line+1-start_line)*(Flt)Xresolution));
    Flt count = (Flt)totalQueues;
    Flt total = (Flt) totalQueueResets;
    Flt avg = (total != 0) ? count/total : 0;
	printf("avg queues/ray\t%16.6f\n\n",  avg );

	printf("bounds checked\t\t%8u\n", nChecked);
	printf("queue inserts\t\t%8u\n", totalQueues);
	printf("queue resets\t\t%8u\n", totalQueueResets);
	printf("max queue size\t\t%8u\n", maxQueueSize);

	printf("\nmax recursion depth       %3d/%d\n\n", deepest+1, maxlevel);

	fflush(stdout);

}

