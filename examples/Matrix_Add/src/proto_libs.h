/**
 * @file proto_libs.h
 * @brief MatrixAdd: Kernel prototype to use specialized third-party libs interface
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "kernels_params.h"

/* Defining kernel prototypes */
/* Generic is also added for other architectures with no lib kernel yet defined: OpenCL, ... */
CTRL_KERNEL_PROTO(Add, 5,
				  CUDALIB, MAGMA,
				  CUDALIB, CUBLAS,
				  HIPLIB, HIPBLAS,
				  CPULIB, MKL,
				  GENERIC, DEFAULT,
				  madd_params);
