#include "kernels_params.h"

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

CTRL_KERNEL(Upscale, OPENCLGPU, DEFAULT, CTRL_KPARAMS(upscale_params), {
	float x = ((float)thr_j + 0.5f) / (float)ctrl_threads.j;
	float y = ((float)thr_i + 0.5f) / (float)ctrl_threads.i;

	// exploit hardware interpolation
	// and scale interpolated vector to match next pyramid level resolution
	hit(out, thr_i, thr_j) = read_imagef(src_img, src_smp, (float2)(x, y)).x * scale;
});
