/**
 * @file Texture_Test_OpenClGpu_Ref_Multi_Sync.c
 * @author Trasgo Group
 * @brief Texture Test: Multiframe native OpenCLGPU version
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

#include <CL/cl.h>
#include <omp.h>
#include <stdbool.h>

#include "../../Utils/opencl_helper.h"
#include "../../Utils/profiler_utils.h"
#include "helper_image.h"

#include "opencl_ref_kernels/addKernel.h"
#include "opencl_ref_kernels/derivativesKernel.h"
#include "opencl_ref_kernels/downscaleKernel.h"
#include "opencl_ref_kernels/solverKernel.h"
#include "opencl_ref_kernels/upscaleKernel.h"
#include "opencl_ref_kernels/warpingKernel.h"

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
void ComputeFlow(const float *I0, const float *I1, int width, int height, int stride,
				 float alpha, int nLevels, int nWarpIters, int nSolverIters, float *u, float *v,
				 cl_mem pI0[], cl_mem pI1[], int pW[], int pH[], int pS[],
				 cl_mem d_tmp, cl_mem d_du0, cl_mem d_dv0, cl_mem d_du1, cl_mem d_dv1,
				 cl_mem d_Ix, cl_mem d_Iy, cl_mem d_Iz,
				 cl_mem d_u, cl_mem d_v, cl_mem d_nu, cl_mem d_nv, cl_command_queue queue, cl_context ctx,
				 cl_kernel k_add, cl_kernel k_derivatives, cl_kernel k_downscale, cl_kernel k_solve,
				 cl_kernel k_upscale, cl_kernel k_warp) {

	const int dataSize = stride * height * sizeof(float);

	// prepare pyramid
	int currentLevel = nLevels - 1;

	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(queue, pI0[currentLevel], CL_TRUE, 0, dataSize, I0, 0, NULL, NULL));
	OPENCL_ASSERT_OP(clEnqueueWriteBuffer(queue, pI1[currentLevel], CL_TRUE, 0, dataSize, I1, 0, NULL, NULL));

	for (; currentLevel > 0; --currentLevel) {
		Downscale(ctx, queue, k_downscale, pI0[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], pI0[currentLevel - 1]);
		Downscale(ctx, queue, k_downscale, pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], pW[currentLevel - 1], pH[currentLevel - 1], pS[currentLevel - 1], pI1[currentLevel - 1]);
	}

	cl_uint pattern = 0;
	OPENCL_ASSERT_OP(clEnqueueFillBuffer(queue, d_u, &pattern, sizeof(cl_uint), 0, dataSize, 0, NULL, NULL));
	OPENCL_ASSERT_OP(clEnqueueFillBuffer(queue, d_v, &pattern, sizeof(cl_uint), 0, dataSize, 0, NULL, NULL));

	// compute flow
	for (; currentLevel < nLevels; ++currentLevel) {
		for (int warpIter = 0; warpIter < nWarpIters; ++warpIter) {
			OPENCL_ASSERT_OP(clEnqueueFillBuffer(queue, d_du0, &pattern, sizeof(cl_uint), 0, dataSize, 0, NULL, NULL));
			OPENCL_ASSERT_OP(clEnqueueFillBuffer(queue, d_dv0, &pattern, sizeof(cl_uint), 0, dataSize, 0, NULL, NULL));

			OPENCL_ASSERT_OP(clEnqueueFillBuffer(queue, d_du1, &pattern, sizeof(cl_uint), 0, dataSize, 0, NULL, NULL));
			OPENCL_ASSERT_OP(clEnqueueFillBuffer(queue, d_dv1, &pattern, sizeof(cl_uint), 0, dataSize, 0, NULL, NULL));

			// on current level we compute optical flow
			// between frame 0 and warped frame 1
			WarpImage(ctx, queue, k_warp, pI1[currentLevel], pW[currentLevel], pH[currentLevel], pS[currentLevel], d_u, d_v, d_tmp);

			ComputeDerivatives(ctx, queue, k_derivatives, pI0[currentLevel], d_tmp, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], d_Ix, d_Iy, d_Iz);

			for (int iter = 0; iter < nSolverIters; ++iter) {
				SolveForUpdate(queue, k_solve, d_du0, d_dv0, d_Ix, d_Iy, d_Iz, pW[currentLevel],
							   pH[currentLevel], pS[currentLevel], alpha, d_du1, d_dv1);

				Swap(d_du0, d_du1, cl_mem);
				Swap(d_dv0, d_dv1, cl_mem);
			}

			// update u, v
			Add(queue, k_add, d_u, d_du0, pW[currentLevel], pH[currentLevel], pS[currentLevel], d_u);
			Add(queue, k_add, d_v, d_dv0, pW[currentLevel], pH[currentLevel], pS[currentLevel], d_v);
		}

		if (currentLevel != nLevels - 1) {
			// prolongate solution
			float scaleX = (float)pW[currentLevel + 1] / (float)pW[currentLevel];

			Upscale(ctx, queue, k_upscale, d_u, pW[currentLevel], pH[currentLevel], pS[currentLevel],
					pW[currentLevel + 1], pH[currentLevel + 1], pS[currentLevel + 1],
					scaleX, d_nu);

			float scaleY = (float)pH[currentLevel + 1] / (float)pH[currentLevel];

			Upscale(ctx, queue, k_upscale, d_v, pW[currentLevel], pH[currentLevel], pS[currentLevel],
					pW[currentLevel + 1], pH[currentLevel + 1], pS[currentLevel + 1],
					scaleY, d_nv);

			Swap(d_u, d_nu, cl_mem);
			Swap(d_v, d_nv, cl_mem);
		}
	}

	OPENCL_ASSERT_OP(clEnqueueReadBuffer(queue, d_u, CL_TRUE, 0, dataSize, u, 0, NULL, NULL));
	OPENCL_ASSERT_OP(clEnqueueReadBuffer(queue, d_v, CL_TRUE, 0, dataSize, v, 0, NULL, NULL));
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

cl_kernel compileOpenCLKernel(cl_context ctx, cl_device_id device_id, const char *kernel_raw, const char *kernel_name) {
	cl_int err;

	char *kernel_str = (char *)malloc((strlen(kernel_raw) + 300) * sizeof(char));
	// only used for solve kernel, in the original was done via templates
	sprintf(&kernel_str[0], " \n #define bx  %d \n#define by %d \n", SOLVECHAR_0, SOLVECHAR_1);
	strcat(kernel_str, kernel_raw);

	size_t kernel_size = strlen(kernel_str);

	cl_program program = clCreateProgramWithSource(ctx, 1, (const char **)(&kernel_str), (const size_t *)(&kernel_size), &err);
	OPENCL_ASSERT_ERROR(err);

	err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (err == CL_BUILD_PROGRAM_FAILURE) {
		fprintf(stderr, "opencl kernel build faliure %s\n", kernel_name);
		fprintf(stderr, "\n\n%s\n\n", kernel_str);
		fflush(stderr);
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char *log = (char *)malloc(log_size);
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		fprintf(stderr, "%s\n", log);
		fflush(stderr);
		free(log);
		free(kernel_str);
		exit(EXIT_FAILURE);
	}

	cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
	OPENCL_ASSERT_ERROR(err);
	free(kernel_str);

	return kernel;
}

///////////////////////////////////////////////////////////////////////////////
/// application entry point
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 9) {
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <device> <platform> <nFrames> <frameDir> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const float alpha        = atof(argv[1]); // smoothness if image brightness is not within [0,1] this paramter should be scaled appropriately (0.2f)
	const int   nLevels      = atoi(argv[2]); // number of pyramid levels (5)
	const int   nSolverIters = atoi(argv[3]); // number of solver iterations on each level (500)
	const int   nWarpIters   = atoi(argv[4]); // number of warping iterations (3)
	const int   device       = atoi(argv[5]);
	const int   platform     = atoi(argv[6]);
	const int   n_frames     = atoi(argv[7]); // number of frames
	// find images
	const char *video_path = argv[8]; // path to directory containing the frames

	/* PLATFORMS & DEVICES */
	cl_platform_id *p_platforms = (cl_platform_id *)malloc((platform + 1) * sizeof(cl_platform_id));
	OPENCL_ASSERT_OP(clGetPlatformIDs(platform + 1, p_platforms, NULL));
	cl_platform_id platform_id = p_platforms[platform];
	free(p_platforms);

	cl_device_id *p_devices = (cl_device_id *)malloc((device + 1) * sizeof(cl_device_id));
	OPENCL_ASSERT_OP(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, device + 1, p_devices, NULL));
	cl_device_id device_id = p_devices[device];
	free(p_devices);

	// Platform and device info
	size_t platform_name_size;
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size));
	char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
	OPENCL_ASSERT_OP(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL));

	size_t device_name_size;
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size));
	char *device_name = (char *)malloc(sizeof(char) * device_name_size);
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL));

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%s, %s, ", platform_name, device_name);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ----------------------- ARGS ------------------------- \n");
	printf("\n ALPHA: %g", alpha);
	printf("\n LEVELS: %d", nLevels);
	printf("\n SOLVERITERS: %d", nSolverIters);
	printf("\n WARPITERS: %d", nWarpIters);
	printf("\n FRAMES: %d", n_frames);
	printf("\n FRAMES PATH: %s", video_path);
	printf("\n POLICY: Sync");
	printf("\n PLATFORM: %s", platform_name);
	printf("\n DEVICE: %s", device_name);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
	fflush(stdout);
	free(platform_name);
	free(device_name);

	cl_int                err;
	cl_context_properties ctx_props[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0};
	cl_context            ctx         = clCreateContext(ctx_props, 1, &device_id, NULL, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	cl_command_queue queue = clCreateCommandQueue(ctx, device_id, 0, &err);
	OPENCL_ASSERT_ERROR(err);

	// pitch required by the device
	cl_uint device_pitch;
	OPENCL_ASSERT_OP(clGetDeviceInfo(device_id, CL_DEVICE_IMAGE_PITCH_ALIGNMENT, sizeof(cl_uint), &device_pitch, NULL));
	if (device_pitch == 0) {
		fprintf(stderr, "[Warning] null device pitch, defaulting to 256\n");
		device_pitch = 256;
	}

	/* Kernel compilation */
	cl_kernel k_add         = compileOpenCLKernel(ctx, device_id, ADD_KERNEL, "Add");
	cl_kernel k_derivatives = compileOpenCLKernel(ctx, device_id, DERIVATIVES_KERNEL, "ComputeDerivatives");
	cl_kernel k_downscale   = compileOpenCLKernel(ctx, device_id, DOWNSCALE_KERNEL, "Downscale");
	cl_kernel k_solve       = compileOpenCLKernel(ctx, device_id, SOLVE_KERNEL, "Solve");
	cl_kernel k_upscale     = compileOpenCLKernel(ctx, device_id, UPSCALE_KERNEL, "Upscale");
	cl_kernel k_warp        = compileOpenCLKernel(ctx, device_id, WARP_KERNEL, "Warp");

	// get image dimensions
	char           frame_path[strlen(video_path) + 9];
	unsigned int   channels, width, height;
	unsigned char *aux = NULL;
	if (!__loadPPM(strcat(strcpy(frame_path, video_path), "/001.ppm"), &aux, &width, &height, &channels)) {
		exit(EXIT_FAILURE);
	}
	free(aux);
	// row access stride
	int stride = iAlignUp(width, device_pitch);

	// allocate host memory
	float *h_source = (float *)malloc(stride * height * sizeof(float));
	float *h_target = (float *)malloc(stride * height * sizeof(float));

	float *h_u = (float *)malloc(stride * height * sizeof(float));
	float *h_v = (float *)malloc(stride * height * sizeof(float));

	// allocate device memory
	cl_mem pI0[nLevels];
	cl_mem pI1[nLevels];

	// device memory pointers
	const int dataSize = stride * height * sizeof(float);

	cl_mem d_tmp = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_du0 = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_dv0 = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_du1 = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_dv1 = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_Ix  = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_Iy  = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_Iz  = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_u   = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_v   = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_nu  = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	cl_mem d_nv  = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

	// prepare pyramid

	int currentLevel = nLevels - 1;
	// allocate GPU memory for input images

	pI0[currentLevel] = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	pI1[currentLevel] = clCreateBuffer(ctx, CL_MEM_READ_WRITE, dataSize, NULL, &err);
	OPENCL_ASSERT_ERROR(err);

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
		int ns = iAlignUp(nw, device_pitch);

		pI0[currentLevel - 1] = clCreateBuffer(ctx, CL_MEM_READ_WRITE, ns * nh * sizeof(float), NULL, &err);
		pI1[currentLevel - 1] = clCreateBuffer(ctx, CL_MEM_READ_WRITE, ns * nh * sizeof(float), NULL, &err);
		OPENCL_ASSERT_ERROR(err);

		pW[currentLevel - 1] = nw;
		pH[currentLevel - 1] = nh;
		pS[currentLevel - 1] = ns;
	}

	double *p_sum = (double *)malloc(sizeof(double) * (n_frames - 1));
	double *p_res = (double *)malloc(sizeof(double) * (n_frames - 1));

	OPENCL_ASSERT_OP(clFinish(queue));
	exec_clock = omp_get_wtime();

	// load first frame
	if (!LoadImageAsFP32(h_source, stride, video_path, 1)) {
		exit(EXIT_FAILURE);
	}
	for (int i = 2; i < n_frames + 1; i++) {
		if (!LoadImageAsFP32(h_target, stride, video_path, i)) {
			exit(EXIT_FAILURE);
		}

		ComputeFlow(h_source, h_target, width, height, stride, alpha, nLevels, nWarpIters, nSolverIters, h_u, h_v,
					pI0, pI1, pW, pH, pS, d_tmp, d_du0, d_dv0, d_du1, d_dv1, d_Ix, d_Iy, d_Iz, d_u, d_v, d_nu, d_nv,
					queue, ctx, k_add, k_derivatives, k_downscale, k_solve, k_upscale, k_warp);

		// compare results (L1 norm)
		normCalc(width, height, stride, h_u, h_v, p_sum, p_res, i);

		Swap(h_source, h_target, float *);
	}

	OPENCL_ASSERT_OP(clFinish(queue));
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
		OPENCL_ASSERT_OP(clReleaseMemObject(pI0[i]));
		OPENCL_ASSERT_OP(clReleaseMemObject(pI1[i]));
	}

	OPENCL_ASSERT_OP(clReleaseMemObject(d_tmp));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_du0));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_dv0));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_du1));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_dv1));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_Ix));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_Iy));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_Iz));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_nu));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_nv));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_u));
	OPENCL_ASSERT_OP(clReleaseMemObject(d_v));

	free(h_u);
	free(h_v);

	free(h_source);
	free(h_target);

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