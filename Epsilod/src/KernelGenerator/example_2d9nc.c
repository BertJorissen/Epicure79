/**
 * @file example_2dnc9.c
 * @brief Epsilod: Kernel definition file example 2dnc9
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

HitShape shp_jacobi2d9nc = hitShape((-2, 2), (-2, 2));

float patt_jacobi2d9nc[] = {
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	1, 1, 1, 1, 1,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0};

float factor_jacobi2d9nc = 9;
