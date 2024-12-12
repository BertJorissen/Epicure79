/**
 * @file zeroKernel.h
 * @brief Controller zero kernel for HSOpticalflow. Initializes a tile to 0 on the device.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "kernels_params.h"

// TODO move this to utils using cudalib default
CTRL_KERNEL(Zero, GENERIC, DEFAULT, CTRL_KPARAMS(zero_params), {
	hit(tile, thr_i, thr_j) = 0;
})