/**
 * @file downscaleKernel.h
 * @brief Controller downscale kernel for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "kernels_params.h"
#include "cpu_tex.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief downscale image
/// \param[in]  src     image to downscale
/// \param[out] out     result
///////////////////////////////////////////////////////////////////////////////
CTRL_KERNEL(Downscale, CUDA, DEFAULT, CTRL_KPARAMS(downscale_params), {
	float dx = 1.0f / (float)hit_tileDimCard(out, 1);
	float dy = 1.0f / (float)hit_tileDimCard(out, 0);

	float x = ((float)thr_j + 0.5f) * dx;
	float y = ((float)thr_i + 0.5f) * dy;

	hit(out, thr_i, thr_j) = 0.25f * (tex2D<float>(src.ext.cuda.tex, x - dx * 0.25f, y) +
									  tex2D<float>(src.ext.cuda.tex, x + dx * 0.25f, y) +
									  tex2D<float>(src.ext.cuda.tex, x, y - dy * 0.25f) +
									  tex2D<float>(src.ext.cuda.tex, x, y + dy * 0.25f));
});

CTRL_KERNEL(Downscale, HIP, DEFAULT, CTRL_KPARAMS(downscale_params), {
	float dx = 1.0f / (float)hit_tileDimCard(out, 1);
	float dy = 1.0f / (float)hit_tileDimCard(out, 0);

	float x = ((float)thr_j + 0.5f) * dx;
	float y = ((float)thr_i + 0.5f) * dy;

	hit(out, thr_i, thr_j) = 0.25f * (tex2D<float>(src.ext.hip.tex, x - dx * 0.25f, y) +
									  tex2D<float>(src.ext.hip.tex, x + dx * 0.25f, y) +
									  tex2D<float>(src.ext.hip.tex, x, y - dy * 0.25f) +
									  tex2D<float>(src.ext.hip.tex, x, y + dy * 0.25f));
});

CTRL_KERNEL(Downscale, OPENCLGPU, DEFAULT, CTRL_KPARAMS(downscale_params), {
	float dx = 1.0f / (float)hit_tileDimCard(out, 1);
	float dy = 1.0f / (float)hit_tileDimCard(out, 0);

	float x = ((float)thr_j + 0.5f) * dx;
	float y = ((float)thr_i + 0.5f) * dy;

	hit(out, thr_i, thr_j) = 0.25f * (read_imagef(src_img, src_smp, (float2)(x - dx * 0.25f, y)).x +
									  read_imagef(src_img, src_smp, (float2)(x + dx * 0.25f, y)).x +
									  read_imagef(src_img, src_smp, (float2)(x, y - dy * 0.25f)).x +
									  read_imagef(src_img, src_smp, (float2)(x, y + dy * 0.25f)).x);
});

CTRL_KERNEL(Downscale, CPU, DEFAULT, CTRL_KPARAMS(downscale_params), {
	// FIXME @sergioalo find better way of calculating texture sizes on cpu
	int tex_w = hit_tileDimCard(src, 1);
	int tex_h = hit_tileDimCard(src, 0);

	const int srcX = thr_j * 2;
	const int srcY = thr_i * 2;
	// average 4 neighbouring pixels
	float sum;
	sum = cpu_Tex2Di(src, tex_w, tex_h, srcX + 0, srcY + 0);
	sum += cpu_Tex2Di(src, tex_w, tex_h, srcX + 0, srcY + 1);
	sum += cpu_Tex2Di(src, tex_w, tex_h, srcX + 1, srcY + 0);
	sum += cpu_Tex2Di(src, tex_w, tex_h, srcX + 1, srcY + 1);
	// normalize
	sum *= 0.25f;
	hit(out, thr_i, thr_j) = sum;
});
