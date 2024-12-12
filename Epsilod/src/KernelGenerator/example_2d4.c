/**
 * @file example_2d4.c
 * @brief Epsilod: Kernel definition file example 2d4
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

HitShape shp_jacobi2d4 = hitShape((-1, 1), (-1, 1));

float patt_jacobi2d4[] = {
	0, 1, 0,
	1, 0, 1,
	0, 1, 0};

float factor_jacobi2d4 = 4;
