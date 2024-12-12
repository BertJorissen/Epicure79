/**
 * @file HSOpticalFlow_Hip_Ref.cu
 * @brief HSOpticalFlow: Native HIP base version
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

#include <hip/hip_runtime.h>
#include <omp.h>

#include "../../Utils/hip_helper.h"
#include "../../Utils/profiler_utils.h"
#include "helper_image.h"

#include "hip_ref_kernels/addKernel.hpp"
#include "hip_ref_kernels/derivativesKernel.hpp"
#include "hip_ref_kernels/downscaleKernel.hpp"
#include "hip_ref_kernels/solverKernel.hpp"
#include "hip_ref_kernels/upscaleKernel.hpp"
#include "hip_ref_kernels/warpingKernel.hpp"

#define AMD_TEX_PITCH_ALIGNMENT 64

double main_clock;
double exec_clock;

///////////////////////////////////////////////////////////////////////////////
/// \brief method logic
///
/// handles memory allocations, control flow
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
void ComputeFlowHIP(const float *I0, const float *I1, int width, int height,
					int stride, float alpha, int nLevels, int nWarpIters,
					int nSolverIters, float *u, float *v) {

	// pI0 and pI1 will hold device pointers
	const float **pI0 = new const float *[nLevels];
	const float **pI1 = new const float *[nLevels];

	int *pW = new int[nLevels];
	int *pH = new int[nLevels];
	int *pS = new int[nLevels];

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

	HIP_OP(hipMalloc(&d_tmp, dataSize));
	HIP_OP(hipMalloc(&d_du0, dataSize));
	HIP_OP(hipMalloc(&d_dv0, dataSize));
	HIP_OP(hipMalloc(&d_du1, dataSize));
	HIP_OP(hipMalloc(&d_dv1, dataSize));

	HIP_OP(hipMalloc(&d_Ix, dataSize));
	HIP_OP(hipMalloc(&d_Iy, dataSize));
	HIP_OP(hipMalloc(&d_Iz, dataSize));

	HIP_OP(hipMalloc(&d_u, dataSize));
	HIP_OP(hipMalloc(&d_v, dataSize));
	HIP_OP(hipMalloc(&d_nu, dataSize));
	HIP_OP(hipMalloc(&d_nv, dataSize));

	// prepare pyramid

	int currentLevel = nLevels - 1;
	// allocate GPU memory for input images
	HIP_OP(hipMalloc(pI0 + currentLevel, dataSize));
	HIP_OP(hipMalloc(pI1 + currentLevel, dataSize));

	HIP_OP(hipMemcpy((void *)pI0[currentLevel], I0, dataSize, hipMemcpyHostToDevice));
	HIP_OP(hipMemcpy((void *)pI1[currentLevel], I1, dataSize, hipMemcpyHostToDevice));

	pW[currentLevel] = width;
	pH[currentLevel] = height;
	pS[currentLevel] = stride;

	// Create lower resolution versions of both images (src y tgt)
	for (; currentLevel > 0; --currentLevel) {
		int nw = pW[currentLevel] / 2;
		int nh = pH[currentLevel] / 2;
		int ns = iAlignUp(nw, AMD_TEX_PITCH_ALIGNMENT);

		HIP_OP(hipMalloc(pI0 + currentLevel - 1, ns * nh * sizeof(float)));
		HIP_OP(hipMalloc(pI1 + currentLevel - 1, ns * nh * sizeof(float)));

		Downscale(pI0[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], nw, nh, ns, (float *)pI0[currentLevel - 1]);
		Downscale(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], nw, nh, ns, (float *)pI1[currentLevel - 1]);

		pW[currentLevel - 1] = nw;
		pH[currentLevel - 1] = nh;
		pS[currentLevel - 1] = ns;
	}

	HIP_OP(hipMemset(d_u, 0, stride * height * sizeof(float)));
	HIP_OP(hipMemset(d_v, 0, stride * height * sizeof(float)));

	// Initial estimate (u, v) starts at 0
	for (; currentLevel < nLevels; ++currentLevel) {
		for (int warpIter = 0; warpIter < nWarpIters; ++warpIter) {
			// Initialize p_du0, p_du1, p_dv0, p_dv1 to 0
			HIP_OP(hipMemset(d_du0, 0, dataSize));
			HIP_OP(hipMemset(d_dv0, 0, dataSize));

			HIP_OP(hipMemset(d_du1, 0, dataSize));
			HIP_OP(hipMemset(d_dv1, 0, dataSize));

			// Warp target image according to current estimate (u, v)
			WarpImage(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], d_u, d_v, d_tmp);

			// Compute matrices of the equation to solve
			ComputeDerivatives(pI0[currentLevel], d_tmp, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], d_Ix, d_Iy, d_Iz);

			// Solve equation for du, dv
			for (int iter = 0; iter < nSolverIters; ++iter) {
				SolveForUpdate(d_du0, d_dv0, d_Ix, d_Iy, d_Iz, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], alpha, d_du1, d_dv1);

				Swap(d_du0, d_du1);
				Swap(d_dv0, d_dv1);
			}

			// Update current estimate
			Add(d_u, d_du0, pH[currentLevel] * pS[currentLevel], d_u);
			Add(d_v, d_dv0, pH[currentLevel] * pS[currentLevel], d_v);
		}

		// Prolongate solution (u, v) for use in the next level
		if (currentLevel != nLevels - 1) {
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

	HIP_OP(hipMemcpy(u, d_u, dataSize, hipMemcpyDeviceToHost));
	HIP_OP(hipMemcpy(v, d_v, dataSize, hipMemcpyDeviceToHost));

	// cleanup
	for (int i = 0; i < nLevels; ++i) {
		HIP_OP(hipFree((void *)pI0[i]));
		HIP_OP(hipFree((void *)pI1[i]));
	}

	delete[] pI0;
	delete[] pI1;
	delete[] pW;
	delete[] pH;
	delete[] pS;

	HIP_OP(hipFree(d_tmp));
	HIP_OP(hipFree(d_du0));
	HIP_OP(hipFree(d_dv0));
	HIP_OP(hipFree(d_du1));
	HIP_OP(hipFree(d_dv1));
	HIP_OP(hipFree(d_Ix));
	HIP_OP(hipFree(d_Iy));
	HIP_OP(hipFree(d_Iz));
	HIP_OP(hipFree(d_nu));
	HIP_OP(hipFree(d_nv));
	HIP_OP(hipFree(d_u));
	HIP_OP(hipFree(d_v));
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
	PROF_ROCTX_RANGEPUSH("Write File");
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
	PROF_ROCTX_RANGEPOP();
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
bool LoadImageAsFP32(float *&img_data, int &img_w, int &img_h, int &img_s,
					 const char *name, const char *exePath) {
	PROF_ROCTX_RANGEPUSH("Load Image");
	unsigned char *data = 0;
	unsigned int   w = 0, h = 0;
	bool           result = sdkLoadPPM4ub(name, &data, &w, &h);

	if (result == false) {
		printf("Invalid file format on %s\n", name);
		return false;
	}

	img_w = w;
	img_h = h;
	img_s = iAlignUp(img_w, AMD_TEX_PITCH_ALIGNMENT);

	img_data = new float[img_s * h];

	// source is 4 channel image
	const int widthStep = 4 * img_w;
	PROF_ROCTX_RANGEPUSH("Init Matrix");

	for (int i = 0; i < img_h; ++i) {
		for (int j = 0; j < img_w; ++j) {
			img_data[j + i * img_s] = ((float)data[j * 4 + i * widthStep]) / 255.0f;
		}
	}

	free(data);

	PROF_ROCTX_RANGEPOP();
	PROF_ROCTX_RANGEPOP();

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
void normCalc(int width, int height, int stride, const float *h_u, const float *h_v) {
	PROF_ROCTX_RANGEPUSH("Norm calc");

	double sum = 0;
	double res = 0;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			const int pos = j + i * stride;
			sum += fabsf(h_u[pos]) + fabsf(h_v[pos]);
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

	PROF_ROCTX_RANGEPOP();
}

///////////////////////////////////////////////////////////////////////////////
/// application entry point
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 9) {
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <sourceFrameName> <targetFrameName> <outputFileName> <GPU> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const float alpha        = atof(argv[1]); // smoothness if image brightness is not within [0,1] this paramter should be scaled appropriately (0.2f)
	const int   nLevels      = atoi(argv[2]); // number of pyramid levels (5)
	const int   nSolverIters = atoi(argv[3]); // number of solver iterations on each level (500)
	const int   nWarpIters   = atoi(argv[4]); // number of warping iterations (3)
	// find images
	const char *const sourceFrameName = argv[5]; // frame10.ppm
	const char *const targetFrameName = argv[6]; // frame11.ppm
	const char *const outputFileName  = argv[7]; // FlowGPU.flo

	const int GPU = atoi(argv[8]);

	HIP_OP(hipSetDevice(GPU));
	hipDeviceProp_t hip_dev_prop;
	HIP_OP(hipGetDeviceProperties(&hip_dev_prop, GPU));
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("HIP_%s, ", hip_dev_prop.name);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n ALPHA: %g", alpha);
	printf("\n LEVELS: %d", nLevels);
	printf("\n SOLVERITERS: %d", nSolverIters);
	printf("\n WARPITERS: %d", nWarpIters);
	printf("\n SOURCE: %s", sourceFrameName);
	printf("\n TARGET: %s", targetFrameName);
	printf("\n OUTPUT: %s", outputFileName);
	printf("\n POLICY: Sync");
	printf("\n DEVICE: %s", hip_dev_prop.name);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);

	// image dimensions
	int width;
	int height;
	// row access stride
	int stride;

	// flow is computed from source image to target image
	float *h_source; // source image, host memory
	float *h_target; // target image, host memory

	// load image from file
	if (!LoadImageAsFP32(h_source, width, height, stride, sourceFrameName, argv[0])) {
		exit(EXIT_FAILURE);
	}

	if (!LoadImageAsFP32(h_target, width, height, stride, targetFrameName, argv[0])) {
		exit(EXIT_FAILURE);
	}

	// allocate host memory for GPU results
	float *h_u = new float[stride * height];
	float *h_v = new float[stride * height];

	HIP_OP(hipDeviceSynchronize());
	exec_clock = omp_get_wtime();

	ComputeFlowHIP(h_source, h_target, width, height, stride, alpha, nLevels, nWarpIters, nSolverIters, h_u, h_v);

	HIP_OP(hipDeviceSynchronize());
	exec_clock = omp_get_wtime() - exec_clock;

	// compare results (L1 norm)
	normCalc(width, height, stride, h_u, h_v);

	// WriteFloFile(outputFileName, width, height, stride, h_u, h_v);

	// free resources
	delete[] h_u;
	delete[] h_v;

	delete[] h_source;
	delete[] h_target;

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