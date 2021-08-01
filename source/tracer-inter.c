
/*
	tracer-inter.c

	Ray intersection determination

	These routines try to find the nearest intersection.

*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "inc/bob.h"

static Flt	num[NSLABS];
static Flt	den[NSLABS];

//
// CheckAndEnqueue(obj, maxdist)
// Check the current ray (as paramaterized with the num and den 
// arrays above) against the bounding volume of obj.
// If we intersect the bounding volume, then insert it into the 
// priority queue.
//
void CheckAndEnqueue(Object* obj, Flt maxdist) {

	Flt	tmin, tmax;
	Flt	dmin = -HUGE;
	Flt	dmax = maxdist;

	nChecked++;

	for (int i=0; i<NSLABS; i++) {

		if (den[i] == 0.0)
			continue;

		// enters the slab here...
		tmin = (obj->o_dmin[i] - num[i]) / den[i];
		// and exits here...
		tmax = (obj->o_dmax[i] - num[i]) / den[i];

		// but we may have to swap...
		if (tmin < tmax) {
			// if exited closer than we thought, update
			if (tmax < dmax)
				dmax = tmax;
			// if entered farther than we thought, update
			if (tmin > dmin)
				dmin = tmin;
		} else {
			// if exited closer than we thought, update
			if (tmin < dmax)
				dmax = tmin;
			// if entered farther than we thought, update
			if (tmax > dmin)
				dmin = tmax;
		}

		if (dmin>dmax || dmax<rayeps)
			return;
	}

	PriorityQueueInsert(dmin, obj);
	nEnqueued++;

}

// 
//  Intersect(ray, hit, maxdist, self)
//  
//  Returns TRUE if we hit something in the root model closer than maxdist.  
//  Returns the closest hit in the "hit" buffer.
// 
// Self is a pointer to the last object hit.  If self is NULL then
// the ray either originated at the eye or the last object may be
// self-intersecting ie spheres and cones.  This can be used to
// eliminate doing an intersection test with the last object.
// 
BOOL Intersect(Ray* ray, Isect* hit, Flt maxdist, Object* self) {

	Isect			nhit;
	int				i;
	Flt				min_dist = maxdist;
	Object*			cobj;
	Object*			child;
	Object*			pobj = NULL;
	CompositeData*	cdp;
	Flt				key;

    // If no scene is loaded, return fail flag
    if (!Root) {
        return FALSE;
    }

	// If the object is simple, then return the hit that it gives you

	if (Root->o_type != T_COMPOSITE)
		return (Root->o_procs->intersect) (Root, ray, hit);

	for (i=0; i<3; i++) {
		num[i] = ray->P[i];
		den[i] = ray->D[i];
	}

	// start with an empty priority queue
	PriorityQueueNull();

	CheckAndEnqueue(Root, maxdist);

	for (;;) {

		if (PriorityQueueEmpty())
			break;

		PriorityQueueDelete(&key, &cobj);

		if (key > min_dist) {

			/*
			 * we have already found a primitive
			 * that was closer, we need look no further...
			 */
			 break;

		} else if (cobj->o_type == T_COMPOSITE) {
			/*
			 * if it is in the queue, it got hit.
			 * check each of its children to see if their
			 * bounding volumes get hit.
			 * if so, then push them into the priority
			 * queue...
			 */
			
			cdp = (CompositeData *) cobj->o_data;
			child = cdp->children;

			while (child) {
				if (self != child) {
					CheckAndEnqueue(child, maxdist);
				}
				child = child->next;
			}

		} else {

			/*
			 * we have a primitive 
			 * intersect with the primitive, and possibly
			 * update the nearest hit if it is indeed closer
			 * than the one we currently have...
			 */
			
			if ((cobj->o_procs->intersect) (cobj, ray, &nhit)) {
				if (nhit.isect_t < min_dist) {
					pobj = cobj;
					*hit = nhit;
					min_dist = nhit.isect_t;
				}
			}
		}
	}

	return ((pobj) ? TRUE : FALSE);
}

