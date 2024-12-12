/**
 * @file kernel_params.h
 * @brief HSOpticalFlow: List of parameters for kernels
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

/* DECLARE THE COMMON PARAMETERS FOR ALL KERNEL VERSIONS */
#include "Ctrl.h"
Ctrl_NewType(float);

#define add_params         3, IN, HitTile_float, op1, IN, HitTile_float, op2, OUT, HitTile_float, sum
#define solver_params      8, IN, HitTile_float, du0, IN, HitTile_float, dv0, IN, HitTile_float, Ix, IN, HitTile_float, Iy, IN, HitTile_float, Iz, INVAL, float, alpha, OUT, HitTile_float, du1, OUT, HitTile_float, dv1
#define warp_params        4, IN, HitTile_float, src, IN, HitTile_float, u, IN, HitTile_float, v, OUT, HitTile_float, out
#define upscale_params     3, IN, HitTile_float, src, INVAL, float, scale, OUT, HitTile_float, out
#define downscale_params   2, IN, HitTile_float, src, OUT, HitTile_float, out
#define derivatives_params 5, IN, HitTile_float, I0, IN, HitTile_float, I1, OUT, HitTile_float, Ix, OUT, HitTile_float, Iy, OUT, HitTile_float, Iz
#define zero_params        1, OUT, HitTile_float, tile
#define norm_params        5, IN, HitTile_float, u, IN, HitTile_float, v, INVAL, double *, p_sum, INVAL, double *, p_res, INVAL, int, frame