/**
 * @file kernel_params.h
 * @brief MatrixAdd: List of parameters for kernels
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

/* DECLARE THE COMMON PARAMETERS FOR ALL KERNEL VERSIONS */

Ctrl_NewType(float);

#define madd_params 4, INVAL, int, n_iter, IN, HitTile_float, A, IN, HitTile_float, B, IO, HitTile_float, C
