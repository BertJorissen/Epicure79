/**
 * @file kernel_protos.h
 * @brief HSOpticalFlow: Prototypes and characterizations for all kernels used in this program
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */
#include "kernels_params.h"

/* Define kernel prototypes */

CTRL_KERNEL_PROTO(Solve, 4, CUDA, DEFAULT, OPENCLGPU, DEFAULT, HIP, DEFAULT, CPU, DEFAULT, solver_params);
CTRL_KERNEL_PROTO(Warp, 4, CUDA, DEFAULT, OPENCLGPU, DEFAULT, HIP, DEFAULT, CPU, DEFAULT, warp_params);
CTRL_KERNEL_PROTO(Upscale, 4, CUDA, DEFAULT, OPENCLGPU, DEFAULT, HIP, DEFAULT, CPU, DEFAULT, upscale_params);
CTRL_KERNEL_PROTO(Downscale, 4, CUDA, DEFAULT, OPENCLGPU, DEFAULT, HIP, DEFAULT, CPU, DEFAULT, downscale_params);
CTRL_KERNEL_PROTO(ComputeDerivatives, 4, CUDA, DEFAULT, OPENCLGPU, DEFAULT, HIP, DEFAULT, CPU, DEFAULT, derivatives_params);
CTRL_KERNEL_PROTO(Add, 1, GENERIC, DEFAULT, add_params);
CTRL_KERNEL_PROTO(Zero, 1, GENERIC, DEFAULT, zero_params);
CTRL_KERNEL_PROTO(Norm, 1, CPU, DEFAULT, norm_params);

/* Define characterizarion for each kernel */
CTRL_KERNEL_CHAR(Solve, MANUAL, SOLVECHAR_1, SOLVECHAR_0);
CTRL_KERNEL_CHAR(Warp, MANUAL, WARPCHAR_1, WARPCHAR_0);
CTRL_KERNEL_CHAR(ComputeDerivatives, MANUAL, DERIVCHAR_1, DERIVCHAR_0);
CTRL_KERNEL_CHAR(Upscale, MANUAL, SCALECHAR_1, SCALECHAR_0);
CTRL_KERNEL_CHAR(Downscale, MANUAL, SCALECHAR_1, SCALECHAR_0);
CTRL_KERNEL_CHAR(Add, MANUAL, 8, 32);
CTRL_KERNEL_CHAR(Zero, MANUAL, 8, 32);
CTRL_KERNEL_CHAR(Norm, MANUAL, 0);