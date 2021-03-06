/*
	math-noise.c

	Noise/turbulence functions

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "inc/bob.h"

#define NOISE_SEED      (666)           // just beastly, eh?

#define NUMPTS  512
#define P1      173
#define P2      263
#define P3      337
#define phi	0.6180339

static Flt pts[NUMPTS];

/*
	init_noise() - generate noise points +- 0.5
*/

void init_noise(void) {

	int	i;

	srand(NOISE_SEED);
	for (i=0; i<NUMPTS; i++) {
		pts[i] = rand()/(Flt)RAND_MAX - 0.5;
	}
}

#define HASH(a,b,c)     (a+b+c & NUMPTS-1)

Flt noise1(Vec p) {

	int     xi, yi, zi, xa, xb, ya, yb, za, zb;
	Flt	    xf, yf, zf;
	Flt     p000, p100, p010, p110;
	Flt     p001, p101, p011, p111;

	xf = p[0];
	yf = p[1];
	zf = p[2];

	xi = (int) floor(xf);
	xa = (int) floor(P1*(xi*phi - floor(xi*phi)));
	xb = (int) floor(P1*((xi+1)*phi - floor((xi+1)*phi)));

	yi = (int) floor(yf);
	ya = (int) floor(P2*(yi*phi - floor(yi*phi)));
	yb = (int) floor(P2*((yi+1)*phi - floor((yi+1)*phi)));

	zi = (int) floor(zf);
	za = (int) floor(P3*(zi*phi - floor(zi*phi)));
	zb = (int) floor(P3*((zi+1)*phi - floor((zi+1)*phi)));

	p000 = pts[HASH(xa, ya, za)];
	p001 = pts[HASH(xa, ya, zb)];
	p010 = pts[HASH(xa, yb, za)];
	p100 = pts[HASH(xb, ya, za)];
	p011 = pts[HASH(xa, yb, zb)];
	p110 = pts[HASH(xb, yb, za)];
	p101 = pts[HASH(xb, ya, zb)];
	p111 = pts[HASH(xb, yb, zb)];

	xf = xf - xi;
	yf = yf - yi;
	zf = zf - zi;

	/* Since we're just doing linear interpolation between the points
	   we need to "bend" the line a bit to fake a spline interpolation.
	   This looks almost as good as the spline interpolation but
	   is much faster. */

	if (xf > 0.5) {
		xf = 1.0 - xf;
		xf = 2.0 * xf * xf;
		xf = 1.0 - xf;
	} else {
		xf = 2.0 * xf * xf;
	}
	if (yf > 0.5) {
		yf = 1.0 - yf;
		yf = 2.0 * yf * yf;
		yf = 1.0 - yf;
	} else {
		yf = 2.0 * yf * yf;
	}
	if (zf > 0.5) {
		zf = 1.0 - zf;
		zf = 2.0 * zf * zf;
		zf = 1.0 - zf;
	} else {
		zf = 2.0 * zf * zf;
	}

	return  p000 * (1-xf) * (1-yf) * (1-zf) +
		p001 * (1-xf) * (1-yf) * zf     +
		p010 * (1-xf) * yf     * (1-zf) +
		p100 * xf     * (1-yf) * (1-zf) +
		p011 * (1-xf) * yf     * zf     +
		p110 * xf     * yf     * (1-zf) +
		p101 * xf     * (1-yf) * zf     +
		p111 * xf     * yf     * zf;
}

void noise3(Vec p, Flt* v) {

	Vec     tmp;

	v[0] = noise1(p);
	tmp[0] = p[1]+P1;
	tmp[1] = p[0]+P2;
	tmp[2] = p[2]+P3;
	v[1] = noise1(tmp);
	tmp[0] += P2;
	tmp[1] += P3;
	tmp[2] += P1;
	v[2] = noise1(tmp);
}

void DNoise(Vec p, Flt* v) {

	Vec     tmp;
	Flt     center;

	center = noise1(p);
	tmp[0] = p[0] + 0.1;
	tmp[1] = p[1];
	tmp[2] = p[2];
	v[0] = (noise1(tmp) - center) * 10.0;
	tmp[0] = p[0];
	tmp[1] += 0.1;
	v[1] = (noise1(tmp) - center) * 10.0;
	tmp[1] = p[1];
	tmp[2] += 0.1;
	v[2] = (noise1(tmp) - center) * 10.0;
}       /* end of DNoise() */

// input point, gets thrashed
// max number of levels
Flt turb1(Vec p, int lvl) {

	Flt     result = 0.0;
	int     i;

	for (i=0; i<lvl; i++) {
		result += noise1(p)/(Flt)(1<<i);
		VecS(2.0, p, p);
	}

	return result;
}

// p    input point, gets thrashed
// v	return vector
// lvl	max number of levels
void turb3(Vec p, Flt* v, int lvl) {

	Vec     c;
	int     i;

	MakeVector(0, 0, 0, v);
	for (i=0; i<lvl; i++) {
		noise3(p, c);
		/* DNoise(p, c); */
		VecS(1.0/(Flt)(1<<i), c, c);
		VecAdd(c, v, v);
		VecS(2.0, p, p);
	}
}


