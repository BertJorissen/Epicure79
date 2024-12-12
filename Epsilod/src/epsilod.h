/**
 * @file epsilod.h
 * @brief Epsilod: Stencil code: Any dimensions, stencil as a pattern of weights. Data type: float
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifndef _EPSILOD_H_
#define _EPSILOD_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef _CTRL_ARCH_CUDA_
#include <cuda_runtime_api.h>
#endif // _CTRL_ARCH_CUDA_

#include <epsilod_ext_type.h>

#include "Ctrl.h"

Ctrl_NewType(float);

/* Print usage: Extra options with environment variables */
void epsilod_print_usage();

/* Special functions definition. */
typedef void (*stencilFunction)(PCtrl, Ctrl_Thread, Ctrl_Thread, int, HitTile_float, HitTile_float, HitTile_float, float, Epsilod_ext *);
typedef void (*initDataFunction)(HitTile_float, int, int[], int[], Epsilod_ext);
typedef void (*outputDataFunction)(HitTile_float, Epsilod_ext);

/* Parallel stencil skeleton launcher prototype (public API). */
void stencilComputation(int                sizes[],
						HitShape           stencilShape,
						float              stencilData[],
						float              factor,
						int                numIterations,
						stencilFunction    f_pdateCell,
						initDataFunction   f_init,
						outputDataFunction f_output,
						Epsilod_ext       *ext_params,
						char              *device_selection_file);

/* Definitions to declare optimized stencil kernel prototypes and wrapper launcher */
#define REGISTER_STENCIL(stencilname, ...)                                                                                                                                                     \
	REGISTER_STENCIL_N(stencilname, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)                                                                                                                 \
	CTRL_KERNEL_CHAR(stencilname, MANUAL, 64, 8, 1);                                                                                                                                           \
	void stencilname(PCtrl ctrl, Ctrl_Thread threads, Ctrl_Thread blockSize, int stream, HitTile_float mat, HitTile_float copy, HitTile_float weight, float factor, Epsilod_ext *ext_params) { \
		Ctrl_LaunchToStream(ctrl, stencilname, threads, blockSize, stream, mat, copy, *ext_params);                                                                                            \
	}

#define REGISTER_STENCIL_N(stencilname, n_archs_times2, ...)  REGISTER_STENCIL_N2(stencilname, n_archs_times2, __VA_ARGS__)
#define REGISTER_STENCIL_N2(stencilname, n_archs_times2, ...) STENCIL_PROTO(stencilname, n_archs_times2, n_archs_##n_archs_times2, __VA_ARGS__)
#define STENCIL_PROTO(stencilname, n_archs_times2, n_archs, ...)           \
	CTRL_KERNEL_PROTO(stencilname,                                         \
					  n_archs,                                             \
					  STENCIL_EXTRACT_ARCHS_##n_archs_times2(__VA_ARGS__), \
					  3,                                                   \
					  OUT, HitTile_float, matrix,                          \
					  IN, HitTile_float, matrixCopy,                       \
					  INVAL, Epsilod_ext, ext_params);

#define STENCIL_EXTRACT_ARCHS_2(arch, subarch)       arch, subarch
#define STENCIL_EXTRACT_ARCHS_4(arch, subarch, ...)  arch, subarch, STENCIL_EXTRACT_ARCHS_2(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_6(arch, subarch, ...)  arch, subarch, STENCIL_EXTRACT_ARCHS_4(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_8(arch, subarch, ...)  arch, subarch, STENCIL_EXTRACT_ARCHS_6(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_10(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_8(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_12(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_10(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_14(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_12(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_16(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_14(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_18(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_16(__VA_ARGS__)
#define STENCIL_EXTRACT_ARCHS_20(arch, subarch, ...) arch, subarch, STENCIL_EXTRACT_ARCHS_18(__VA_ARGS__)

#define n_archs_2  1
#define n_archs_4  2
#define n_archs_6  3
#define n_archs_8  4
#define n_archs_10 5
#define n_archs_12 6
#define n_archs_14 7
#define n_archs_16 8
#define n_archs_18 9
#define n_archs_20 10

#endif
