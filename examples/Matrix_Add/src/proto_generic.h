/**
 * @file proto_generic.h
 * @brief MatrixAdd: Kernel prototype to use only the generic kernel on any architecture
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "kernels_params.h"

/* Defining kernel prototypes */
CTRL_KERNEL_PROTO(Add, 1,
				  GENERIC, DEFAULT,
				  madd_params);
