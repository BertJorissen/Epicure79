/**
 * @file Matrix_Increment_Ctrl_Kernels.c
 * @brief Matrix Increment: Ctrl kernel file
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include <math.h>

Ctrl_NewType(float);

CTRL_KERNEL(Increment, GENERIC, DEFAULT, int n, KHitTile_float input, KHitTile_float output, {
	float res = hit(input, thr_i, thr_j);
	for (int i = 0; i < n; i++) {
		res += 1;
	}
	hit(output, thr_i, thr_j) = res;
	// hit(output, thr_i, thr_j) = hit(input, thr_i, thr_j) + n;
});

CTRL_KERNEL(Norm_calc, CPU, DEFAULT, KHitTile_float matrix, {
	double resultado = 0;
	double suma      = 0;
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			suma += pow(hit(matrix, i, j), 2);
		}
	}
	resultado = sqrt(suma);

	printf("\n ----------------------- NORM ----------------------- \n");
	printf("\n Acumulated sum: %lf", suma);
	printf("\n Result: %lf \n", resultado);
	printf("\n ---------------------------------------------------- \n");
	fflush(stdout);
});
