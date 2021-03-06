
/*
	math-vector.c

	Vector and matrix function

*/

#define SIZE    (4)

#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "inc/bob.h"

static void lubksb(Matrix a, int* indx, Flt* b);
static void ludcmp(Matrix a, int* indx, Flt* d);

Flt	VecNormalize(Vec vec) {

	Flt	len;

	len = VecDot(vec, vec);
	if (ABS(len)>TINY) {
		len = sqrt(len);
		vec[0] /= len;
		vec[1] /= len;
		vec[2] /= len;
	} else {
		// printf("%.4f %.4f %.4f == %.4f bummer\n", vec[0], vec[1], vec[2], len);
		len = 1.0;
		vec[0] = 1.0;
		vec[1] = 0.0;
		vec[2] = 0.0;
	}
	return(len);
}

void identity(Matrix mat) {

	int     i, j;

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			if (i==j)
				mat[i][j] = 1.0;
			else
				mat[i][j] = 0.0;
}

void matrix_cat(Matrix m1, Matrix m2, Matrix dest) {

	Matrix  m3;
	int     i, j, k;

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			m3[i][j] = 0;
			for (k=0; k<4; k++) {
				m3[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	// copy results to dest
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			dest[i][j] = m3[i][j];
		}
	}
}

void trans_vector(Matrix mat, Vec in, Vec out) {

	Flt     in4[4], out4[4];
	int     i, j;

	in4[0] = in[0];
	in4[1] = in[1];
	in4[2] = in[2];
	in4[3] = 1.0;

	for (i=0; i<4; i++) {
		out4[i] = 0.0;
		for (j=0; j<4; j++) {
			out4[i] += mat[j][i] * in4[j];
		}
	}
	out[0] = out4[0]/out4[3];
	out[1] = out4[1]/out4[3];
	out[2] = out4[2]/out4[3];
}

void trans_normal(Matrix mat, Vec in, Vec out) {

	Vec     t1, t2;         // tangent vectors
	Vec     orig;           // imaginary center
	Flt     dot;

	VecNormalize(in);

	MakeVector(0.0, 0.0, 0.0, orig);

	// create a vector not aligned with in
	MakeVector(1.0, 0.0, 0.0, t1);
	dot = VecDot(t1, in);
	if(ABS(dot) > 0.8) {
		MakeVector(0.0, 1.0, 0.0, t1);
	}

	VecCross(in, t1, t2);   // create t2
	VecCross(t2, in, t1);   // create proper t1

	// transform tangents
	trans_vector(mat, t1, t1);
	trans_vector(mat, t2, t2);
	trans_vector(mat, orig, orig);

	VecSub(t1, orig, t1);
	VecSub(t2, orig, t2);

	VecCross(t1, t2, out);  // recreate normal

}

/*
	matrix_inverse() -- creates the inverse of a 4x4 matrix.
*/

void matrix_inverse(Matrix m, Matrix n) {

	Matrix  y;
	int     i, j, indx[4];
	double  d, col[4];

	matrix_copy(m, n);                 // save original matrix
	ludcmp(n, indx, &d);               // matrix lu decomposition

	for (j = 0; j < SIZE; j++) {       // matrix inversion
		for (i = 0; i < SIZE; i++) {
			col[i] = 0.0;
		}
		col[j] = 1.0;
		lubksb(n, indx, col);
		for (i = 0;i < SIZE;i++) {
			y[i][j] = col[i];
		}
	}
	matrix_copy(y, n);
}

//
// LU backward substitution
// a			input matrix
// indx			row permutation record
// b			right hand vector
//
static void lubksb(Matrix a, int* indx, Flt* b) {

	int     i, j, ii=-1, ip;
	Flt     sum;

	for (i = 0;i < SIZE;i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii>=0) {
			for (j = ii;j <= i-1;j++) {
				sum -= a[i][j] * b[j];
			}
		}
		else if (sum != 0.0) {
			ii = i;
		}
		b[i] = sum;
	}
	for (i = SIZE-1;i >= 0;i--) {
		sum = b[i];
		for (j = i+1;j < SIZE;j++) {
			sum -= a[i][j] * b[j];
		}
		b[i] = sum/a[i][i];
	}
}

//
// LU decomposition
//	a           input matrix. gets thrashed
//	indx        row permutation record
//	d           +/- 1.0 (even or odd # of row interchanges
//
static void ludcmp(Matrix a, int* indx, Flt* d) {

	Flt     vv[SIZE];               // implicit scale for each row
	Flt     big, dum, sum, tmp;
	int     i, imax, j, k;

	*d = 1.0;
	for (i = 0;i < SIZE;i++) {
		big = 0.0;
		for (j = 0;j < SIZE;j++) {
			if ((tmp = fabs(a[i][j])) > big) {
				big = tmp;
			}
		}
		if (big == 0.0) {
			fprintf(stderr, "ludcmp(): singular matrix found...\n");
			last_call(1);
		}
		vv[i] = 1.0/big;
	}
	for (j = 0;j < SIZE;j++) {
		for (i = 0;i < j;i++) {
			sum = a[i][j];
			for (k = 0;k < i;k++) {
				sum -= a[i][k] * a[k][j];
			}
			a[i][j] = sum;
		}
		big = 0.0;
		for (i = j;i < SIZE;i++) {
			sum = a[i][j];
			for (k = 0;k < j;k++) {
				sum -= a[i][k]*a[k][j];
			}
			a[i][j] = sum;
			if ((dum = vv[i] * fabs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k = 0;k < SIZE;k++) {
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (a[j][j] == 0.0) {
			a[j][j] = 1.0e-20;      // can be 0.0 also...
		}
		if (j != SIZE-1) {
			dum = 1.0/a[j][j];
			for (i = j+1;i < SIZE;i++) {
				a[i][j] *= dum;
			}
		}
	}
}

// m1 source
// m2 dest
void matrix_copy(Matrix m1, Matrix m2) {
	memcpy(m2, m1, sizeof(Matrix));
}

