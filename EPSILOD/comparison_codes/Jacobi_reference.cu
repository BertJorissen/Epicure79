/*
 * Jacobi_reference.cu
 * 	Stencil code: Jacobi 2D for the heat equation. Implemented as a 2D cellular automata.
 * 	CUDA reference.
 *
 * v1.0
 * (c) 2007-2021, Arturo Gonzalez-Escribano, Manuel de Castro Caballero
 */

/*
 * <license>
 *
 * Hitmap v1.3
 *
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2007-2019, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * More information on http://trasgo.infor.uva.es/
 *
 * </license>
 */

#include <stdio.h>
#include <stdlib.h>

#define access(mat, i, j)   mat[(i)*COLS + j]
#define access_d(mat, i, j) mat[(i)*COLS_d + j]

extern "C" __global__ void traverseDataCUDA(float *mat, float *copy, int ROWS_d, int COLS_d) {
	int i = threadIdx.y + blockIdx.y * blockDim.y;
	int j = threadIdx.x + blockIdx.x * blockDim.x;

	if (i < 1 || i >= ROWS_d - 1 || j < 1 || j >= COLS_d - 1) return;

	// Compute iteration
	access_d(mat, i, j) =
		(access_d(copy, i - 1, j) +
		 access_d(copy, i + 1, j) +
		 access_d(copy, i, j - 1) +
		 access_d(copy, i, j + 1)) /
		4;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <rows> <columns> <numIter>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int ROWS   = atoi(argv[1]);
	int COLS   = atoi(argv[2]);
	int STAGES = atoi(argv[3]);

	float *mat;
	cudaMallocHost(&mat, ROWS * COLS * sizeof(float));
	float *copy;
	cudaMallocHost(&copy, ROWS * COLS * sizeof(float));
	float *mat_d;
	cudaMalloc(&mat_d, ROWS * COLS * sizeof(float));
	float *copy_d;
	cudaMalloc(&copy_d, ROWS * COLS * sizeof(float));

	int i, j;

	// Init
	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++) {
			access(mat, i, j)  = 0.0;
			access(copy, i, j) = 0.0;
		}

	for (i = 0; i < ROWS; i++) {
		access(mat, i, 0)         = 3;
		access(copy, i, 0)        = 3;
		access(mat, i, COLS - 1)  = 4;
		access(copy, i, COLS - 1) = 4;
	}

	for (j = 0; j < COLS; j++) {
		access(mat, 0, j)         = 1;
		access(copy, 0, j)        = 1;
		access(mat, ROWS - 1, j)  = 2;
		access(copy, ROWS - 1, j) = 2;
	}

	cudaMemcpy(mat_d, mat, ROWS * COLS * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(copy_d, copy, ROWS * COLS * sizeof(float), cudaMemcpyHostToDevice);

	// Compute
	int stage;
	for (stage = 0; stage < STAGES; stage++) {
		traverseDataCUDA<<<dim3(COLS / 64 + 1, ROWS / 8 + 1, 1), dim3(64, 8, 1)>>>(mat_d, copy_d, ROWS, COLS);
		float *tmp = mat_d;
		mat_d      = copy_d;
		copy_d     = tmp;
	}

	// End
	cudaMemcpy(mat, mat_d, ROWS * COLS * sizeof(float), cudaMemcpyDeviceToHost);
	return (int)mat[ROWS / 2 * COLS + COLS / 2];
}
