/**
 * @file Texture_Test_Ctrl_Kernels.c
 * @brief Texture test: Ctrl version kernels.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include "Texture_Test_Ctrl.h"

Ctrl_NewType(float);

CTRL_KERNEL(TexTest, CUDA, DEFAULT, CTRL_KPARAMS(textest_params), {
	hit(matrix_result, thr_i, thr_j) = tex2D<float>(matrix_a.ext.cuda.tex, thr_j, thr_i);
});

CTRL_KERNEL(TexTest, HIP, DEFAULT, CTRL_KPARAMS(textest_params), {
	hit(matrix_result, thr_i, thr_j) = tex2D<float>(matrix_a.ext.hip.tex, thr_j, thr_i);
});

CTRL_KERNEL(TexTest, OPENCLGPU, DEFAULT, CTRL_KPARAMS(textest_params), {
	hit(matrix_result, thr_i, thr_j) = read_imagef(matrix_a_img, matrix_a_smp, (int2)(thr_j, thr_i)).x;
});