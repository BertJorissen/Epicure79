/**
 * @file HSOpticalFLow_Ctrl.cu
 * @author Trasgo Group
 * @brief HSOpticalFlow: Ctrl base version
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
#include "helper_image.h"
#include <math.h>
#include <unistd.h>

#include "ctrl_kernels/kernels_protos.h"

#include "../../Utils/profiler_utils.h"

#define hit_tileSwap(a, b)               \
	{                                    \
		HitTile tmp    = *(HitTile *)&a; \
		a              = b;              \
		*(HitTile *)&b = tmp;            \
	}

double main_clock;
double exec_clock;

/* E. Host task to calculate and print the norm */
CTRL_HOST_TASK(Norm_calc, HitTile_float u, HitTile_float v) {
	PROF_RANGEPUSH("Norm calc");

	double sum = 0;
	double res = 0;

	int h = hit_tileDimCard(u, 0);
	int w = hit_tileDimCard(u, 1);

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			sum += fabsf(hit(u, i, j)) + fabsf(hit(v, i, j));
		}
	}

	res = sum / (double)(w * h);

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

CTRL_HOST_TASK(Init, HitTile_float matrix, unsigned char *data) {
	PROF_RANGEPUSH("Init matrix");

	int w = hit_tileDimCard(matrix, 1);
	int h = hit_tileDimCard(matrix, 0);

	// source is 4 channel image
	const int channels = 4;

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			hit(matrix, i, j) = ((float)data[j * channels + i * channels * w]) / 255.0f;
		}
	}

	free(data);

	PROF_RANGEPOP();
}

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Norm_calc, 2, IN, HitTile_float, u, IN, HitTile_float, v);
CTRL_HOST_TASK_PROTO(Init, 2, OUT, HitTile_float, matrix, INVAL, unsigned char *, data);

HitTile_float LoadImageAsFP32(PCtrl ctrl, const char *name) {
	PROF_RANGEPUSH("Load Image");

	unsigned char *data = 0;
	unsigned int   w = 0, h = 0;

	// NOTE @sergioalo in PPM format the pixels are stored as RGB triples, this function returns a 4 channel image in which the 4th component is 0
	bool result = sdkLoadPPM4ub(name, &data, &w, &h);

	if (result == false) {
		printf("Invalid file format on %s\n", name);
		exit(EXIT_FAILURE);
	}

	HitTile_float img_data = Ctrl_DomainAlloc(ctrl, float, hitShapeSize(h, w), CTRL_MEM_ALIGNED);
	Ctrl_HostTask(Init, img_data, data);

	PROF_RANGEPOP();

	return img_data;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief save optical flow in format described on vision.middlebury.edu/flow
/// \param[in] name output file name
/// \param[in] u    horizontal displacement
/// \param[in] v    vertical displacement
///////////////////////////////////////////////////////////////////////////////
void WriteFloFile(const char *name, HitTile_float u, HitTile_float v) {
	PROF_RANGEPUSH("Write File");

	FILE *stream = fopen(name, "wb");

	if (stream == 0) {
		printf("[ERROR] Could not save flow to \"%s\"\n", name);
		return;
	}

	float data = 202021.25f;
	int   w    = hit_tileDimCard(u, 1);
	int   h    = hit_tileDimCard(u, 0);
	fwrite(&data, sizeof(float), 1, stream);
	fwrite(&w, sizeof(w), 1, stream);
	fwrite(&h, sizeof(h), 1, stream);

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			fwrite(&hit(u, i, j), sizeof(float), 1, stream);
			fwrite(&hit(v, i, j), sizeof(float), 1, stream);
		}
	}

	fclose(stream);
	PROF_RANGEPOP();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief method logic
///
/// handles memory allocations, control flow
/// \param[in]  I0           source image
/// \param[in]  I1           tracked image
/// \param[in]  alpha        degree of displacement field smoothness
/// \param[in]  nLevels      number of levels in a pyramid
/// \param[in]  nWarpIters   number of warping iterations per pyramid level
/// \param[in]  nSolverIters number of solver iterations (Jacobi iterations)
/// \param[out] u            horizontal displacement
/// \param[out] v            vertical displacement
///////////////////////////////////////////////////////////////////////////////
void ComputeFlowCUDA(PCtrl ctrl, HitTile_float I0, HitTile_float I1, float alpha, int nLevels, int nWarpIters,
					 int nSolverIters, HitTile_float u, HitTile_float v) {

	HitTile_float pI0[nLevels];
	HitTile_float pI1[nLevels];

	Ctrl_Thread p_threads[nLevels];
	// +1 in both dims to force extra threads to copy last column and row of the
	// matrix in the first padding row and column to have boundary values during
	// stencil stage on solve kernel
	Ctrl_Thread p_threads1[nLevels];

	Ctrl_TexDesc tex_desc      = {0};
	tex_desc.normalized_coords = true;
	tex_desc.filter_mode       = CTRL_TEX_FILTERMODE_LINEAR;
	tex_desc.read_mode         = CTRL_TEX_READMODE_ELEMTYPE;
	tex_desc.addr_mode[0]      = CTRL_TEX_ADDRMODE_MIRROR;
	tex_desc.addr_mode[1]      = CTRL_TEX_ADDRMODE_MIRROR;

	// device memory pointers
	HitTile_float d_tmp = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
	HitTile_float d_du0 = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
	HitTile_float d_dv0 = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
	HitTile_float d_du1 = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
	HitTile_float d_dv1 = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);

	HitTile_float d_Ix = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
	HitTile_float d_Iy = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
	HitTile_float d_Iz = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);

	HitTile_float d_nu = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
	HitTile_float d_nv = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(I0), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);

	// prepare pyramid
	int currentLevel = nLevels - 1;

	pI0[currentLevel] = I0;
	pI1[currentLevel] = I1;
	// using default tile size for textures
	Ctrl_CreateTex(ctrl, pI0[currentLevel], tex_desc);
	Ctrl_CreateTex(ctrl, pI1[currentLevel], tex_desc);
	// NOTE I0 I1 are copied to dev here in ref

	Ctrl_ThreadInit(p_threads[currentLevel], hit_tileDimCard(I0, 0), hit_tileDimCard(I0, 1));
	Ctrl_ThreadInit(p_threads1[currentLevel], hit_tileDimCard(I0, 0) + 1, hit_tileDimCard(I0, 1) + 1);

	// Create lower resolution versions of both images (src y tgt)
	for (; currentLevel > 0; --currentLevel) {
		int nh = hit_tileDimCard(pI0[currentLevel], 0) / 2;
		int nw = hit_tileDimCard(pI0[currentLevel], 1) / 2;

		HitShape new_shape    = hitShapeSize(nh, nw);
		pI0[currentLevel - 1] = Ctrl_DomainAlloc(ctrl, float, new_shape, CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
		pI1[currentLevel - 1] = Ctrl_DomainAlloc(ctrl, float, new_shape, CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);

		// using default tile size for textures
		Ctrl_CreateTex(ctrl, pI0[currentLevel - 1], tex_desc);
		Ctrl_CreateTex(ctrl, pI1[currentLevel - 1], tex_desc);

		Ctrl_ThreadInit(p_threads[currentLevel - 1], nh, nw);
		Ctrl_ThreadInit(p_threads1[currentLevel - 1], nh + 1, nw + 1);

		Ctrl_Launch(ctrl, Downscale, p_threads[currentLevel - 1], CTRL_THREAD_NULL, pI0[currentLevel], pI0[currentLevel - 1]);
		Ctrl_Launch(ctrl, Downscale, p_threads[currentLevel - 1], CTRL_THREAD_NULL, pI1[currentLevel], pI1[currentLevel - 1]);
	}

	// TODO what to do with this? custom kernel? lib kernel that calls to memset? 1D char or 2D char
	Ctrl_Launch(ctrl, Zero, p_threads[nLevels - 1], CTRL_THREAD_NULL, u);
	Ctrl_Launch(ctrl, Zero, p_threads[nLevels - 1], CTRL_THREAD_NULL, v);

	// Initial estimate (u, v) starts at 0
	for (; currentLevel < nLevels; ++currentLevel) {
		// Texture creation
		tex_desc.width  = p_threads[currentLevel].j;
		tex_desc.height = p_threads[currentLevel].i;
		Ctrl_CreateTex(ctrl, d_tmp, tex_desc);
		Ctrl_CreateTex(ctrl, u, tex_desc);
		Ctrl_CreateTex(ctrl, v, tex_desc);

		for (int warpIter = 0; warpIter < nWarpIters; ++warpIter) {
			// Initialize p_du0, p_du1, p_dv0, p_dv1 to 0
			Ctrl_Launch(ctrl, Zero, p_threads[nLevels - 1], CTRL_THREAD_NULL, d_du0);
			Ctrl_Launch(ctrl, Zero, p_threads[nLevels - 1], CTRL_THREAD_NULL, d_dv0);

			Ctrl_Launch(ctrl, Zero, p_threads[nLevels - 1], CTRL_THREAD_NULL, d_du1);
			Ctrl_Launch(ctrl, Zero, p_threads[nLevels - 1], CTRL_THREAD_NULL, d_dv1);

			// Warp target image according to current estimate (u, v)
			Ctrl_Launch(ctrl, Warp, p_threads[currentLevel], CTRL_THREAD_NULL, pI1[currentLevel], u, v, d_tmp);

			// Compute matrices of the equation to solve
			Ctrl_Launch(ctrl, ComputeDerivatives, p_threads[currentLevel], CTRL_THREAD_NULL, pI0[currentLevel], d_tmp, d_Ix, d_Iy, d_Iz);

			// Solve equation for du, dv
			for (int iter = 0; iter < nSolverIters; ++iter) {
				// threads are +1 in both dims because of the way the copy to shared mem is done
				Ctrl_Launch(ctrl, Solve, p_threads1[currentLevel], CTRL_THREAD_NULL, d_du0, d_dv0, d_Ix, d_Iy, d_Iz, alpha, d_du1, d_dv1);

				hit_tileSwap(d_du0, d_du1);
				hit_tileSwap(d_dv0, d_dv1);
			}

			// Update current estimate
			Ctrl_Launch(ctrl, Add, p_threads[currentLevel], CTRL_THREAD_NULL, u, d_du0, u);
			Ctrl_Launch(ctrl, Add, p_threads[currentLevel], CTRL_THREAD_NULL, v, d_dv0, v);
		}

		// Prolongate solution (u, v) for use in the next level
		if (currentLevel != nLevels - 1) {
			float scaleX = (float)hit_tileDimCard(pI0[currentLevel + 1], 1) / (float)hit_tileDimCard(pI0[currentLevel], 1);

			Ctrl_Launch(ctrl, Upscale, p_threads[currentLevel + 1], CTRL_THREAD_NULL, u, scaleX, d_nu);

			float scaleY = (float)hit_tileDimCard(pI0[currentLevel + 1], 0) / (float)hit_tileDimCard(pI0[currentLevel], 0);

			Ctrl_Launch(ctrl, Upscale, p_threads[currentLevel + 1], CTRL_THREAD_NULL, v, scaleY, d_nv);

			hit_tileSwap(u, d_nu);
			hit_tileSwap(v, d_nv);
		}
	}

	// NOTE results are copied to host here in ref

	// cleanup
	for (int i = 0; i < nLevels; ++i) {
		Ctrl_Free(ctrl, pI0[i], pI1[i]);
	}

	Ctrl_Free(ctrl, d_tmp);
	Ctrl_Free(ctrl, d_du0, d_dv0);
	Ctrl_Free(ctrl, d_du1, d_dv1);
	Ctrl_Free(ctrl, d_Ix, d_Iy, d_Iz);
	Ctrl_Free(ctrl, d_nu, d_nv);
}

int main(int argc, char *argv[]) {
	Ctrl_Init(&argc, &argv);
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 9) {
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <sourceFrameName> <targetFrameName> <outputFileName> <config_file> \n\n", argv[0]);
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

	char *ctrl_conf_file = argv[8];

	__ctrl_block__(ctrl_conf_file) {
		// 3. Get controller object and print info
		PCtrl ctrl = Ctrl_Get(0);

		Ctrl_Info info = Ctrl_GetInfo(ctrl);
		#ifdef _CTRL_EXAMPLES_EXP_MODE_
		printf("%s, ", info.device_name);
		#else // _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n ALPHA: %g", alpha);
		printf("\n LEVELS: %d", nLevels);
		printf("\n SOLVERITERS: %d", nSolverIters);
		printf("\n WARPITERS: %d", nWarpIters);
		printf("\n SOURCE: %s", sourceFrameName);
		printf("\n TARGET: %s", targetFrameName);
		printf("\n OUTPUT: %s", outputFileName);

		printf("\n\n CTRL TYPE: %s", info.type);
		printf("\n PLATFORM: %s", info.platform_name);
		printf("\n DEVICE: %s", info.device_name);
		printf("\n N_THREADS: %d", info.n_threads);
		printf("\n MEM_MOVES: %s", info.mem_transfers ? "ON" : "OFF");
		printf("\n NUMA RANGE: %d-%d", info.numa_range_min, info.numa_range_max);
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		// flow is computed from source image to target image
		// 2. load image from file
		HitTile_float source = LoadImageAsFP32(ctrl, sourceFrameName);
		HitTile_float target = LoadImageAsFP32(ctrl, targetFrameName);

		HitTile_float u = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(source), CTRL_MEM_ALIGNED);
		HitTile_float v = Ctrl_DomainAlloc(ctrl, float, hit_tileShape(source), CTRL_MEM_ALIGNED);

		// 6. Sync and start timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		// 7. Launch the operations, copies (if necessary) are implicit
		ComputeFlowCUDA(ctrl, source, target, alpha, nLevels, nWarpIters, nSolverIters, u, v);

		// 8. Sync and stop timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime() - exec_clock;

		// This should not be neccessary but there are weird issues with the freeing of shared tiles in opencl ctrls
		Ctrl_Synchronize();

		// compare results (L1 norm)
		Ctrl_HostTask(Norm_calc, u, v);
		Ctrl_WaitTile(ctrl, u, v);

		// WriteFloFile(outputFileName, u, v);

		// 10. Free data structures
		Ctrl_Free(ctrl, u, v);

		// 11. Destroy the controller
		Ctrl_EndBlock();
	}

	// 12. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n");
	printf("Clock main: %lf\n", main_clock);
	printf("Clock exec: %lf\n", exec_clock);
	printf("\n\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	Ctrl_Finalize();
	return EXIT_SUCCESS;
}
