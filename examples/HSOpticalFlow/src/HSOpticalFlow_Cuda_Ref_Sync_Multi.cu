/**
 * @file HSOpticalFlow_Cuda_Ref_Multi.cu
 * @author Trasgo Group
 * @brief HSOpticalFlow: Multiframe native CUDA version
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

/* Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cuda_runtime.h>
#include <omp.h>

#include "../../Utils/cuda_helper.h"
#include "../../Utils/profiler_utils.h"
#include "helper_image.h"

#include "cuda_ref_kernels/addKernel.cuh"
#include "cuda_ref_kernels/derivativesKernel.cuh"
#include "cuda_ref_kernels/downscaleKernel.cuh"
#include "cuda_ref_kernels/solverKernel.cuh"
#include "cuda_ref_kernels/upscaleKernel.cuh"
#include "cuda_ref_kernels/warpingKernel.cuh"

double main_clock;
double exec_clock;

///////////////////////////////////////////////////////////////////////////////
/// \brief method logic
///
/// handles memory allocations, control flow
/// \param[in]  I0             source image
/// \param[in]  I1             tracked image
/// \param[in]  width          images width
/// \param[in]  height         images height
/// \param[in]  stride         images stride
/// \param[in]  alpha          degree of displacement field smoothness
/// \param[in]  nLevels        number of levels in a pyramid
/// \param[in]  nWarpIters     number of warping iterations per pyramid level
/// \param[in]  nSolverIters   number of solver iterations (Jacobi iterations)
/// \param[out] u              horizontal displacement
/// \param[out] v              vertical displacement
/// \param      pI0,pI1        downscaled images
/// \param      pW,pH,pS       downscaled images
/// \param      dtmp           image after warping
/// \param      d_du0,d_dv0    ancillary for jacobi iterations
/// \param      d_du1,d_dv1    ancillary for jacobi iterations
/// \param      d_nu,d_nv      ancillary for upscaling
/// \param      d_Ix,d_Iy,d_Iz derivatives
///////////////////////////////////////////////////////////////////////////////
void ComputeFlowCUDA(const float *I0, const float *I1, int width, int height, int stride,
					 float alpha, int nLevels, int nWarpIters, int nSolverIters, float *u, float *v,
					 const float **pI0, const float **pI1, int *pW, int *pH, int *pS,
					 float *d_tmp, float *d_du0, float *d_dv0, float *d_du1, float *d_dv1,
					 float *d_Ix, float *d_Iy, float *d_Iz,
					 float *d_u, float *d_v, float *d_nu, float *d_nv) {

	const int dataSize = stride * height * sizeof(float);

	// prepare pyramid
	int currentLevel = nLevels - 1;

	CUDA_OP(cudaMemcpy((void *)pI0[currentLevel], I0, dataSize, cudaMemcpyHostToDevice));
	CUDA_OP(cudaMemcpy((void *)pI1[currentLevel], I1, dataSize, cudaMemcpyHostToDevice));

	for (; currentLevel > 0; --currentLevel) {
		Downscale(pI0[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], (float *)pI0[currentLevel - 1]);
		Downscale(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], (float *)pI1[currentLevel - 1]);
	}

	CUDA_OP(cudaMemset(d_u, 0, stride * height * sizeof(float)));
	CUDA_OP(cudaMemset(d_v, 0, stride * height * sizeof(float)));

	// compute flow
	for (; currentLevel < nLevels; ++currentLevel) {
		for (int warpIter = 0; warpIter < nWarpIters; ++warpIter) {
			CUDA_OP(cudaMemset(d_du0, 0, dataSize));
			CUDA_OP(cudaMemset(d_dv0, 0, dataSize));

			CUDA_OP(cudaMemset(d_du1, 0, dataSize));
			CUDA_OP(cudaMemset(d_dv1, 0, dataSize));

			// on current level we compute optical flow
			// between frame 0 and warped frame 1
			WarpImage(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], d_u, d_v, d_tmp);

			ComputeDerivatives(pI0[currentLevel], d_tmp, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], d_Ix, d_Iy, d_Iz);

			for (int iter = 0; iter < nSolverIters; ++iter) {
				SolveForUpdate(d_du0, d_dv0, d_Ix, d_Iy, d_Iz, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], alpha, d_du1, d_dv1);

				Swap(d_du0, d_du1);
				Swap(d_dv0, d_dv1);
			}

			// update u, v
			Add(d_u, d_du0, pH[currentLevel] * pS[currentLevel], d_u);
			Add(d_v, d_dv0, pH[currentLevel] * pS[currentLevel], d_v);
		}

		if (currentLevel != nLevels - 1) {
			// prolongate solution
			float scaleX = (float)pW[currentLevel + 1] / (float)pW[currentLevel];

			Upscale(d_u, pW[currentLevel], pH[currentLevel], pS[currentLevel],
					pW[currentLevel + 1], pH[currentLevel + 1], pS[currentLevel + 1],
					scaleX, d_nu);

			float scaleY = (float)pH[currentLevel + 1] / (float)pH[currentLevel];

			Upscale(d_v, pW[currentLevel], pH[currentLevel], pS[currentLevel],
					pW[currentLevel + 1], pH[currentLevel + 1], pS[currentLevel + 1],
					scaleY, d_nv);

			Swap(d_u, d_nu);
			Swap(d_v, d_nv);
		}
	}

	CUDA_OP(cudaMemcpy(u, d_u, dataSize, cudaMemcpyDeviceToHost));
	CUDA_OP(cudaMemcpy(v, d_v, dataSize, cudaMemcpyDeviceToHost));
}

///////////////////////////////////////////////////////////////////////////////
/// load 4-channel unsigned byte image
/// and convert it to single channel FP32 image
/// \param[in] img_data   pointer to raw image data
/// \param[in] img_s      image row stride
/// \param[in] video_path path to video directory
/// \param[in] frame      frame to load
/// \return true if image is successfully loaded or false otherwise
///////////////////////////////////////////////////////////////////////////////
bool LoadImageAsFP32(float *img_data, int img_s, const char *video_path, int frame) {
	PROF_NVTX_RANGEPUSH("Load Image");

	unsigned char *data = 0;
	unsigned int   w = 0, h = 0;

	char frame_path[strlen(video_path) + 9];
	sprintf(frame_path, "%s/%03d.ppm", video_path, frame);

	bool result = sdkLoadPPM4ub(frame_path, &data, &w, &h);

	if (result == false) {
		printf("Invalid file format on %s\n", video_path);
		return false;
	}

	PROF_NVTX_RANGEPUSH("Init Image");

	// source is 4 channel image
	const int channels = 4;

	for (size_t i = 0; i < h; ++i) {
		for (size_t j = 0; j < w; ++j) {
			// only keep first component
			img_data[j + i * img_s] = ((float)data[j * channels + i * channels * w]) / 255.0f;
		}
	}

	free(data);

	PROF_NVTX_RANGEPOP();
	PROF_NVTX_RANGEPOP();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief compute and print a norm (L1 norm) of the flow field for easier result checking
/// \param[in] width    optical flow field width
/// \param[in] height   optical flow field height
/// \param[in] stride   optical flow field row stride
/// \param[in] h_u      horizontal displacement
/// \param[in] h_v      vertical displacement
///////////////////////////////////////////////////////////////////////////////
void normCalc(int width, int height, int stride, const float *h_u, const float *h_v, double *p_sum, double *p_res, int frame) {
	PROF_NVTX_RANGEPUSH("Norm calc")

	double sum = 0;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			const int pos = j + i * stride;
			sum += fabsf(h_u[pos]) + fabsf(h_v[pos]);
		}
	}

	// -2 because frame refers to the target frame and frames start on 1
	p_sum[frame - 2] = sum;
	p_res[frame - 2] = sum / (double)(width * height);

	PROF_NVTX_RANGEPOP();
}

///////////////////////////////////////////////////////////////////////////////
/// application entry point
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 8) {
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <GPU> <nFrames> <frameDir> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const float alpha        = atof(argv[1]); // smoothness if image brightness is not within [0,1] this paramter should be scaled appropriately (0.2f)
	const int   nLevels      = atoi(argv[2]); // number of pyramid levels (5)
	const int   nSolverIters = atoi(argv[3]); // number of solver iterations on each level (500)
	const int   nWarpIters   = atoi(argv[4]); // number of warping iterations (3)
	const int   GPU          = atoi(argv[5]);
	const int   n_frames     = atoi(argv[6]); // number of frames
	// find images
	const char *video_path = argv[7]; // path to directory containing the frames

	CUDA_OP(cudaSetDevice(GPU));
	cudaDeviceProp cu_dev_prop;
	CUDA_OP(cudaGetDeviceProperties(&cu_dev_prop, GPU));
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, ", cu_dev_prop.name);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n ALPHA: %g", alpha);
	printf("\n LEVELS: %d", nLevels);
	printf("\n SOLVERITERS: %d", nSolverIters);
	printf("\n WARPITERS: %d", nWarpIters);
	printf("\n FRAMES: %d", n_frames);
	printf("\n FRAMES PATH: %s", video_path);
	printf("\n POLICY: Sync");
	printf("\n DEVICE: %s", cu_dev_prop.name);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	// get image dimensions
	char           frame_path[strlen(video_path) + 9];
	unsigned int   channels, width, height;
	unsigned char *aux = NULL;
	if (!__loadPPM(strcat(strcpy(frame_path, video_path), "/001.ppm"), &aux, &width, &height, &channels)) {
		exit(EXIT_FAILURE);
	}
	free(aux);
	// row access stride
	int stride = iAlignUp(width);

	// allocate host memory
	float *h_source = new float[stride * height];
	float *h_target = new float[stride * height];

	float *h_u = new float[stride * height];
	float *h_v = new float[stride * height];

	// allocate device memory
	const float **pI0 = new const float *[nLevels];
	const float **pI1 = new const float *[nLevels];

	// device memory pointers
	float *d_tmp;
	float *d_du0;
	float *d_dv0;
	float *d_du1;
	float *d_dv1;

	float *d_Ix;
	float *d_Iy;
	float *d_Iz;

	float *d_u;
	float *d_v;
	float *d_nu;
	float *d_nv;

	const int dataSize = stride * height * sizeof(float);

	CUDA_OP(cudaMalloc(&d_tmp, dataSize));
	CUDA_OP(cudaMalloc(&d_du0, dataSize));
	CUDA_OP(cudaMalloc(&d_dv0, dataSize));
	CUDA_OP(cudaMalloc(&d_du1, dataSize));
	CUDA_OP(cudaMalloc(&d_dv1, dataSize));

	CUDA_OP(cudaMalloc(&d_Ix, dataSize));
	CUDA_OP(cudaMalloc(&d_Iy, dataSize));
	CUDA_OP(cudaMalloc(&d_Iz, dataSize));

	CUDA_OP(cudaMalloc(&d_u, dataSize));
	CUDA_OP(cudaMalloc(&d_v, dataSize));
	CUDA_OP(cudaMalloc(&d_nu, dataSize));
	CUDA_OP(cudaMalloc(&d_nv, dataSize));

	// prepare pyramid

	int currentLevel = nLevels - 1;
	// allocate GPU memory for input images
	CUDA_OP(cudaMalloc(pI0 + currentLevel, dataSize));
	CUDA_OP(cudaMalloc(pI1 + currentLevel, dataSize));

	// sizes of downscaled images
	int *pW = new int[nLevels];
	int *pH = new int[nLevels];
	int *pS = new int[nLevels];

	pW[currentLevel] = width;
	pH[currentLevel] = height;
	pS[currentLevel] = stride;

	for (; currentLevel > 0; --currentLevel) {
		int nw = pW[currentLevel] / 2;
		int nh = pH[currentLevel] / 2;
		int ns = iAlignUp(nw);

		CUDA_OP(cudaMalloc(pI0 + currentLevel - 1, ns * nh * sizeof(float)));
		CUDA_OP(cudaMalloc(pI1 + currentLevel - 1, ns * nh * sizeof(float)));

		pW[currentLevel - 1] = nw;
		pH[currentLevel - 1] = nh;
		pS[currentLevel - 1] = ns;
	}

	double *p_sum = new double[n_frames - 1];
	double *p_res = new double[n_frames - 1];

	CUDA_OP(cudaDeviceSynchronize());
	exec_clock = omp_get_wtime();

	// load first frame
	if (!LoadImageAsFP32(h_source, stride, video_path, 1)) {
		exit(EXIT_FAILURE);
	}
	for (int i = 2; i < n_frames + 1; i++) {
		if (!LoadImageAsFP32(h_target, stride, video_path, i)) {
			exit(EXIT_FAILURE);
		}

		ComputeFlowCUDA(h_source, h_target, width, height, stride, alpha, nLevels, nWarpIters, nSolverIters, h_u, h_v,
						pI0, pI1, pW, pH, pS, d_tmp, d_du0, d_dv0, d_du1, d_dv1, d_Ix, d_Iy, d_Iz, d_u, d_v, d_nu, d_nv);

		// compare results (L1 norm)
		normCalc(width, height, stride, h_u, h_v, p_sum, p_res, i);

		Swap(h_source, h_target);
	}

	CUDA_OP(cudaDeviceSynchronize());
	exec_clock = omp_get_wtime() - exec_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", p_sum[n_frames - 2], p_res[n_frames - 2]);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- NORM ----------------------- \n\n");
	for (int i = 0; i < n_frames - 1; i++) {
		printf(" frames: %d-%d, sum: %lf, res: %lf\n", i + 1, i + 2, p_sum[i], p_res[i]);
	}
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	// free resources
	for (int i = 0; i < nLevels; ++i) {
		CUDA_OP(cudaFree((void *)pI0[i]));
		CUDA_OP(cudaFree((void *)pI1[i]));
	}

	delete[] pI0;
	delete[] pI1;
	delete[] pW;
	delete[] pH;
	delete[] pS;

	CUDA_OP(cudaFree(d_tmp));
	CUDA_OP(cudaFree(d_du0));
	CUDA_OP(cudaFree(d_dv0));
	CUDA_OP(cudaFree(d_du1));
	CUDA_OP(cudaFree(d_dv1));
	CUDA_OP(cudaFree(d_Ix));
	CUDA_OP(cudaFree(d_Iy));
	CUDA_OP(cudaFree(d_Iz));
	CUDA_OP(cudaFree(d_nu));
	CUDA_OP(cudaFree(d_nv));
	CUDA_OP(cudaFree(d_u));
	CUDA_OP(cudaFree(d_v));

	delete[] h_u;
	delete[] h_v;

	delete[] h_source;
	delete[] h_target;

	delete[] p_sum;
	delete[] p_res;

	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	return EXIT_SUCCESS;
}