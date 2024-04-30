#include "kernels_params.h"

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