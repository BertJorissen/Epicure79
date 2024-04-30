#include "../../../Utils/opencl_helper.h"
#include "../common.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief compute image derivatives
///
/// CUDA kernel, relies heavily on texture unit
/// \param[in]  width   image width
/// \param[in]  height  image height
/// \param[in]  stride  image stride
/// \param[out] Ix      x derivative
/// \param[out] Iy      y derivative
/// \param[out] Iz      temporal derivative
///////////////////////////////////////////////////////////////////////////////
#define DERIVATIVES_KERNEL "                                                                                     \
	__kernel void ComputeDerivatives(int width, int height, int stride,                                          \
									 __global float *Ix, __global float *Iy, __global float *Iz,                 \
									 __read_only image2d_t texSource,                                            \
									 __read_only image2d_t texTarget) {                                          \
		const int ix = get_global_id(0);                                                                         \
		const int iy = get_global_id(1);                                                                         \
                                                                                                                 \
		const int pos = ix + iy * stride;                                                                        \
                                                                                                                 \
		if (ix >= width || iy >= height) return;                                                                 \
                                                                                                                 \
		float dx = 1.0f / (float)width;                                                                          \
		float dy = 1.0f / (float)height;                                                                         \
                                                                                                                 \
		float     x   = ((float)ix + 0.5f) * dx;                                                                 \
		float     y   = ((float)iy + 0.5f) * dy;                                                                 \
		sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR;            \
                                                                                                                 \
		float t0, t1;                                                                                            \
		/*  x derivative */                                                                                      \
		t0 = read_imagef(texSource, smp, (float2)(x - 2.0f * dx, y)).x;                                          \
		t0 -= read_imagef(texSource, smp, (float2)(x - 1.0f * dx, y)).x * 8.0f;                                  \
		t0 += read_imagef(texSource, smp, (float2)(x + 1.0f * dx, y)).x * 8.0f;                                  \
		t0 -= read_imagef(texSource, smp, (float2)(x + 2.0f * dx, y)).x;                                         \
		t0 /= 12.0f;                                                                                             \
                                                                                                                 \
		t1 = read_imagef(texTarget, smp, (float2)(x - 2.0f * dx, y)).x;                                          \
		t1 -= read_imagef(texTarget, smp, (float2)(x - 1.0f * dx, y)).x * 8.0f;                                  \
		t1 += read_imagef(texTarget, smp, (float2)(x + 1.0f * dx, y)).x * 8.0f;                                  \
		t1 -= read_imagef(texTarget, smp, (float2)(x + 2.0f * dx, y)).x;                                         \
		t1 /= 12.0f;                                                                                             \
                                                                                                                 \
		Ix[pos] = (t0 + t1) * 0.5f;                                                                              \
                                                                                                                 \
		/* t derivative */                                                                                       \
		Iz[pos] = read_imagef(texTarget, smp, (float2)(x, y)).x - read_imagef(texSource, smp, (float2)(x, y)).x; \
                                                                                                                 \
		/* y derivative */                                                                                       \
		t0 = read_imagef(texSource, smp, (float2)(x, y - 2.0f * dy)).x;                                          \
		t0 -= read_imagef(texSource, smp, (float2)(x, y - 1.0f * dy)).x * 8.0f;                                  \
		t0 += read_imagef(texSource, smp, (float2)(x, y + 1.0f * dy)).x * 8.0f;                                  \
		t0 -= read_imagef(texSource, smp, (float2)(x, y + 2.0f * dy)).x;                                         \
		t0 /= 12.0f;                                                                                             \
                                                                                                                 \
		t1 = read_imagef(texTarget, smp, (float2)(x, y - 2.0f * dy)).x;                                          \
		t1 -= read_imagef(texTarget, smp, (float2)(x, y - 1.0f * dy)).x * 8.0f;                                  \
		t1 += read_imagef(texTarget, smp, (float2)(x, y + 1.0f * dy)).x * 8.0f;                                  \
		t1 -= read_imagef(texTarget, smp, (float2)(x, y + 2.0f * dy)).x;                                         \
		t1 /= 12.0f;                                                                                             \
                                                                                                                 \
		Iy[pos] = (t0 + t1) * 0.5f;                                                                              \
	} "

///////////////////////////////////////////////////////////////////////////////
/// \brief compute image derivatives
///
/// \param[in]  I0  source image
/// \param[in]  I1  tracked image
/// \param[in]  w   image width
/// \param[in]  h   image height
/// \param[in]  s   image stride
/// \param[out] Ix  x derivative
/// \param[out] Iy  y derivative
/// \param[out] Iz  temporal derivative
///////////////////////////////////////////////////////////////////////////////
static void ComputeDerivatives(cl_context ctx, cl_command_queue queue, cl_kernel k_derivatives, cl_mem I0, cl_mem I1, int w, int h,
							   int s, cl_mem Ix, cl_mem Iy, cl_mem Iz) {

	size_t local_size[2]  = {32, 6};
	size_t global_size[2] = {iAlignUp(w, local_size[0]), iAlignUp(h, local_size[1])};

	#ifdef ALL_IMG
	cl_mem texSource = I0;
	cl_mem texTarget = I1;
	#else // ALL_IMG
	cl_int err;

	// create images from buffers

	// Define the image format
	cl_image_format image_fmt;
	image_fmt.image_channel_order     = CL_R; // Single-channel (red)
	image_fmt.image_channel_data_type = CL_FLOAT;

	// Create the image from the existing buffer
	cl_image_desc image_desc;
	image_desc.image_type        = CL_MEM_OBJECT_IMAGE2D;
	image_desc.image_width       = w;
	image_desc.image_height      = h;
	image_desc.image_array_size  = 1;
	image_desc.image_row_pitch   = 0;
	image_desc.image_slice_pitch = 0;
	image_desc.num_mip_levels    = 0;
	image_desc.num_samples       = 0;
	image_desc.buffer            = I0;

	cl_mem texSource = clCreateImage(ctx, CL_MEM_READ_ONLY, &image_fmt, &image_desc, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	image_desc.buffer = I1;
	cl_mem texTarget  = clCreateImage(ctx, CL_MEM_READ_ONLY, &image_fmt, &image_desc, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	#endif // ALL_IMG

	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 0, sizeof(int), &w));
	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 1, sizeof(int), &h));
	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 2, sizeof(int), &s));
	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 3, sizeof(cl_mem), &Ix));
	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 4, sizeof(cl_mem), &Iy));
	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 5, sizeof(cl_mem), &Iz));
	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 6, sizeof(cl_mem), &texSource));
	OPENCL_ASSERT_OP(clSetKernelArg(k_derivatives, 7, sizeof(cl_mem), &texTarget));

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queue, k_derivatives, 2, NULL, global_size, local_size, 0, NULL, NULL));
}
