
#include <math.h>


/**
 * x_k+1 = a  x_k  (mod 2^46)
 * r_k = 2^-46 x_k
 */

double	randlc(double * X, double A){

	long long Lx, La;
	long long i246m1 = 0x00003FFFFFFFFFFFLL;
	double d2m46  = pow(0.5,46.0);

	Lx = (long long) *X; La = (long long) A;

	Lx = (Lx * La) & i246m1;
	(*X) = (double) Lx;

	return  (d2m46 * (double) Lx);
}



void	vranlc(N, X, A, Y)
int N;
double *X;
double A;
double Y[];
{
	int i;
	long long Lx, La;
	long long i246m1 = 0x00003FFFFFFFFFFFLL;
	double d2m46 = pow(0.5,46.0);

	Lx = (long long) *X; La = (long long) A;

	for(i=0;i<N;i++){

		Lx = (Lx * La) & i246m1;
		Y[i] =  d2m46 * (double) Lx;
	}
	(*X) = (double) Lx;

	return;
}


