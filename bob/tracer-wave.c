
/*
	
    tracer-wave.c

	Wave function

*/

#include <stdio.h>
#include <math.h>
#include "inc/bob.h"


//	P		the point in question
//	R		where to put the result
//	waves	top of the linked list

void make_waves(Vec P, Vec R, Wave* waves) {

	Vec     diff;           // diff between point and source
	Flt     dist;           // dist form point to source of wave
	Flt     amp;            // current height of wave

	MakeVector(0, 0, 0, R);				// just to be sure
	
	while (waves) {

		VecSub(P, waves->center, diff);
		
		dist = VecNormalize(diff);
		dist /= waves->wavelength;      // where in cycle are we?
		dist += waves->phase;           // add on offset

		if (waves->damp < 1.0) {         // account for damping
			amp = waves->amp * pow(waves->damp, dist);
		} 
		
		else {
			amp = waves->amp;			// no damping
		}

		amp *= cos(dist * PI*2.0);

		VecAddS(amp, diff, R, R);

		waves = waves->next;
	}

}

