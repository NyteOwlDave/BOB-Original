
/*

	tracer.-trace.c

	Shoots a ray out into the scene to hopefully add color

*/

#include <stdio.h>
#include <math.h>
#include "inc/bob.h"

/*
	Trace returns the distance to the next hit.
	This gives shade() a value to use for determining color
	attenuation for haze and transparent objects.
*/

Flt Trace(int level, Flt weight, Ray* ray, Color color, Flt ior, Object* self) {

	Object	*prim;
	Vec	P, N;
	Isect	hit;

	if (level >= maxlevel) {
		color[0] = color[1] = color[2] = 0.0;
		return 0.0;
	} else if (level > deepest) {
		deepest = level;
	}
		
	nRays ++;

	if (Intersect(ray, &hit, HUGE, self)) {

		// end of warning

		prim = hit.isect_prim;
		RayPoint(ray, hit.isect_t, P);
		// get normal vector of intersection
		(*prim -> o_procs -> normal) (prim, &hit, P, N);

		Shade(level, weight, P, N, ray->D, &hit, color, ior);
		return hit.isect_t;
	} 
	
	else {
		bkg(ray->D, color);
		return HUGE;
	}

}

/*
    Deal with rays that hit nothing
*/

void bkg(Vec dir, Color col) {

	Flt     dot, index;
	int     indx;

	if (background.color[0] < 0.0) {
	
		dot = -VecDot(dir, background.up);
		
		index = 127.0 * dot + 128.0;
		indx = (int)index;
		index -= indx;
		
		col[0] = 
			(1.0-index)*background.pal[indx][0]/256.0 + 
			index*background.pal[indx+1][0]/256.0;
		
		col[1] = 
			(1.0-index)*background.pal[indx][1]/256.0 + 
			index*background.pal[indx+1][1]/256.0;
		
		col[2] = 
			(1.0-index)*background.pal[indx][2]/256.0 + 
			index*background.pal[indx+1][2]/256.0;

	} 
	
	else {
		VecCopy(background.color, col);
	}
}

