/**
 * @file Chain_MatMult_Ctrl_Kernels.c
 * @author Trasgo Group
 * @brief Chain matrix multiplication: Ctrl kernel file
 * @version 4.0
 * @date 2021-07-31
 *
 * @copyright This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @copyright Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * @copyright More information on http://trasgo.infor.uva.es/
 */

#include "Ctrl.h"
#include <math.h>

#ifdef _PROFILING_ENABLED_
#ifdef _CTRL_ARCH_CUDA_
#include "nvToolsExt.h"
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_OPENCL_GPU_
#include <roctx.h>
#endif // _CTRL_ARCH_OPENCL_GPU_
#endif //_PROFILING_ENABLED_

// TODO @sergioalo this should be on common header included by both kernel file and main file
#define mult_params 3, OUT, HitTile_float, matrix_result, IN, HitTile_float, matrix_a, IN, HitTile_float, matrix_b
#define norm_calc   4, INVAL, int, ITER, INVAL, double *, p_sum, INVAL, double *, p_res, IN, HitTile_float, matrix

Ctrl_NewType(float);

CTRL_KERNEL(Mult, OPENCLGPU, DEFAULT, CTRL_KPARAMS(mult_params), {
	__local float tile_a[BLOCKSIZE][BLOCKSIZE];
	__local float tile_b[BLOCKSIZE][BLOCKSIZE];

	int n_groups_x = get_num_groups(0);

	int local_size = get_local_size(0);

	int local_x = get_local_id(0);
	int local_y = get_local_id(1);

	float tmp = 0.0;
	int   idx;

	for (int sub = 0; sub < n_groups_x; ++sub) {
		idx                      = thr_i * hit_tileDimCard(matrix_a, 0) + sub * local_size + local_x;
		tile_a[local_y][local_x] = hit(matrix_a, idx);
		idx                      = (sub * local_size + local_y) * hit_tileDimCard(matrix_a, 0) + thr_j;
		tile_b[local_y][local_x] = hit(matrix_b, idx);
		barrier(CLK_LOCAL_MEM_FENCE);

		for (int k = 0; k < local_size; ++k) {
			tmp += tile_a[local_y][k] * tile_b[k][local_x];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	hit(matrix_result, thr_i, thr_j) = tmp;
});

CTRL_KERNEL(Mult, CUDA, DEFAULT, CTRL_KPARAMS(mult_params), {
	__shared__ float tile_a[BLOCKSIZE][BLOCKSIZE];
	__shared__ float tile_b[BLOCKSIZE][BLOCKSIZE];

	float tmp = 0.0;
	int   idx;

	for (int sub = 0; sub < gridDim.x; ++sub) {
		idx                              = thr_i * hit_tileDimCard(matrix_a, 0) + sub * BLOCKSIZE + threadIdx.x;
		tile_a[threadIdx.y][threadIdx.x] = hit(matrix_a, idx);
		idx                              = (sub * BLOCKSIZE + threadIdx.y) * hit_tileDimCard(matrix_a, 0) + thr_j;
		tile_b[threadIdx.y][threadIdx.x] = hit(matrix_b, idx);
		__syncthreads();

		for (int k = 0; k < BLOCKSIZE; ++k) {
			tmp += tile_a[threadIdx.y][k] * tile_b[k][threadIdx.x];
		}
		__syncthreads();
	}

	hit(matrix_result, thr_i, thr_j) = tmp;
});

CTRL_KERNEL(Mult, CPU, DEFAULT, CTRL_KPARAMS(mult_params), {
	// using the same size to address all matrixes for compiler optimization issues
	int size = hit_tileDimCard(matrix_result, 0);
	matrix_result.data[thr_i * size + thr_k] += matrix_a.data[thr_i * size + thr_j] * matrix_b.data[thr_j * size + thr_k];
	// TODO @sergioalo test if hit_as version has the same performance
	// hit(matrix_result, thr_i, thr_k) += hit_as(matrix_a, matrix_result, thr_i, thr_j) * hit_as(matrix_b, matrix_result, thr_j, thr_k);
});

CTRL_KERNEL(Norm_calc, CPU, DEFAULT, CTRL_KPARAMS(norm_calc), {
	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePushA("Norm calc");
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePush("Norm calc");
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_

	double suma = 0;
	for (int i = 0; i < hit_tileDimCard(matrix, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(matrix, 1); j++) {
			suma += hit(matrix, i, j) * hit(matrix, i, j);
		}
	}
	p_sum[ITER] = suma;
	p_res[ITER] = sqrt(suma);

	#ifdef _PROFILING_ENABLED_
	#ifdef _CTRL_ARCH_CUDA_
	nvtxRangePop();
	#endif // _CTRL_ARCH_CUDA_

	#ifdef _CTRL_ARCH_OPENCL_GPU_
	roctxRangePop();
	#endif // _CTRL_ARCH_OPENCL_GPU_
	#endif //_PROFILING_ENABLED_
});
