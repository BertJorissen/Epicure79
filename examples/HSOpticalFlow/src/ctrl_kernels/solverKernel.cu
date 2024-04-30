#include "kernels_params.h"
#include <cooperative_groups.h>

// NOTE @sergioalo thread space for this kernel needs to be height+1 width+1

namespace cg = cooperative_groups;

#define bx SOLVECHAR_0
#define by SOLVECHAR_1

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
CTRL_KERNEL(Solve, CUDA, DEFAULT, CTRL_KPARAMS(solver_params), {
	// Handle to thread block group
	cg::thread_block cta = cg::this_thread_block();

	volatile __shared__ float du[(bx + 2) * (by + 2)];
	volatile __shared__ float dv[(bx + 2) * (by + 2)];

	// position within global memory array
	int w = ctrl_threads.j - 1;
	int h = ctrl_threads.i - 1;

	// position within shared memory array
	const int shMemPos = threadIdx.x + 1 + (threadIdx.y + 1) * (bx + 2);

	// Load data to shared memory.
	// load tile being processed
	du[shMemPos] = hit(du0, min(thr_i, h - 1), min(thr_j, w - 1));
	dv[shMemPos] = hit(dv0, min(thr_i, h - 1), min(thr_j, w - 1));

	// load necessary neighbouring elements
	// We clamp out-of-range coordinates.
	// It is equivalent to mirroring
	// because we access data only one step away from borders.
	if (threadIdx.y == 0) {
		// beginning of the tile
		const int bsx = blockIdx.x * blockDim.x;
		const int bsy = blockIdx.y * blockDim.y;
		// element position within matrix
		int x;
		int y;
		// element position within linear array
		// sm - shared memory
		int smPos;

		x = min(bsx + threadIdx.x, w - 1);
		// row just below the tile
		y         = max(bsy - 1, 0);
		smPos     = threadIdx.x + 1;
		du[smPos] = hit(du0, y, x);
		dv[smPos] = hit(dv0, y, x);

		// row above the tile
		y = min(bsy + by, h - 1);
		smPos += (by + 1) * (bx + 2);
		du[smPos] = hit(du0, y, x);
		dv[smPos] = hit(dv0, y, x);
	} else if (threadIdx.y == 1) {
		// beginning of the tile
		const int bsx = blockIdx.x * blockDim.x;
		const int bsy = blockIdx.y * blockDim.y;
		// element position within matrix
		int x;
		int y;
		// element position within linear array
		// sm - shared memory
		int smPos;

		y = min(bsy + threadIdx.x, h - 1);
		// column to the left
		x     = max(bsx - 1, 0);
		smPos = bx + 2 + threadIdx.x * (bx + 2);

		// check if we are within tile
		if (threadIdx.x < by) {
			du[smPos] = hit(du0, y, x);
			dv[smPos] = hit(dv0, y, x);
			// column to the right
			x = min(bsx + bx, w - 1);
			smPos += bx + 1;
			du[smPos] = hit(du0, y, x);
			dv[smPos] = hit(dv0, y, x);
		}
	}

	cg::sync(cta);

	if (thr_j >= w || thr_i >= h) return;

	// now all necessary data are loaded to shared memory
	int left  = shMemPos - 1;
	int right = shMemPos + 1;
	int up    = shMemPos + bx + 2;
	int down  = shMemPos - bx - 2;

	float sumU = (du[left] + du[right] + du[up] + du[down]) * 0.25f;
	float sumV = (dv[left] + dv[right] + dv[up] + dv[down]) * 0.25f;

	float frac = (hit(Ix, thr_i, thr_j) * sumU + hit(Iy, thr_i, thr_j) * sumV + hit(Iz, thr_i, thr_j)) /
				 (hit(Ix, thr_i, thr_j) * hit(Ix, thr_i, thr_j) + hit(Iy, thr_i, thr_j) * hit(Iy, thr_i, thr_j) + alpha);

	hit(du1, thr_i, thr_j) = sumU - hit(Ix, thr_i, thr_j) * frac;
	hit(dv1, thr_i, thr_j) = sumV - hit(Iy, thr_i, thr_j) * frac;
});

CTRL_KERNEL(Solve, OPENCLGPU, DEFAULT, CTRL_KPARAMS(solver_params), {
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
	volatile __local float du[(bx + 2) * (by + 2)];
	volatile __local float dv[(bx + 2) * (by + 2)];

	// position within global memory array
	int w = ctrl_threads.j - 1;
	int h = ctrl_threads.i - 1;

	// position within shared memory array
	const int shMemPos = get_local_id(0) + 1 + (get_local_id(1) + 1) * (bx + 2);

	// Load data to shared memory.
	// load tile being processed
	du[shMemPos] = hit(du0, MIN(thr_i, h - 1), MIN(thr_j, w - 1));
	dv[shMemPos] = hit(dv0, MIN(thr_i, h - 1), MIN(thr_j, w - 1));

	// load necessary neighbouring elements
	// We clamp out-of-range coordinates.
	// It is equivalent to mirroring
	// because we access data only one step away from borders.
	if (get_local_id(1) == 0) {
		// beginning of the tile
		const int bsx = get_group_id(0) * get_local_size(0);
		const int bsy = get_group_id(1) * get_local_size(1);
		// element position within matrix
		int x;
		int y;
		// element position within linear array
		// sm - shared memory
		int smPos;

		x = MIN(bsx + get_local_id(0), w - 1);
		// row just below the tile
		y         = MAX(bsy - 1, 0);
		smPos     = get_local_id(0) + 1;
		du[smPos] = hit(du0, y, x);
		dv[smPos] = hit(dv0, y, x);

		// row above the tile
		y = MIN(bsy + by, h - 1);
		smPos += (by + 1) * (bx + 2);
		du[smPos] = hit(du0, y, x);
		dv[smPos] = hit(dv0, y, x);
	} else if (get_local_id(1) == 1) {
		// beginning of the tile
		const int bsx = get_group_id(0) * get_local_size(0);
		const int bsy = get_group_id(1) * get_local_size(1);
		// element position within matrix
		int x;
		int y;
		// element position within linear array
		// sm - shared memory
		int smPos;

		y = MIN(bsy + get_local_id(0), h - 1);
		// column to the left
		x     = MAX(bsx - 1, 0);
		smPos = bx + 2 + get_local_id(0) * (bx + 2);

		// check if we are within tile
		if (get_local_id(0) < by) {
			du[smPos] = hit(du0, y, x);
			dv[smPos] = hit(dv0, y, x);
			// column to the right
			x = MIN(bsx + bx, w - 1);
			smPos += bx + 1;
			du[smPos] = hit(du0, y, x);
			dv[smPos] = hit(dv0, y, x);
		}
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	if (thr_j >= w || thr_i >= h) return;

	// now all necessary data are loaded to shared memory
	int left  = shMemPos - 1;
	int right = shMemPos + 1;
	int up    = shMemPos + bx + 2;
	int down  = shMemPos - bx - 2;

	float sumU = (du[left] + du[right] + du[up] + du[down]) * 0.25f;
	float sumV = (dv[left] + dv[right] + dv[up] + dv[down]) * 0.25f;

	float frac = (hit(Ix, thr_i, thr_j) * sumU + hit(Iy, thr_i, thr_j) * sumV + hit(Iz, thr_i, thr_j)) /
				 (hit(Ix, thr_i, thr_j) * hit(Ix, thr_i, thr_j) + hit(Iy, thr_i, thr_j) * hit(Iy, thr_i, thr_j) + alpha);

	hit(du1, thr_i, thr_j) = sumU - hit(Ix, thr_i, thr_j) * frac;
	hit(dv1, thr_i, thr_j) = sumV - hit(Iy, thr_i, thr_j) * frac;
});
