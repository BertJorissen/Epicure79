#include "kernels_params.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief compute image derivatives
/// \param[in]  I0  source image
/// \param[in]  I1  tracked image
/// \param[out] Ix  x derivative
/// \param[out] Iy  y derivative
/// \param[out] Iz  temporal derivative
///////////////////////////////////////////////////////////////////////////////
CTRL_KERNEL(ComputeDerivatives, CUDA, DEFAULT, CTRL_KPARAMS(derivatives_params), {
	float dx = 1.0f / (float)hit_tileDimCard(I0, 1);
	float dy = 1.0f / (float)hit_tileDimCard(I0, 0);

	float x = ((float)thr_j + 0.5f) * dx;
	float y = ((float)thr_i + 0.5f) * dy;

	float t0;
	float t1;
	// x derivative
	t0 = tex2D<float>(I0.ext.cuda.tex, x - 2.0f * dx, y);
	t0 -= tex2D<float>(I0.ext.cuda.tex, x - 1.0f * dx, y) * 8.0f;
	t0 += tex2D<float>(I0.ext.cuda.tex, x + 1.0f * dx, y) * 8.0f;
	t0 -= tex2D<float>(I0.ext.cuda.tex, x + 2.0f * dx, y);
	t0 /= 12.0f;

	t1 = tex2D<float>(I1.ext.cuda.tex, x - 2.0f * dx, y);
	t1 -= tex2D<float>(I1.ext.cuda.tex, x - 1.0f * dx, y) * 8.0f;
	t1 += tex2D<float>(I1.ext.cuda.tex, x + 1.0f * dx, y) * 8.0f;
	t1 -= tex2D<float>(I1.ext.cuda.tex, x + 2.0f * dx, y);
	t1 /= 12.0f;

	hit(Ix, thr_i, thr_j) = (t0 + t1) * 0.5f;

	// t derivative
	hit(Iz, thr_i, thr_j) = tex2D<float>(I1.ext.cuda.tex, x, y) - tex2D<float>(I0.ext.cuda.tex, x, y);

	// y derivative
	t0 = tex2D<float>(I0.ext.cuda.tex, x, y - 2.0f * dy);
	t0 -= tex2D<float>(I0.ext.cuda.tex, x, y - 1.0f * dy) * 8.0f;
	t0 += tex2D<float>(I0.ext.cuda.tex, x, y + 1.0f * dy) * 8.0f;
	t0 -= tex2D<float>(I0.ext.cuda.tex, x, y + 2.0f * dy);
	t0 /= 12.0f;

	t1 = tex2D<float>(I1.ext.cuda.tex, x, y - 2.0f * dy);
	t1 -= tex2D<float>(I1.ext.cuda.tex, x, y - 1.0f * dy) * 8.0f;
	t1 += tex2D<float>(I1.ext.cuda.tex, x, y + 1.0f * dy) * 8.0f;
	t1 -= tex2D<float>(I1.ext.cuda.tex, x, y + 2.0f * dy);
	t1 /= 12.0f;

	hit(Iy, thr_i, thr_j) = (t0 + t1) * 0.5f;
});

CTRL_KERNEL(ComputeDerivatives, OPENCLGPU, DEFAULT, CTRL_KPARAMS(derivatives_params), {
	float dx = 1.0f / (float)hit_tileDimCard(I0, 1);
	float dy = 1.0f / (float)hit_tileDimCard(I0, 0);

	float x = ((float)thr_j + 0.5f) * dx;
	float y = ((float)thr_i + 0.5f) * dy;

	float t0;
	float t1;
	// x derivative
	t0 = read_imagef(I0_img, I0_smp, (float2)(x - 2.0f * dx, y)).x;
	t0 -= read_imagef(I0_img, I0_smp, (float2)(x - 1.0f * dx, y)).x * 8.0f;
	t0 += read_imagef(I0_img, I0_smp, (float2)(x + 1.0f * dx, y)).x * 8.0f;
	t0 -= read_imagef(I0_img, I0_smp, (float2)(x + 2.0f * dx, y)).x;
	t0 /= 12.0f;

	t1 = read_imagef(I1_img, I1_smp, (float2)(x - 2.0f * dx, y)).x;
	t1 -= read_imagef(I1_img, I1_smp, (float2)(x - 1.0f * dx, y)).x * 8.0f;
	t1 += read_imagef(I1_img, I1_smp, (float2)(x + 1.0f * dx, y)).x * 8.0f;
	t1 -= read_imagef(I1_img, I1_smp, (float2)(x + 2.0f * dx, y)).x;
	t1 /= 12.0f;

	hit(Ix, thr_i, thr_j) = (t0 + t1) * 0.5f;

	// t derivative
	hit(Iz, thr_i, thr_j) = read_imagef(I1_img, I1_smp, (float2)(x, y)).x - read_imagef(I0_img, I0_smp, (float2)(x, y)).x;

	// y derivative
	t0 = read_imagef(I0_img, I0_smp, (float2)(x, y - 2.0f * dy)).x;
	t0 -= read_imagef(I0_img, I0_smp, (float2)(x, y - 1.0f * dy)).x * 8.0f;
	t0 += read_imagef(I0_img, I0_smp, (float2)(x, y + 1.0f * dy)).x * 8.0f;
	t0 -= read_imagef(I0_img, I0_smp, (float2)(x, y + 2.0f * dy)).x;
	t0 /= 12.0f;

	t1 = read_imagef(I1_img, I1_smp, (float2)(x, y - 2.0f * dy)).x;
	t1 -= read_imagef(I1_img, I1_smp, (float2)(x, y - 1.0f * dy)).x * 8.0f;
	t1 += read_imagef(I1_img, I1_smp, (float2)(x, y + 1.0f * dy)).x * 8.0f;
	t1 -= read_imagef(I1_img, I1_smp, (float2)(x, y + 2.0f * dy)).x;
	t1 /= 12.0f;

	hit(Iy, thr_i, thr_j) = (t0 + t1) * 0.5f;
});