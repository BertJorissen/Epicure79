/**
 * @file upscaleKernel.h
 * @brief OpenCL reference upscale kernel for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "../../../Utils/opencl_helper.h"
#include "../common.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief upscale one component of a displacement field, OpenCL kernel
/// \param[in]  width   field width
/// \param[in]  height  field height
/// \param[in]  stride  field stride
/// \param[in]  scale   scale factor (multiplier)
/// \param[out] out     result
///////////////////////////////////////////////////////////////////////////////
#ifdef ALL_IMG
#define UPSCALE_KERNEL "                                                                                          \
	__kernel void Upscale(int width, int height, int stride, float scale,                                         \
						  __write_only image2d_t out, __read_only image2d_t texCoarse) {                          \
		const int ix = get_global_id(0);                                                                          \
		const int iy = get_global_id(1);                                                                          \
                                                                                                                  \
		if (ix >= width || iy >= height) return;                                                                  \
                                                                                                                  \
		float x = ((float)ix + 0.5f) / (float)width;                                                              \
		float y = ((float)iy + 0.5f) / (float)height;                                                             \
                                                                                                                  \
		sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR;             \
                                                                                                                  \
		/* exploit hardware interpolation and scale interpolated vector to match next pyramid level resolution */ \
		write_imagef(out, (int2)(ix, iy), read_imagef(texCoarse, smp, (float2)(x, y)).x * scale);                 \
	} "
#else // ALL_IMG
#define UPSCALE_KERNEL "                                                                                          \
	__kernel void Upscale(int width, int height, int stride, float scale,                                         \
						  __global float *out, __read_only image2d_t texCoarse) {                                 \
		const int ix = get_global_id(0);                                                                          \
		const int iy = get_global_id(1);                                                                          \
                                                                                                                  \
		if (ix >= width || iy >= height) return;                                                                  \
                                                                                                                  \
		float x = ((float)ix + 0.5f) / (float)width;                                                              \
		float y = ((float)iy + 0.5f) / (float)height;                                                             \
                                                                                                                  \
		sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR;             \
                                                                                                                  \
		/* exploit hardware interpolation and scale interpolated vector to match next pyramid level resolution */ \
		out[ix + iy * stride] = read_imagef(texCoarse, smp, (float2)(x, y)).x * scale;                            \
	} "
#endif // ALL_IMG

///////////////////////////////////////////////////////////////////////////////
/// \brief upscale one component of a displacement field, kernel wrapper
/// \param[in]  src         field component to upscale
/// \param[in]  width       field current width
/// \param[in]  height      field current height
/// \param[in]  stride      field current stride
/// \param[in]  newWidth    field new width
/// \param[in]  newHeight   field new height
/// \param[in]  newStride   field new stride
/// \param[in]  scale       value scale factor (multiplier)
/// \param[out] out         upscaled field component
///////////////////////////////////////////////////////////////////////////////
static void Upscale(cl_context ctx, cl_command_queue queue, cl_kernel k_upscale, cl_mem src, int width, int height, int stride,
					int newWidth, int newHeight, int newStride, float scale,
					cl_mem out) {

	size_t local_size[2]  = {32, 8};
	size_t global_size[2] = {iAlignUp(newWidth, local_size[0]), iAlignUp(newHeight, local_size[1])};

	#ifdef ALL_IMG
	cl_mem texCoarse = src;
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
	image_desc.image_width       = width;
	image_desc.image_height      = height;
	image_desc.image_array_size  = 1;
	image_desc.image_row_pitch   = 0;
	image_desc.image_slice_pitch = 0;
	image_desc.num_mip_levels    = 0;
	image_desc.num_samples       = 0;
	image_desc.buffer            = src;

	cl_mem texCoarse = clCreateImage(ctx, CL_MEM_READ_ONLY, &image_fmt, &image_desc, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	#endif // ALL_IMG

	OPENCL_ASSERT_OP(clSetKernelArg(k_upscale, 0, sizeof(int), &newWidth));
	OPENCL_ASSERT_OP(clSetKernelArg(k_upscale, 1, sizeof(int), &newHeight));
	OPENCL_ASSERT_OP(clSetKernelArg(k_upscale, 2, sizeof(int), &newStride));
	OPENCL_ASSERT_OP(clSetKernelArg(k_upscale, 3, sizeof(float), &scale));
	OPENCL_ASSERT_OP(clSetKernelArg(k_upscale, 4, sizeof(cl_mem), &out));
	OPENCL_ASSERT_OP(clSetKernelArg(k_upscale, 5, sizeof(cl_mem), &texCoarse));

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queue, k_upscale, 2, NULL, global_size, local_size, 0, NULL, NULL));
}
