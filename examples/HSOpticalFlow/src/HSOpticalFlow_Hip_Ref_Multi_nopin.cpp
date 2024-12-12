/**
 * @file HSOpticalFlow_Hip_Ref_Multi.cu
 * @brief HSOpticalFlow: Multiframe native HIP version
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
void ComputeFlowHIP(const float *I0, const float *I1, int width, int height, int stride,
					float alpha, int nLevels, int nWarpIters, int nSolverIters, float *u, float *v,
					const float **pI0, const float **pI1, int *pW, int *pH, int *pS,
					float *d_tmp, float *d_du0, float *d_dv0, float *d_du1, float *d_dv1,
					float *d_Ix, float *d_Iy, float *d_Iz,
					float *d_u, float *d_v, float *d_nu, float *d_nv) {

	const int dataSize = stride * height * sizeof(float);

	// prepare pyramid
	int currentLevel = nLevels - 1;

	HIP_OP(hipMemcpy((void *)pI0[currentLevel], I0, dataSize, hipMemcpyHostToDevice));
	HIP_OP(hipMemcpy((void *)pI1[currentLevel], I1, dataSize, hipMemcpyHostToDevice));

	for (; currentLevel > 0; --currentLevel) {
		Downscale(pI0[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], (float *)pI0[currentLevel - 1]);
		Downscale(pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], (float *)pI1[currentLevel - 1]);
	}

	HIP_OP(hipMemset(d_u, 0, stride * height * sizeof(float)));
	HIP_OP(hipMemset(d_v, 0, stride * height * sizeof(float)));

	// compute flow
	for (; currentLevel < nLevels; ++currentLevel) {
		for (int warpIter = 0; warpIter < nWarpIters; ++warpIter) {
			HIP_OP(hipMemset(d_du0, 0, dataSize));
			HIP_OP(hipMemset(d_dv0, 0, dataSize));

			HIP_OP(hipMemset(d_du1, 0, dataSize));
			HIP_OP(hipMemset(d_dv1, 0, dataSize));

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

	HIP_OP(hipMemcpy(u, d_u, dataSize, hipMemcpyDeviceToHost));
	HIP_OP(hipMemcpy(v, d_v, dataSize, hipMemcpyDeviceToHost));
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
	PROF_ROCTX_RANGEPUSH("Load Image");

	unsigned char *data = 0;
	unsigned int   w = 0, h = 0;

	char frame_path[strlen(video_path) + 9];
	sprintf(frame_path, "%s/%03d.ppm", video_path, frame);

	bool result = sdkLoadPPM4ub(frame_path, &data, &w, &h);

	if (result == false) {
		printf("Invalid file format on %s\n", video_path);
		return false;
	}

	PROF_ROCTX_RANGEPUSH("Init Image");

	// source is 4 channel image
	const int channels = 4;

	for (size_t i = 0; i < h; ++i) {
		for (size_t j = 0; j < w; ++j) {
			// only keep first component
			img_data[j + i * img_s] = ((float)data[j * channels + i * channels * w]) / 255.0f;
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
void normCalc(int width, int height, int stride, const float *h_u, const float *h_v, double *p_sum, double *p_res, int frame) {
	PROF_ROCTX_RANGEPUSH("Norm calc")

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

	PROF_ROCTX_RANGEPOP();
}

///////////////////////////////////////////////////////////////////////////////
/// application entry point
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 8) {
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <nFrames> <frameDir> <GPU> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const float alpha        = atof(argv[1]); // smoothness if image brightness is not within [0,1] this paramter should be scaled appropriately (0.2f)
	const int   nLevels      = atoi(argv[2]); // number of pyramid levels (5)
	const int   nSolverIters = atoi(argv[3]); // number of solver iterations on each level (500)
	const int   nWarpIters   = atoi(argv[4]); // number of warping iterations (3)
	const int   n_frames     = atoi(argv[5]); // number of frames
	// find images
	const char *video_path = argv[6]; // path to directory containing the frames

	const int GPU = atoi(argv[7]);

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
	printf("\n FRAMES: %d", n_frames);
	printf("\n FRAMES PATH: %s", video_path);
	printf("\n POLICY: Sync");
	printf("\n DEVICE: %s", hip_dev_prop.name);
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
	int stride = iAlignUp(width, hip_dev_prop.texturePitchAlignment);

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
		int ns = iAlignUp(nw, hip_dev_prop.texturePitchAlignment);

		HIP_OP(hipMalloc(pI0 + currentLevel - 1, ns * nh * sizeof(float)));
		HIP_OP(hipMalloc(pI1 + currentLevel - 1, ns * nh * sizeof(float)));

		pW[currentLevel - 1] = nw;
		pH[currentLevel - 1] = nh;
		pS[currentLevel - 1] = ns;
	}

	double *p_sum = new double[n_frames - 1];
	double *p_res = new double[n_frames - 1];

	HIP_OP(hipDeviceSynchronize());
	exec_clock = omp_get_wtime();

	// load first frame
	if (!LoadImageAsFP32(h_source, stride, video_path, 1)) {
		exit(EXIT_FAILURE);
	}
	for (int i = 2; i < n_frames + 1; i++) {
		if (!LoadImageAsFP32(h_target, stride, video_path, i)) {
			exit(EXIT_FAILURE);
		}

		ComputeFlowHIP(h_source, h_target, width, height, stride, alpha, nLevels, nWarpIters, nSolverIters, h_u, h_v,
					   pI0, pI1, pW, pH, pS, d_tmp, d_du0, d_dv0, d_du1, d_dv1, d_Ix, d_Iy, d_Iz, d_u, d_v, d_nu, d_nv);

		// compare results (L1 norm)
		normCalc(width, height, stride, h_u, h_v, p_sum, p_res, i);

		Swap(h_source, h_target);
	}

	HIP_OP(hipDeviceSynchronize());
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