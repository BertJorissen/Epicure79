#include "kernels_params.h"

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

CTRL_KERNEL(Warp, OPENCLGPU, DEFAULT, CTRL_KPARAMS(warp_params), {
	float x = ((float)thr_j + hit(u, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 1);
	float y = ((float)thr_i + hit(v, thr_i, thr_j) + 0.5f) / (float)hit_tileDimCard(src, 0);

	hit(out, thr_i, thr_j) = read_imagef(src_img, src_smp, (float2)(x, y)).x;
});