/**
 * @file downscaleKernel.h
 * @brief OpenCL reference downscale kernel for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "../../../Utils/opencl_helper.h"
#include "../common.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief downscale image
///
/// OpenCL kernel, relies heavily on texture unit
/// \param[in]  width   image width
/// \param[in]  height  image height
/// \param[in]  stride  image stride
/// \param[out] out     result
///////////////////////////////////////////////////////////////////////////////

#ifdef ALL_IMG
#define DOWNSCALE_KERNEL "                                                                            \
	__kernel void Downscale(int width, int height, int stride, __write_only image2d_t out,            \
							  __read_only image2d_t texFine) {                                        \
		const int ix = get_global_id(0);                                                              \
		const int iy = get_global_id(1);                                                              \
                                                                                                      \
		if (ix >= width || iy >= height) {                                                            \
			return;                                                                                   \
		}                                                                                             \
		sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR; \
                                                                                                      \
		float dx = 1.0f / (float)width;                                                               \
		float dy = 1.0f / (float)height;                                                              \
                                                                                                      \
		float x = ((float)ix + 0.5f) * dx;                                                            \
		float y = ((float)iy + 0.5f) * dy;                                                            \
                                                                                                      \
		write_imagef(out, (int2)(ix, iy), (float4)(0.25f * (read_imagef(texFine, smp, (float2)(x - dx * 0.25f, y)).x +   \
										 read_imagef(texFine, smp, (float2)(x + dx * 0.25f, y)).x +   \
										 read_imagef(texFine, smp, (float2)(x, y - dy * 0.25f)).x +   \
										 read_imagef(texFine, smp, (float2)(x, y + dy * 0.25f)).x), 0, 0, 0));   \
	} "
#else // ALL_IMG
#define DOWNSCALE_KERNEL "                                                                            \
	__kernel void Downscale(int width, int height, int stride, __global float *out,                   \
							  __read_only image2d_t texFine) {                                        \
		const int ix = get_global_id(0);                                                              \
		const int iy = get_global_id(1);                                                              \
                                                                                                      \
		if (ix >= width || iy >= height) {                                                            \
			return;                                                                                   \
		}                                                                                             \
		sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR; \
                                                                                                      \
		float dx = 1.0f / (float)width;                                                               \
		float dy = 1.0f / (float)height;                                                              \
                                                                                                      \
		float x = ((float)ix + 0.5f) * dx;                                                            \
		float y = ((float)iy + 0.5f) * dy;                                                            \
                                                                                                      \
		out[ix + iy * stride] = 0.25f * (read_imagef(texFine, smp, (float2)(x - dx * 0.25f, y)).x +   \
										 read_imagef(texFine, smp, (float2)(x + dx * 0.25f, y)).x +   \
										 read_imagef(texFine, smp, (float2)(x, y - dy * 0.25f)).x +   \
										 read_imagef(texFine, smp, (float2)(x, y + dy * 0.25f)).x);   \
	} "
#endif // ALL_IMG

///////////////////////////////////////////////////////////////////////////////
/// \brief downscale image
///
/// \param[in]  src     image to downscale
/// \param[in]  width   image width
/// \param[in]  height  image height
/// \param[in]  stride  image stride
/// \param[out] out     result
///////////////////////////////////////////////////////////////////////////////
static void Downscale(cl_context ctx, cl_command_queue queue, cl_kernel k_downscale, cl_mem src, int width, int height, int stride,
					  int newWidth, int newHeight, int newStride, cl_mem out) {

	size_t local_size[2]  = {32, 8};
	size_t global_size[2] = {iAlignUp(newWidth, local_size[0]), iAlignUp(newHeight, local_size[1])};

	#ifdef ALL_IMG
	cl_mem texFine = src;
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

	cl_mem texFine = clCreateImage(ctx, CL_MEM_READ_ONLY, &image_fmt, &image_desc, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	#endif // ALL_IMG

	OPENCL_ASSERT_OP(clSetKernelArg(k_downscale, 0, sizeof(int), &newWidth));
	OPENCL_ASSERT_OP(clSetKernelArg(k_downscale, 1, sizeof(int), &newHeight));
	OPENCL_ASSERT_OP(clSetKernelArg(k_downscale, 2, sizeof(int), &newStride));
	OPENCL_ASSERT_OP(clSetKernelArg(k_downscale, 3, sizeof(cl_mem), &out));
	OPENCL_ASSERT_OP(clSetKernelArg(k_downscale, 4, sizeof(cl_mem), &texFine));

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queue, k_downscale, 2, NULL, global_size, local_size, 0, NULL, NULL);)
}
