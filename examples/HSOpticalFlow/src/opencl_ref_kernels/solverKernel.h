#include "../../../Utils/opencl_helper.h"
#include "../common.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief one iteration of classical Horn-Schunck method, CUDA kernel.
///
/// It is one iteration of Jacobi method for a corresponding linear system.
/// \param[in]  du0     current horizontal displacement approximation
/// \param[in]  dv0     current vertical displacement approximation
/// \param[in]  Ix      image x derivative
/// \param[in]  Iy      image y derivative
/// \param[in]  Iz      temporal derivative
/// \param[in]  w       width
/// \param[in]  h       height
/// \param[in]  s       stride
/// \param[in]  alpha   degree of smoothness
/// \param[out] du1     new horizontal displacement approximation
/// \param[out] dv1     new vertical displacement approximation
///////////////////////////////////////////////////////////////////////////////
#define SOLVE_KERNEL "                                                               \
	#define MAX(x, y) (((x) > (y)) ? (x) : (y))                                    \n\
	#define MIN(x, y) (((x) < (y)) ? (x) : (y))                                    \n\
	__kernel void Solve(__global const float *du0, __global const float *dv0,        \
						__global const float *Ix, __global const float *Iy,          \
						__global const float *Iz, int w, int h, int s,               \
						float alpha, __global float *du1, __global float *dv1) {     \
                                                                                     \
		volatile __local float du[(bx + 2) * (by + 2)];                              \
		volatile __local float dv[(bx + 2) * (by + 2)];                              \
                                                                                     \
		const int ix = get_global_id(0);                                             \
		const int iy = get_global_id(1);                                             \
                                                                                     \
		/* position within global memory array */                                    \
		const int pos = MIN(ix, w - 1) + MIN(iy, h - 1) * s;                         \
                                                                                     \
		/* position within shared memory array */                                    \
		const int shMemPos = get_local_id(0) + 1 + (get_local_id(1) + 1) * (bx + 2); \
                                                                                     \
		/* Load data to shared memory.                                               \
		load tile being processed */                                                 \
		du[shMemPos] = du0[pos];                                                     \
		dv[shMemPos] = dv0[pos];                                                     \
                                                                                     \
		/* load necessary neighbouring elements                                      \
		We clamp out-of-range coordinates.                                           \
		It is equivalent to mirroring                                                \
		because we access data only one step away from borders. */                   \
		if (get_local_id(1) == 0) {                                                  \
			/* beginning of the tile */                                              \
			const int bsx = get_group_id(0) * get_local_size(0);                     \
			const int bsy = get_group_id(1) * get_local_size(1);                     \
			/* element position within matrix */                                     \
			int x, y;                                                                \
			/* element position within linear array */                               \
			/* gm - global memory */                                                 \
			/* sm - shared memory */                                                 \
			int gmPos, smPos;                                                        \
                                                                                     \
			x = MIN(bsx + get_local_id(0), w - 1);                                   \
			/* row just below the tile */                                            \
			y         = MAX(bsy - 1, 0);                                             \
			gmPos     = y * s + x;                                                   \
			smPos     = get_local_id(0) + 1;                                         \
			du[smPos] = du0[gmPos];                                                  \
			dv[smPos] = dv0[gmPos];                                                  \
                                                                                     \
			/* row above the tile */                                                 \
			y = MIN(bsy + by, h - 1);                                                \
			smPos += (by + 1) * (bx + 2);                                            \
			gmPos     = y * s + x;                                                   \
			du[smPos] = du0[gmPos];                                                  \
			dv[smPos] = dv0[gmPos];                                                  \
		} else if (get_local_id(1) == 1) {                                           \
			/* beginning of the tile */                                              \
			const int bsx = get_group_id(0) * get_local_size(0);                     \
			const int bsy = get_group_id(1) * get_local_size(1);                     \
			/* element position within matrix */                                     \
			int x, y;                                                                \
			/* element position within linear array */                               \
			/* gm - global memory */                                                 \
			/* sm - shared memory */                                                 \
			int gmPos, smPos;                                                        \
                                                                                     \
			y = MIN(bsy + get_local_id(0), h - 1);                                   \
			/* column to the left */                                                 \
			x     = MAX(bsx - 1, 0);                                                 \
			smPos = bx + 2 + get_local_id(0) * (bx + 2);                             \
			gmPos = x + y * s;                                                       \
                                                                                     \
			/* check if we are within tile */                                        \
			if (get_local_id(0) < by) {                                              \
				du[smPos] = du0[gmPos];                                              \
				dv[smPos] = dv0[gmPos];                                              \
				/* column to the right */                                            \
				x     = MIN(bsx + bx, w - 1);                                        \
				gmPos = y * s + x;                                                   \
				smPos += bx + 1;                                                     \
				du[smPos] = du0[gmPos];                                              \
				dv[smPos] = dv0[gmPos];                                              \
			}                                                                        \
		}                                                                            \
                                                                                     \
		barrier(CLK_LOCAL_MEM_FENCE);                                                \
                                                                                     \
		if (ix >= w || iy >= h) return;                                              \
                                                                                     \
		/* now all necessary data are loaded to shared memory */                     \
		int left, right, up, down;                                                   \
		left  = shMemPos - 1;                                                        \
		right = shMemPos + 1;                                                        \
		up    = shMemPos + bx + 2;                                                   \
		down  = shMemPos - bx - 2;                                                   \
                                                                                     \
		float sumU = (du[left] + du[right] + du[up] + du[down]) * 0.25f;             \
		float sumV = (dv[left] + dv[right] + dv[up] + dv[down]) * 0.25f;             \
                                                                                     \
		float frac = (Ix[pos] * sumU + Iy[pos] * sumV + Iz[pos]) /                   \
					 (Ix[pos] * Ix[pos] + Iy[pos] * Iy[pos] + alpha);                \
                                                                                     \
		du1[pos] = sumU - Ix[pos] * frac;                                            \
		dv1[pos] = sumV - Iy[pos] * frac;                                            \
	} "

///////////////////////////////////////////////////////////////////////////////
/// \brief one iteration of classical Horn-Schunck method, CUDA kernel wrapper.
///
/// It is one iteration of Jacobi method for a corresponding linear system.
/// \param[in]  du0     current horizontal displacement approximation
/// \param[in]  dv0     current vertical displacement approximation
/// \param[in]  Ix      image x derivative
/// \param[in]  Iy      image y derivative
/// \param[in]  Iz      temporal derivative
/// \param[in]  w       width
/// \param[in]  h       height
/// \param[in]  s       stride
/// \param[in]  alpha   degree of smoothness
/// \param[out] du1     new horizontal displacement approximation
/// \param[out] dv1     new vertical displacement approximation
///////////////////////////////////////////////////////////////////////////////
static void SolveForUpdate(cl_command_queue queue, cl_kernel k_solve, cl_mem du0, cl_mem dv0,
						   cl_mem Ix, cl_mem Iy, cl_mem Iz, int w, int h, int s,
						   float alpha, cl_mem du1, cl_mem dv1) {
	size_t local_size[2]  = {32, 6};
	size_t global_size[2] = {iAlignUp(w, local_size[0]), iAlignUp(h, local_size[1])};

	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 0, sizeof(cl_mem), &du0));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 1, sizeof(cl_mem), &dv0));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 2, sizeof(cl_mem), &Ix));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 3, sizeof(cl_mem), &Iy));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 4, sizeof(cl_mem), &Iz));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 5, sizeof(int), &w));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 6, sizeof(int), &h));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 7, sizeof(int), &s));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 8, sizeof(float), &alpha));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 9, sizeof(cl_mem), &du1));
	OPENCL_ASSERT_OP(clSetKernelArg(k_solve, 10, sizeof(cl_mem), &dv1));

	OPENCL_ASSERT_OP(clEnqueueNDRangeKernel(queue, k_solve, 2, NULL, global_size, local_size, 0, NULL, NULL));
}
