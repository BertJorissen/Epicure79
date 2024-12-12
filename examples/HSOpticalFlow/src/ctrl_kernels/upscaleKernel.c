/**
 * @file upscaleKernel.h
 * @brief Controller upscale kernel for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "kernels_params.h"
#include "cpu_tex.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief upscale one component of a displacement field, CUDA kernel
/// \param[in]  src     field component to upscale
/// \param[in]  scale   scale factor (multiplier)
/// \param[out] out     upscaled field component
///////////////////////////////////////////////////////////////////////////////
CTRL_KERNEL(Upscale, CUDA, DEFAULT, CTRL_KPARAMS(upscale_params), {
	float x = ((float)thr_j + 0.5f) / (float)ctrl_threads.j;
	float y = ((float)thr_i + 0.5f) / (float)ctrl_threads.i;

	// exploit hardware interpolation
	// and scale interpolated vector to match next pyramid level resolution
	hit(out, thr_i, thr_j) = tex2D<float>(src.ext.cuda.tex, x, y) * scale;
});

CTRL_KERNEL(Upscale, HIP, DEFAULT, CTRL_KPARAMS(upscale_params), {
	float x = ((float)thr_j + 0.5f) / (float)ctrl_threads.j;
	float y = ((float)thr_i + 0.5f) / (float)ctrl_threads.i;

	// exploit hardware interpolation
	// and scale interpolated vector to match next pyramid level resolution
	hit(out, thr_i, thr_j) = tex2D<float>(src.ext.hip.tex, x, y) * scale;
});

CTRL_KERNEL(Upscale, OPENCLGPU, DEFAULT, CTRL_KPARAMS(upscale_params), {
	float x = ((float)thr_j + 0.5f) / (float)ctrl_threads.j;
	float y = ((float)thr_i + 0.5f) / (float)ctrl_threads.i;

	// exploit hardware interpolation
	// and scale interpolated vector to match next pyramid level resolution
	hit(out, thr_i, thr_j) = read_imagef(src_img, src_smp, (float2)(x, y)).x * scale;
});

CTRL_KERNEL(Upscale, CPU, DEFAULT, CTRL_KPARAMS(upscale_params), {
	// FIXME @sergioalo find better way of calculating texture sizes on cpu, currently this only works on hso multiframe version
	int tex_w = hit_tileDimCard(src, 1);
	int tex_h = hit_tileDimCard(src, 0);

	float x = ((float)thr_j - 0.5f) * 0.5f;
	float y = ((float)thr_i - 0.5f) * 0.5f;

	hit(out, thr_i, thr_j) = cpu_Tex2D(src, tex_w, tex_h, x, y) * scale;
});