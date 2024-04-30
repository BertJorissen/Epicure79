#include "../../../Utils/opencl_helper.h"
#include "../common.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief warp image with a given displacement field, OpenCL kernel.
/// \param[in]  width   image width
/// \param[in]  height  image height
/// \param[in]  stride  image stride
/// \param[in]  u       horizontal displacement
/// \param[in]  v       vertical displacement
/// \param[out] out     result
///////////////////////////////////////////////////////////////////////////////
#ifdef ALL_IMG
#define WARP_KERNEL "                                                                                 \
	__kernel void Warp(int width, int height, int stride, __read_only image2d_t u,                    \
					   __read_only image2d_t v, __write_only image2d_t out,                           \
					   __read_only image2d_t texToWarp) {                                             \
		const int ix = get_global_id(0);                                                              \
		const int iy = get_global_id(1);                                                              \
                                                                                                      \
		const int pos = ix + iy * stride;                                                             \
		const int2 pos_i = (int2)(ix, iy);                                                            \
                                                                                                      \
		if (ix >= width || iy >= height) return;                                                      \
                                                                                                      \
		sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR; \
                                                                                                      \
		float x = ((float)ix + read_imagef(u, pos_i).x + 0.5f) / (float)width;                        \
		float y = ((float)iy + read_imagef(v, pos_i).x + 0.5f) / (float)height;                       \
                                                                                                      \
		write_imagef(out, pos_i, read_imagef(texToWarp, smp, (float2)(x, y)));                        \
	} "
#else // ALL_IMG
#define WARP_KERNEL "                                                                                 \
	__kernel void Warp(int width, int height, int stride, __global const float *u,                    \
					   __global const float *v, __global float *out,                                  \
					   __read_only image2d_t texToWarp) {                                             \
		const int ix = get_global_id(0);                                                              \
		const int iy = get_global_id(1);                                                              \
                                                                                                      \
		const int pos = ix + iy * stride;                                                             \
                                                                                                      \
		if (ix >= width || iy >= height) return;                                                      \
                                                                                                      \
		sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR; \
                                                                                                      \
		float x = ((float)ix + u[pos] + 0.5f) / (float)width;                                         \
		float y = ((float)iy + v[pos] + 0.5f) / (float)height;                                        \
                                                                                                      \
		out[pos] = read_imagef(texToWarp, smp, (float2)(x, y)).x;                                     \
	} "
#endif // ALL_IMG

///////////////////////////////////////////////////////////////////////////////
/// \brief warp image with provided vector field, CUDA kernel wrapper.
///
/// For each output pixel there is a vector which tells which pixel
/// from a source image should be mapped to this particular output
/// pixel.
/// It is assumed that images and the vector field have the same stride and
/// resolution.
/// \param[in]  src source image
/// \param[in]  w   width
/// \param[in]  h   height
/// \param[in]  s   stride
/// \param[in]  u   horizontal displacement
/// \param[in]  v   vertical displacement
/// \param[out] out warped image
///////////////////////////////////////////////////////////////////////////////
static void WarpImage(cl_context ctx, cl_command_queue queue, cl_kernel k_warp, cl_mem src, int w, int h, int s,
					  cl_mem u, cl_mem v, cl_mem out) {

	size_t local_size[2]  = {32, 6};
	size_t global_size[2] = {iAlignUp(w, local_size[0]), iAlignUp(h, local_size[1])};

	#ifdef ALL_IMG
	cl_mem texToWarp = src;
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
	image_desc.buffer            = src;

	cl_mem texToWarp = clCreateImage(ctx, CL_MEM_READ_ONLY, &image_fmt, &image_desc, NULL, &err);
	OPENCL_ASSERT_ERROR(err);
	#endif // ALL_IMG

	OPENCL_ASSERT_OP(clSetKernelArg(k_warp, 0, sizeof(int), &w));
	OPENCL_ASSERT_OP(clSetKernelArg(k_warp, 1, sizeof(int), &h));
	OPENCL_ASSERT_OP(clSetKernelArg(k_warp, 2, sizeof(int), &s));
	OPENCL_ASSERT_OP(clSetKernelArg(k_warp, 3, sizeof(cl_mem), &u));
	OPENCL_ASSERT_OP(clSetKernelArg(k_warp, 4, sizeof(cl_mem), &v));
	OPENCL_ASSERT_OP(clSetKernelArg(k_warp, 5, sizeof(cl_mem), &out));
	OPENCL_ASSERT_OP(clSetKernelArg(k_warp, 6, sizeof(cl_mem), &texToWarp));

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queue, k_warp, 2, NULL, global_size, local_size, 0, NULL, NULL));
}
