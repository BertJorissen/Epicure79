/**
 * @file HSOpticalFlow_Cpu_Ref.c
 * @brief HSOpticalFlow: Native CPU base version. Parallelized using OpenMP.
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

///////////////////////////////////////////////////////////////////////////////
/// \brief method logic
///
/// handles memory allocation and control flow
/// \param[in]  I0           source image
/// \param[in]  I1           tracked image
/// \param[in]  width        images width
/// \param[in]  height       images height
/// \param[in]  stride       images stride
/// \param[in]  alpha        degree of displacement field smoothness
/// \param[in]  nLevels      number of levels in a pyramid
/// \param[in]  nWarpIters   number of warping iterations per pyramid level
/// \param[in]  nSolverIters number of solver iterations (Jacobi iterations)
/// \param[out] u            horizontal displacement
/// \param[out] v            vertical displacement
///////////////////////////////////////////////////////////////////////////////
void ComputeFlow(const float *I0, const float *I1, int width, int height,
				 int stride, float alpha, int nLevels, int nWarpIters,
				 int nSolverIters, float *u, float *v) {

	float *u0 = u;
	float *v0 = v;

	const float *pI0[nLevels];
	const float *pI1[nLevels];

	int pW[nLevels];
	int pH[nLevels];
	int pS[nLevels];

	const int pixelCountAligned = height * stride;

	float *tmp = (float *)malloc(pixelCountAligned * sizeof(float));
	float *du0 = (float *)malloc(pixelCountAligned * sizeof(float));
	float *dv0 = (float *)malloc(pixelCountAligned * sizeof(float));
	float *du1 = (float *)malloc(pixelCountAligned * sizeof(float));
	float *dv1 = (float *)malloc(pixelCountAligned * sizeof(float));
	float *Ix  = (float *)malloc(pixelCountAligned * sizeof(float));
	float *Iy  = (float *)malloc(pixelCountAligned * sizeof(float));
	float *Iz  = (float *)malloc(pixelCountAligned * sizeof(float));
	float *nu  = (float *)malloc(pixelCountAligned * sizeof(float));
	float *nv  = (float *)malloc(pixelCountAligned * sizeof(float));

	// prepare pyramid
	int currentLevel  = nLevels - 1;
	pI0[currentLevel] = I0;
	pI1[currentLevel] = I1;

	pW[currentLevel] = width;
	pH[currentLevel] = height;
	pS[currentLevel] = stride;

	for (; currentLevel > 0; --currentLevel) {
		int nw                = pW[currentLevel] / 2;
		int nh                = pH[currentLevel] / 2;
		int ns                = iAlignUp(nw, ALIGNMENT);
		pI0[currentLevel - 1] = (float *)malloc(ns * nh * sizeof(float));
		pI1[currentLevel - 1] = (float *)malloc(ns * nh * sizeof(float));

		Downscale(pI0[currentLevel], pW[currentLevel], pH[currentLevel],
				  pS[currentLevel], nw, nh, ns, (float *)pI0[currentLevel - 1]);

		Downscale(pI1[currentLevel], pW[currentLevel], pH[currentLevel],
				  pS[currentLevel], nw, nh, ns, (float *)pI1[currentLevel - 1]);

		pW[currentLevel - 1] = nw;
		pH[currentLevel - 1] = nh;
		pS[currentLevel - 1] = ns;
	}

	// initial approximation
	memset(u, 0, stride * height * sizeof(float));
	memset(v, 0, stride * height * sizeof(float));

	// compute flow
	for (; currentLevel < nLevels; ++currentLevel) {
		for (int warpIter = 0; warpIter < nWarpIters; ++warpIter) {
			memset(du0, 0, pixelCountAligned * sizeof(float));
			memset(dv0, 0, pixelCountAligned * sizeof(float));

			memset(du1, 0, pixelCountAligned * sizeof(float));
			memset(dv1, 0, pixelCountAligned * sizeof(float));

			WarpImage(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], u, v, tmp);

			// on current level we compute optical flow
			// between frame 0 and warped frame 1
			ComputeDerivatives(pI0[currentLevel], tmp, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], Ix, Iy, Iz);

			for (int iter = 0; iter < nSolverIters; ++iter) {
				SolveForUpdate(du0, dv0, Ix, Iy, Iz, pW[currentLevel], pH[currentLevel],
							   pS[currentLevel], alpha, du1, dv1);
				Swap(du0, du1, float *);
				Swap(dv0, dv1, float *);
			}

			// update u, v
			Add(u, du0, pH[currentLevel] * pS[currentLevel], u);
			Add(v, dv0, pH[currentLevel] * pS[currentLevel], v);

		} // end for (int warpIter = 0; warpIter < nWarpIters; ++warpIter)

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
	} // end for (; currentLevel < nLevels; ++currentLevel)

	if (u != u0) {
		// solution is not in the specified array
		// copy
		memcpy(u0, u, pixelCountAligned * sizeof(float));
		memcpy(v0, v, pixelCountAligned * sizeof(float));
		Swap(u, nu, float *);
		Swap(v, nv, float *);
	}

	// cleanup
	// last level is not being freed here
	// because it refers to input images
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
}

///////////////////////////////////////////////////////////////////////////////
/// \brief save optical flow in format described on vision.middlebury.edu/flow
/// \param[in] name output file name
/// \param[in] w    optical flow field width
/// \param[in] h    optical flow field height
/// \param[in] s    optical flow field row stride
/// \param[in] u    horizontal displacement
/// \param[in] v    vertical displacement
///////////////////////////////////////////////////////////////////////////////
void WriteFloFile(const char *name, int w, int h, int s, const float *u,
				  const float *v) {
	PROF_RANGEPUSH("Write File");
	FILE *stream;
	stream = fopen(name, "wb");

	if (stream == 0) {
		printf("Could not save flow to \"%s\"\n", name);
		return;
	}

	float data = 202021.25f;
	fwrite(&data, sizeof(float), 1, stream);
	fwrite(&w, sizeof(w), 1, stream);
	fwrite(&h, sizeof(h), 1, stream);

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			const int pos = j + i * s;
			fwrite(u + pos, sizeof(float), 1, stream);
			fwrite(v + pos, sizeof(float), 1, stream);
		}
	}

	fclose(stream);
	PROF_RANGEPOP();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// load 4-channel unsigned byte image
/// and convert it to single channel FP32 image
/// \param[out] img_data pointer to raw image data
/// \param[out] img_w    image width
/// \param[out] img_h    image height
/// \param[out] img_s    image row stride
/// \param[in]  name     image file name
/// \param[in]  exePath  executable file path
/// \return true if image is successfully loaded or false otherwise
///////////////////////////////////////////////////////////////////////////////
bool LoadImageAsFP32(float **img_data, int *img_w, int *img_h, int *img_s,
					 const char *name, const char *exePath) {
	PROF_RANGEPUSH("Load Image");
	unsigned char *data = 0;
	unsigned int   w = 0, h = 0;
	bool           result = sdkLoadPPM4ub(name, &data, &w, &h);

	if (result == false) {
		printf("Invalid file format on %s\n", name);
		return false;
	}

	*img_w = w;
	*img_h = h;
	int s  = iAlignUp(w, ALIGNMENT);
	*img_s = s;

	*img_data = (float *)malloc(*img_s * h * sizeof(float));

	// source is 4 channel image
	const int widthStep = 4 * w;
	PROF_RANGEPUSH("Init Matrix");

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			img_data[0][j + i * s] = ((float)data[j * 4 + i * widthStep]) / 255.0f;
		}
	}

	free(data);

	PROF_RANGEPOP();
	PROF_RANGEPOP();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief compute and print a norm (L1 norm) of the flow field for easier result checking
/// \param[in] width    optical flow field width
/// \param[in] height   optical flow field height
/// \param[in] stride   optical flow field row stride
/// \param[in] u      horizontal displacement
/// \param[in] v      vertical displacement
///////////////////////////////////////////////////////////////////////////////
void normCalc(int width, int height, int stride, const float *u, const float *v) {
	PROF_RANGEPUSH("Norm calc");

	double sum = 0;
	double res = 0;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			const int pos = j + i * stride;
			sum += fabsf(u[pos]) + fabsf(v[pos]);
		}
	}

	res = sum / (double)(width * height);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf, ", sum, res);
	#else
	printf("\n ----------------------- NORM ----------------------- \n\n");
	printf(" Sum: %lf \n", sum);
	printf(" Result: %lf \n", res);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	PROF_RANGEPOP();
}

///////////////////////////////////////////////////////////////////////////////
/// application entry point
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	double main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 9) {
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <sourceFrameName> <targetFrameName> <outputFileName> <threads> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const float alpha        = atof(argv[1]); // smoothness if image brightness is not within [0,1] this paramter should be scaled appropriately (0.2f)
	const int   nLevels      = atoi(argv[2]); // number of pyramid levels (5)
	const int   nSolverIters = atoi(argv[3]); // number of solver iterations on each level (500)
	const int   nWarpIters   = atoi(argv[4]); // number of warping iterations (3)
	// find images
	const char *const sourceFrameName = argv[5]; // frame10.ppm
	const char *const targetFrameName = argv[6]; // frame11.ppm
	const char *const outputFileName  = argv[7]; // FlowCPU.flo

	const int THREADS = atoi(argv[8]);

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("CPU-%d, ", THREADS);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n ALPHA: %g", alpha);
	printf("\n LEVELS: %d", nLevels);
	printf("\n SOLVERITERS: %d", nSolverIters);
	printf("\n WARPITERS: %d", nWarpIters);
	printf("\n SOURCE: %s", sourceFrameName);
	printf("\n TARGET: %s", targetFrameName);
	printf("\n OUTPUT: %s", outputFileName);
	printf("\n N_THREADS: %d", THREADS);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	// image dimensions
	int width;
	int height;
	// row access stride
	int stride;

	// flow is computed from source image to target image
	float *source; // source image
	float *target; // target image

	// load image from file
	if (!LoadImageAsFP32(&source, &width, &height, &stride, sourceFrameName, argv[0])) {
		exit(EXIT_FAILURE);
	}

	if (!LoadImageAsFP32(&target, &width, &height, &stride, targetFrameName, argv[0])) {
		exit(EXIT_FAILURE);
	}

	// allocate memory for results
	float *u = (float *)malloc(stride * height * sizeof(float));
	float *v = (float *)malloc(stride * height * sizeof(float));

	double exec_clock = omp_get_wtime();

	ComputeFlow(source, target, width, height, stride, alpha, nLevels, nWarpIters, nSolverIters, u, v);

	exec_clock = omp_get_wtime() - exec_clock;

	// compare results (L1 norm)
	normCalc(width, height, stride, u, v);

	// WriteFloFile(outputFileName, width, height, stride, u, v);

	// free resources
	free(u);
	free(v);

	free(source);
	free(target);

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
