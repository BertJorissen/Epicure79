/**
 * @file warpingKernel.h
 * @brief Controller warping kernel for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "kernels_params.h"
#include "cpu_tex.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief warp image with a given displacement field, CUDA kernel.
/// \param[in]  src     source image
/// \param[in]  u       horizontal displacement
/// \param[in]  v       vertical displacement
/// \param[out] out     result
///////////////////////////////////////////////////////////////////////////////
CTRL_KERNEL(Warp, CUDA, DEFAULT, CTRL_KPARAMS(warp_params), {
	float x = ((float)thr_j + hit(u, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 1);
	float y = ((float)thr_i + hit(v, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 0);

	hit(out, thr_i, thr_j) = tex2D<float>(src.ext.cuda.tex, x, y);
});

CTRL_KERNEL(Warp, HIP, DEFAULT, CTRL_KPARAMS(warp_params), {
	float x = ((float)thr_j + hit(u, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 1);
	float y = ((float)thr_i + hit(v, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 0);

	hit(out, thr_i, thr_j) = tex2D<float>(src.ext.hip.tex, x, y);
});

CTRL_KERNEL(Warp, OPENCLGPU, DEFAULT, CTRL_KPARAMS(warp_params), {
	float x = ((float)thr_j + hit(u, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 1);
	float y = ((float)thr_i + hit(v, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 0);

	hit(out, thr_i, thr_j) = read_imagef(src_img, src_smp, (float2)(x, y)).x;
});

CTRL_KERNEL(Warp, CPU, DEFAULT, CTRL_KPARAMS(warp_params), {
	// FIXME @sergioalo find better way of calculating texture sizes on cpu
	int tex_w = ctrl_threads.j;
	int tex_h = ctrl_threads.i;

	float x = (float)thr_j + hit(u, thr_i, thr_j);
	float y = (float)thr_i + hit(v, thr_i, thr_j);

	hit(out, thr_i, thr_j) = cpu_Tex2D(src, tex_w, tex_h, x, y);
});