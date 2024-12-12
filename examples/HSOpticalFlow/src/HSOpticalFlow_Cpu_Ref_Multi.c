/**
 * @file HSOpticalFlow_Cpu_Ref_Multi.c
 * @brief HSOpticalFlow: Multiframe native CPU version. Parallelized using OpenMP.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 *
 * @copyright This file is part of a modified version of a CUDA sample. Thus the following applies:
 * @copyright Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
 *
 * @copyright Redistribution and use in source and binary forms, with or without
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
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
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

#include <stdbool.h>

#include <omp.h>

#include "../../Utils/profiler_utils.h"
#include "helper_image.h"
#include "common.h"

#include "cpu_ref_kernels/addKernel.h"
#include "cpu_ref_kernels/derivativesKernel.h"
#include "cpu_ref_kernels/downscaleKernel.h"
#include "cpu_ref_kernels/solverKernel.h"
#include "cpu_ref_kernels/upscaleKernel.h"
#include "cpu_ref_kernels/warpKernel.h"

#define ALIGNMENT 32

double main_clock;
double exec_clock;

///////////////////////////////////////////////////////////////////////////////
/// \brief method logic
///
/// handles control flow
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
/// \param      pW,pH,pS       image sizes
/// \param      tmp           image after warping
/// \param      du0,dv0    ancillary for jacobi iterations
/// \param      du1,dv1    ancillary for jacobi iterations
/// \param      nu,nv      ancillary for upscaling
/// \param      Ix,Iy,Iz derivatives
///////////////////////////////////////////////////////////////////////////////
void ComputeFlow(const float *I0, const float *I1, int width, int height, int stride,
				 float alpha, int nLevels, int nWarpIters, int nSolverIters, float *u, float *v,
				 const float **pI0, const float **pI1, int *pW, int *pH, int *pS,
				 float *tmp, float *du0, float *dv0, float *du1, float *dv1,
				 float *Ix, float *Iy, float *Iz,
				 float *nu, float *nv) {

	float *u0 = u;
	float *v0 = v;

	const int dataSize = stride * height * sizeof(float);

	// prepare pyramid
	int currentLevel  = nLevels - 1;
	pI0[currentLevel] = I0;
	pI1[currentLevel] = I1;

	for (; currentLevel > 0; --currentLevel) {
		Downscale(pI0[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], (float *)pI0[currentLevel - 1]);
		Downscale(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], (float *)pI1[currentLevel - 1]);
	}

	memset(u, 0, stride * height * sizeof(float));
	memset(v, 0, stride * height * sizeof(float));

	// compute flow
	for (; currentLevel < nLevels; ++currentLevel) {
		for (int warpIter = 0; warpIter < nWarpIters; ++warpIter) {
			memset(du0, 0, dataSize);
			memset(dv0, 0, dataSize);

			memset(du1, 0, dataSize);
			memset(dv1, 0, dataSize);

			// on current level we compute optical flow
			// between frame 0 and warped frame 1
			WarpImage(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], u, v, tmp);

			ComputeDerivatives(pI0[currentLevel], tmp, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], Ix, Iy, Iz);

			for (int iter = 0; iter < nSolverIters; ++iter) {
				SolveForUpdate(du0, dv0, Ix, Iy, Iz, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], alpha, du1, dv1);

				Swap(du0, du1, float *);
				Swap(dv0, dv1, float *);
			}

			// update u, v
			Add(u, du0, pH[currentLevel] * pS[currentLevel], u);
			Add(v, dv0, pH[currentLevel] * pS[currentLevel], v);
		}

		if (currentLevel != nLevels - 1) {
			// prolongate solution
			float scaleX = (float)pW[currentLevel + 1] / (float)pW[currentLevel];

			Upscale(u, pW[currentLevel], pH[currentLevel], pS[currentLevel],
					pW[currentLevel + 1], pH[currentLevel + 1], pS[currentLevel + 1],
					scaleX, nu);

			float scaleY = (float)pH[currentLevel + 1] / (float)pH[currentLevel];

			Upscale(v, pW[currentLevel], pH[currentLevel], pS[currentLevel],
					pW[currentLevel + 1], pH[currentLevel + 1], pS[currentLevel + 1],
					scaleY, nv);

			Swap(u, nu, float *);
			Swap(v, nv, float *);
		}
	}

	if (u != u0) {
		// solution is not in the specified array
		// copy
		memcpy(u0, u, dataSize);
		memcpy(v0, v, dataSize);
		Swap(u, nu, float *);
		Swap(v, nv, float *);
	}
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
/// \param[in] u        horizontal displacement
/// \param[in] v        vertical displacement
///////////////////////////////////////////////////////////////////////////////
void normCalc(int width, int height, int stride, const float *u, const float *v, double *p_sum, double *p_res, int frame) {
	PROF_NVTX_RANGEPUSH("Norm calc");

	double sum = 0;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			const int pos = j + i * stride;
			sum += fabsf(u[pos]) + fabsf(v[pos]);
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
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <nFrames> <frameDir> <threads> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const float alpha        = atof(argv[1]); // smoothness if image brightness is not within [0,1] this paramter should be scaled appropriately (0.2f)
	const int   nLevels      = atoi(argv[2]); // number of pyramid levels (5)
	const int   nSolverIters = atoi(argv[3]); // number of solver iterations on each level (500)
	const int   nWarpIters   = atoi(argv[4]); // number of warping iterations (3)
	const int   n_frames     = atoi(argv[5]); // number of frames
	// find images
	const char *video_path = argv[6]; // path to directory containing the frames
	const int   THREADS    = atoi(argv[7]);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("CPU-%d, ", THREADS);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n ALPHA: %g", alpha);
	printf("\n LEVELS: %d", nLevels);
	printf("\n SOLVERITERS: %d", nSolverIters);
	printf("\n WARPITERS: %d", nWarpIters);
	printf("\n FRAMES: %d", n_frames);
	printf("\n FRAMES PATH: %s", video_path);
	printf("\n POLICY: Async");
	printf("\n N_THREADS: %d", THREADS);
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
	int stride = iAlignUp(width, ALIGNMENT);

	// allocate memory
	float *source = (float *)malloc(stride * height * sizeof(float));
	float *target = (float *)malloc(stride * height * sizeof(float));

	float *u = (float *)malloc(stride * height * sizeof(float));
	float *v = (float *)malloc(stride * height * sizeof(float));

	const float *pI0[nLevels];
	const float *pI1[nLevels];

	const int dataSize = stride * height * sizeof(float);

	float *tmp = (float *)malloc(dataSize);
	float *du0 = (float *)malloc(dataSize);
	float *dv0 = (float *)malloc(dataSize);
	float *du1 = (float *)malloc(dataSize);
	float *dv1 = (float *)malloc(dataSize);

	float *Ix = (float *)malloc(dataSize);
	float *Iy = (float *)malloc(dataSize);
	float *Iz = (float *)malloc(dataSize);

	float *nu = (float *)malloc(dataSize);
	float *nv = (float *)malloc(dataSize);

	// prepare pyramid

	int currentLevel = nLevels - 1;

	// sizes of downscaled images
	int pW[nLevels];
	int pH[nLevels];
	int pS[nLevels];

	pW[currentLevel] = width;
	pH[currentLevel] = height;
	pS[currentLevel] = stride;

	for (; currentLevel > 0; --currentLevel) {
		int nw = pW[currentLevel] / 2;
		int nh = pH[currentLevel] / 2;
		int ns = iAlignUp(nw, ALIGNMENT);

		pI0[currentLevel - 1] = (float *)malloc(ns * nh * sizeof(float));
		pI1[currentLevel - 1] = (float *)malloc(ns * nh * sizeof(float));

		pW[currentLevel - 1] = nw;
		pH[currentLevel - 1] = nh;
		pS[currentLevel - 1] = ns;
	}

	double *p_sum = (double *)malloc((n_frames - 1) * sizeof(double));
	double *p_res = (double *)malloc((n_frames - 1) * sizeof(double));

	exec_clock = omp_get_wtime();

	// load first frame
	if (!LoadImageAsFP32(source, stride, video_path, 1)) {
		exit(EXIT_FAILURE);
	}
	for (int i = 2; i < n_frames + 1; i++) {
		if (!LoadImageAsFP32(target, stride, video_path, i)) {
			exit(EXIT_FAILURE);
		}

		ComputeFlow(source, target, width, height, stride, alpha, nLevels, nWarpIters, nSolverIters, u, v,
					pI0, pI1, pW, pH, pS, tmp, du0, dv0, du1, dv1, Ix, Iy, Iz, nu, nv);

		// compare results (L1 norm)
		normCalc(width, height, stride, u, v, p_sum, p_res, i);

		Swap(source, target, float *);
	}

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
	for (int i = 0; i < nLevels - 1; ++i) {
		free((void *)pI0[i]);
		free((void *)pI1[i]);
	}

	free(tmp);
	free(du0);
	free(dv0);
	free(du1);
	free(dv1);
	free(Ix);
	free(Iy);
	free(Iz);
	free(nu);
	free(nv);

	free(u);
	free(v);

	free(source);
	free(target);

	free(p_sum);
	free(p_res);

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