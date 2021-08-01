/*
	csg-clip.c

	Clipping routines

*/

#include <stdio.h>
#include <math.h>
#include "inc/bob.h"

/*
	clip_check() -- check a point against a list of clips.
		Returns 1 if passes, 0 if fails.
*/

BOOL clip_check(Clip* head, Vec P) {

	Vec     V;
	Flt     dist;

	while (head) {
		VecSub(P, head->center, V);
		if (head->type & C_PLANE) {
			if (VecDot(V, head->normal) < 0.0)
				return FALSE;
		} else if (head->type & C_SPHERE) {
			dist = V[0]*V[0] + V[1]*V[1] + V[2]*V[2];
			if (head->type & C_INSIDE) {
				if (dist > head->radius1) {
					return FALSE;
				}
			} else if (dist < head->radius1) {   // must be outside sphere
				return FALSE;
			}
		} else if (head->type & C_CONE) {
			Vec     ap;
			Flt     ap_dot, percent, radius, dist;

			VecSub(P, head->apex, ap);
			ap_dot = VecDot(ap, head->normal);
			if (ap_dot<0.0 || ap_dot>head->length) {
				if (head->type & C_INSIDE) {
					return FALSE;
				}
			} else {        // on "inside" of ends
				percent = ap_dot/head->length;
				radius = percent*head->radius2 + (1.0-percent)*head->radius1;
				radius = radius * radius;
				VecAddS(ap_dot, head->normal, head->apex, ap);
				dist = (ap[0]-P[0]) * (ap[0]-P[0]) +
				       (ap[1]-P[1]) * (ap[1]-P[1]) +
				       (ap[2]-P[2]) * (ap[2]-P[2]);
				if (head->type & C_INSIDE) {
					if (dist>radius) {
						return FALSE;
					}
				} else {
					if (dist<radius) {
						return FALSE;
					}
				}
			}       // if inside of ends


		} else {
			fprintf(stderr, "\nInternal error.  Unknown clip type %04x.\n", head->type);
			last_call(1);
		}

		head = head->next;      // move on down
	}

	// Point is visible
	return TRUE;
}


/*
	bound_opt -- optimize the bounding box of an object which has
		one or more clips.  Only works for planes and inside
		spheres.
*/

void bound_opt(Object* obj) {

	Clip            *cl;
	int             i, i1, i2;
	Flt             intersect,
			b1, b2,         // values of box corner
			c1, c2,         // values of clip center for "other" axes
			d1, d2;         // values of clip normal for "other" axes

    // For each clipping object
	cl = obj->clips;
	while (cl) {
		
        // Inside of a sphere
		if ((cl->type&C_SPHERE) && (cl->type&C_INSIDE)) {
			Flt     radius;

			radius = sqrt(cl->radius1);
			for (i=0; i<3; i++) {    // for each axis
				if (obj->o_dmax[i]  > cl->center[i]+radius) {
					obj->o_dmax[i] = cl->center[i]+radius;
				}
				if (obj->o_dmin[i]  > cl->center[i]-radius) {
					obj->o_dmin[i] = cl->center[i]-radius;
				}
			}
		} 
		
        // Inside of a cone
		if ((cl->type&C_CONE) && (cl->type&C_INSIDE)) {
			for (i=0; i<3; i++) {    // for each axis
				if (cl->apex[i] > cl->base[i]) {
					if (obj->o_dmax[i] > cl->apex[i]+cl->radius1) {
						obj->o_dmax[i] = cl->apex[i]+cl->radius1;
					}
					if (obj->o_dmin[i] > cl->base[i]-cl->radius2) {
						obj->o_dmin[i] = cl->base[i]-cl->radius2;
					}
				} else {
					if (obj->o_dmax[i] > cl->base[i]+cl->radius2) {
						obj->o_dmax[i] = cl->base[i]+cl->radius2;
					}
					if (obj->o_dmin[i] > cl->apex[i]-cl->radius1) {
						obj->o_dmin[i] = cl->apex[i]-cl->radius1;
					}
				}
			}
		} 
		
        // Clip plane
		else if (cl->type & C_PLANE) {
			for (i=0; i<3; i++) {             // for each axis
				i1 = (i+1)%3;
				i2 = (i+2)%3;
				if (cl->normal[i] == 0.0) {   // clip perpendicular to axis, nothing happens
					continue;
				}

				// get clip center values
				c1 = cl->center[i1];
				c2 = cl->center[i2];

				// get clip normal values
				d1 = cl->normal[i1];
				d2 = cl->normal[i2];

				// get b's
				b1 = (d1 > 0.0) ? obj->o_dmax[i1] : obj->o_dmin[i1];
				b2 = (d2 > 0.0) ? obj->o_dmax[i2] : obj->o_dmin[i2];

				intersect = cl->center[i];
				intersect += (c1-b1)*d1/cl->normal[i];
				intersect += (c2-b2)*d2/cl->normal[i];

				if (cl->normal[i] > 0.0) {      // shrink min
					if(obj->o_dmin[i] < intersect) {
						obj->o_dmin[i] = intersect;
					}
				} else {                        // shrink max
					if (obj->o_dmax[i] > intersect) {
						obj->o_dmax[i] = intersect;
					}
				}
			}       // end of i loop for each axis
		}       // end of if clipping plane

		cl = cl->next;
	}       // end of while loop
}


