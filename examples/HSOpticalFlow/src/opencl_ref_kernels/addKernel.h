#include "../../../Utils/opencl_helper.h"
#include "../common.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief add two vectors of size _count_
///
/// Opencl kernel
/// \param[in]  op1   term one
/// \param[in]  op2   term two
/// \param[in]  count vector size
/// \param[out] sum   result
///////////////////////////////////////////////////////////////////////////////
#ifdef ALL_IMG
#define ADD_KERNEL \
	"__kernel void Add(__read_only image2d_t op1, __global const float *op2, int width, int height, int stride, __write_only image2d_t sum) { \
		const int ix = get_global_id(0);                                                                                                      \
		const int iy = get_global_id(1);                                                                                                      \
                                                                                                                                              \
		const int pos_b = ix + iy * stride;                                                                                                   \
		const int2 pos_i =(int2)(ix, iy);                                                                                                     \
                                                                                                                                              \
		if (ix >= width || iy >= height) return;                                                                                              \
		write_imagef(sum, pos_i, (float4)(read_imagef(op1, pos_i).x + op2[pos_b], 0, 0, 0));                                                  \
	}"
#else // ALL_IMG
#define ADD_KERNEL \
	"__kernel void Add(__global const float *op1, __global const float *op2, int count, __global float *sum) { \
		const int pos = get_global_id(0);                                                                      \
		if (pos >= count) return;                                                                              \
		sum[pos] = op1[pos] + op2[pos];                                                                        \
	}"
#endif // ALL_IMG

///////////////////////////////////////////////////////////////////////////////
/// \brief add two vectors of size _count_
///
/// \note args must be different in the cl img version
///
/// \param[in]  op1   term one
/// \param[in]  op2   term two
/// \param[in]  count vector size
/// \param[out] sum   result
///////////////////////////////////////////////////////////////////////////////
static void Add(cl_command_queue queue, cl_kernel k_add, cl_mem op1, cl_mem op2, int width, int height, int stride, cl_mem sum) {

	#ifdef ALL_IMG
	size_t local_size[3]  = {32, 8};
	size_t global_size[2] = {iAlignUp(width, local_size[0]), iAlignUp(height, local_size[1])};
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 0, sizeof(cl_mem), &op1));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 1, sizeof(cl_mem), &op2));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 2, sizeof(int), &width));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 3, sizeof(int), &height));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 4, sizeof(int), &stride));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 5, sizeof(cl_mem), &sum));

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queue, k_add, 2, NULL, global_size, local_size, 0, NULL, NULL));
	#else // ALL_IMG
	size_t local_size  = 256;
	int    count       = height * stride;
	size_t global_size = iAlignUp(count, local_size);
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 0, sizeof(cl_mem), &op1));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 1, sizeof(cl_mem), &op2));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 2, sizeof(int), &count));
	OPENCL_ASSERT_OP(clSetKernelArg(k_add, 3, sizeof(cl_mem), &sum));

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queue, k_add, 1, NULL, &global_size, &local_size, 0, NULL, NULL));
	#endif // ALL_IMG
}
