
/*
	shape-null.c

	Null procedures for composite objects

*/

#include <stdio.h>
#include <math.h>
#include "inc/bob.h"

/*
 * various routines to print error messages and die...
 */

ObjectProcs NullProcs = {
	NullIntersect,
	NullNormal
};

int NullIntersect(Object* obj, Ray* ray, Isect* hit) {
	fprintf(stderr, "Called non-existant intersect routine for bounding box, dying...\n");
	last_call(1);
	return 1;       /* keep lint/compilers quiet */
}

void NullNormal(Object* obj, Isect* hit, Point P, Vec N) {
	fprintf(stderr, "Called non-existant normal routine for bounding box, dying...\n");
	last_call(1);
}

