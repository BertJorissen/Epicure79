/**
 * @file HSOpticalFLow_Ctrl_Multi.cu
 * @brief HSOpticalFlow: Ctrl version, muli frame and multi device
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"
#include "helper_image.h"
#include <math.h>
#include <unistd.h>
#include "../../examples/Utils/ctrl_print_info.h"

#include "ctrl_kernels/kernels_protos.h"

#define hit_tileSwap(a, b)               \
	{                                    \
		HitTile tmp    = *(HitTile *)&a; \
		a              = b;              \
		*(HitTile *)&b = tmp;            \
	}

double main_clock;
double exec_clock;

CTRL_HOST_TASK(Load_frame, HitTile_float matrix, const char *video_path, int frame) {
	unsigned char *data = 0;
	unsigned int   w = 0, h = 0;

	char frame_path[strlen(video_path) + 9];
	sprintf(frame_path, "%s/%03d.ppm", video_path, frame);

	// NOTE @sergioalo in PPM format the pixels are stored as RGB triples, this function returns a 4 channel image in which the 4th component is 0
	bool result = sdkLoadPPM4ub(frame_path, &data, &w, &h);

	if (result == false) {
		fprintf(stderr, "Invalid file format on %s\n", frame_path);
		exit(EXIT_FAILURE);
	}

	// source is 4 channel image
	const int channels = 4;

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			// only keep first component
			hit(matrix, i, j) = ((float)data[j * channels + i * channels * w]) / 255.0f;
		}
	}

	free(data);
}

/* F. Defining host task prototypes */
CTRL_HOST_TASK_PROTO(Load_frame, 3, OUT, HitTile_float, matrix, INVAL, const char *, video_path, INVAL, int, frame);

#define CTRL_BLK_DEF     CTRL_THREAD_NULL
#define hit_height(tile) hit_tileDimCard(tile, 0)
#define hit_width(tile)  hit_tileDimCard(tile, 1)

typedef struct Cut_Point {
	int cid; /**< device id */
	int lvl; /**< End level */
	int wi;  /**< End warp iteration */
} Cut_Point;

Cut_Point *cut_points = NULL;
int        npoints    = 0;

/**
 * @brief Initalize work partition.
 *
 * Allocates memory to store a list of cutting points of size equal to the number of compute devices
 */
void InitWorkPartition() {
	cut_points = (Cut_Point *)malloc(sizeof(Cut_Point) * (Ctrl_GetNCtrls() - 1));
}

/**
 * @brief free previously allocated work partition.
 */
void DestroyWorkPartition() {
	free(cut_points);
}

/**
 * @brief Adds a new cut point to the work partition
 *
 * The cut points must be added in sequential order.
 * There may not be more cut points than the number of compute devices.
 *
 * @param cid id of device responsible for the section
 * @param lvl level of the cut point
 * @param wi Warp iteration of the cu point
 */
void AddCutPoint(int cid, int lvl, int wi) {
	if (npoints >= Ctrl_GetNCtrls()) {
		fprintf(stderr, "Error: tried to add more cut points than compute devices available\n");
		exit(EXIT_FAILURE);
	}

	cut_points[npoints++] = (Cut_Point){.cid = cid, .lvl = lvl, .wi = wi};
}

/**
 * @brief Get the id of the device in charge of executing warp iter \p wi of level \p lvl
 *
 * @param lvl level
 * @param wi warp iter
 * @return Ctrl id in charge of this block
 */
int Get_Cid(int lvl, int wi) {

	for (int i = 0; i < npoints; i++) {
		Cut_Point cp = cut_points[i];

		if (lvl < cp.lvl) return cp.cid;
		if (lvl == cp.lvl && wi <= cp.wi) return cp.cid;
	}
	fprintf(stderr, "Error: invalid lvl (%d) or warp (%d) for partition config\n", lvl, wi);
	exit(EXIT_FAILURE);
}

/**
 * \brief Compute the optical flow between \p src and \p tgt.
 *
 * The computation will be split among multiple ctrls as specified in \p lw2cid.
 * Tile creation, memory allocations and memory deallocation must be handled outside of this function.
 *
 * \param[in]  src                     source image
 * \param[in]  tgt                     tracked image
 * \param[in]  n_ctrls                 number of ctrls
 * \param[in]  alpha                   degree of displacement field smoothness
 * \param[in]  n_lvls                  number of levels in a pyramid
 * \param[in]  n_warps                 number of warping iterations per pyramid level
 * \param[in]  n_solves                number of solver iterations (Jacobi iterations)
 * \param[out] u                       horizontal displacement
 * \param[out] v                       vertical displacement
 * \param      pp_src,pp_tgt             downscaled images. Last lvl has refs to original images tiles \p src and \p tgt. Device only except for last lvl.
 * \param      pp_u,pp_v               partial results of levels and warps. Of size of the corresponding lvl, tiles for warps of the same lvl may be
 *                                     references to the same tile if handled by the same ctrl.
 *                                     On ctrl transition points tiles must be properly attached to the relevant ctrl and have host memory to allow device to device communication.
 *                                     Last tile of last lvl is a ref to \p u or \p v.
 * \param      p_tmp                   image after warping. Tiles of size of original image, one per ctrl participating in compute. Device only
 * \param      p_du0,p_dv0,p_du1,p_dv1 ancillary for jacobi iterations. Tiles of size of original image, one per ctrl participating in compute. Device only
 * \param      p_Ix,p_Iy,p_Iz          derivatives. Tiles of size of original image, one per ctrl participating in compute. Device only
 */
void ComputeFlow(HitTile_float src, HitTile_float tgt, int n_ctrls, float alpha, int n_lvls, int n_warps, int n_solves, HitTile_float u, HitTile_float v,
				 HitTile_float pp_src[n_lvls][n_ctrls], HitTile_float pp_tgt[n_lvls][n_ctrls], HitTile_float pp_u[n_lvls][n_warps + 1], HitTile_float pp_v[n_lvls][n_warps + 1],
				 HitTile_float p_tmp[n_ctrls], HitTile_float p_du0[n_ctrls], HitTile_float p_dv0[n_ctrls], HitTile_float p_du1[n_ctrls], HitTile_float p_dv1[n_ctrls],
				 HitTile_float p_Ix[n_ctrls], HitTile_float p_Iy[n_ctrls], HitTile_float p_Iz[n_ctrls]) {

	Ctrl_Thread thr_space[n_lvls];

	// +1 in both dims to force extra threads to copy last column and row of the
	// matrix in the first padding row and column to have boundary values during
	// stencil stage on solve kernel
	Ctrl_Thread thr_space_aug[n_lvls];

	Ctrl_TexDesc tex_desc      = {0};
	tex_desc.normalized_coords = true;
	tex_desc.filter_mode       = CTRL_TEX_FILTERMODE_LINEAR;
	tex_desc.read_mode         = CTRL_TEX_READMODE_ELEMTYPE;
	tex_desc.addr_mode[0]      = CTRL_TEX_ADDRMODE_MIRROR;
	tex_desc.addr_mode[1]      = CTRL_TEX_ADDRMODE_MIRROR;

	// prepare pyramid
	int lvl = n_lvls - 1;

	for (int cid = 0; cid < n_ctrls; cid++) {
		pp_src[lvl][cid] = src;
		pp_tgt[lvl][cid] = tgt;
		// NOTE using default tile size for textures
		Ctrl_CreateTex(Ctrl_Get(cid), pp_src[lvl][cid], tex_desc);
		Ctrl_CreateTex(Ctrl_Get(cid), pp_tgt[lvl][cid], tex_desc);
	}

	// last results are left on uv
	pp_u[lvl][n_warps] = u;
	pp_v[lvl][n_warps] = v;

	// NOTE I0 I1 are copied to dev here in ref

	Ctrl_ThreadInit(thr_space[lvl], hit_tileDimCard(src, 0), hit_tileDimCard(src, 1));
	Ctrl_ThreadInit(thr_space_aug[lvl], hit_tileDimCard(src, 0) + 1, hit_tileDimCard(src, 1) + 1);

	// Create lower resolution versions of both images (src y tgt)
	for (; lvl > 0; --lvl) {
		int nh = hit_tileDimCard(pp_src[lvl][0], 0) / 2;
		int nw = hit_tileDimCard(pp_src[lvl][0], 1) / 2;

		Ctrl_ThreadInit(thr_space[lvl - 1], nh, nw);
		Ctrl_ThreadInit(thr_space_aug[lvl - 1], nh + 1, nw + 1);

		for (int cid = 0; cid < n_ctrls; cid++) {
			// using default tile size for textures
			Ctrl_CreateTex(Ctrl_Get(cid), pp_src[lvl - 1][cid], tex_desc);
			Ctrl_CreateTex(Ctrl_Get(cid), pp_tgt[lvl - 1][cid], tex_desc);

			Ctrl_Launch(Ctrl_Get(cid), Downscale, thr_space[lvl - 1], CTRL_BLK_DEF, pp_src[lvl][cid], pp_src[lvl - 1][cid]);
			Ctrl_Launch(Ctrl_Get(cid), Downscale, thr_space[lvl - 1], CTRL_BLK_DEF, pp_tgt[lvl][cid], pp_tgt[lvl - 1][cid]);
		}
	}

	Ctrl_Launch(Ctrl_Get(Get_Cid(0, 0)), Zero, thr_space[0], CTRL_BLK_DEF, pp_u[0][0]);
	Ctrl_Launch(Ctrl_Get(Get_Cid(0, 0)), Zero, thr_space[0], CTRL_BLK_DEF, pp_v[0][0]);

	// Initial estimate (u, v) starts at 0
	for (; lvl < n_lvls; ++lvl) {
		for (int wi = 0; wi < n_warps; ++wi) {
			// Ctrl_Synchronize();
			// double wi_timer = omp_get_wtime();
			int   cid  = Get_Cid(lvl, wi); // ctrl id
			PCtrl ctrl = Ctrl_Get(cid);

			// cpu type ctrls shouldn't use the augmented thr space for solve kernels
			Ctrl_Thread *thr_space_solve = (!strcmp(Ctrl_GetInfo(ctrl).type, "CPU")) ? thr_space : thr_space_aug;

			// Texture creation
			tex_desc.width  = thr_space[lvl].j;
			tex_desc.height = thr_space[lvl].i;
			Ctrl_CreateTex(ctrl, p_tmp[cid], tex_desc);
			Ctrl_CreateTex(ctrl, pp_u[lvl][wi], tex_desc);
			Ctrl_CreateTex(ctrl, pp_v[lvl][wi], tex_desc);

			// Initialize p_du0, p_du1, p_dv0, p_dv1 to 0
			Ctrl_Launch(ctrl, Zero, thr_space[n_lvls - 1], CTRL_BLK_DEF, p_du0[cid]);
			Ctrl_Launch(ctrl, Zero, thr_space[n_lvls - 1], CTRL_BLK_DEF, p_dv0[cid]);

			Ctrl_Launch(ctrl, Zero, thr_space[n_lvls - 1], CTRL_BLK_DEF, p_du1[cid]);
			Ctrl_Launch(ctrl, Zero, thr_space[n_lvls - 1], CTRL_BLK_DEF, p_dv1[cid]);

			// Warp target image according to current estimate (u, v)
			Ctrl_Launch(ctrl, Warp, thr_space[lvl], CTRL_BLK_DEF, pp_tgt[lvl][cid], pp_u[lvl][wi], pp_v[lvl][wi], p_tmp[cid]);

			// Compute matrices of the equation to solve
			Ctrl_Launch(ctrl, ComputeDerivatives, thr_space[lvl], CTRL_BLK_DEF, pp_src[lvl][cid], p_tmp[cid], p_Ix[cid], p_Iy[cid], p_Iz[cid]);

			// Solve equation for du, dv
			for (int iter = 0; iter < n_solves; ++iter) {
				// threads are +1 in both dims because of the way the copy to shared mem is done
				Ctrl_Launch(ctrl, Solve, thr_space_solve[lvl], CTRL_BLK_DEF, p_du0[cid], p_dv0[cid], p_Ix[cid], p_Iy[cid], p_Iz[cid], alpha, p_du1[cid], p_dv1[cid]);

				hit_tileSwap(p_du0[cid], p_du1[cid]);
				hit_tileSwap(p_dv0[cid], p_dv1[cid]);
			}

			// Update current estimate
			Ctrl_Launch(ctrl, Add, thr_space[lvl], CTRL_BLK_DEF, pp_u[lvl][wi], p_du0[cid], pp_u[lvl][wi + 1]);
			Ctrl_Launch(ctrl, Add, thr_space[lvl], CTRL_BLK_DEF, pp_v[lvl][wi], p_dv0[cid], pp_v[lvl][wi + 1]);

			// Ctrl_Synchronize();
			// printf("lvl %d wi %d cid %d time %lf\n", lvl, wi, cid, omp_get_wtime() - wi_timer);
			// fflush(stdout);
		}

		// Prolongate solution (u, v) for use in the next level
		if (lvl != n_lvls - 1) {
			PCtrl ctrl   = Ctrl_Get(Get_Cid(lvl, n_warps - 1));
			float scaleX = (float)hit_width(pp_src[lvl + 1][0]) / hit_width(pp_src[lvl][0]);
			Ctrl_Launch(ctrl, Upscale, thr_space[lvl + 1], CTRL_BLK_DEF, pp_u[lvl][n_warps], scaleX, pp_u[lvl + 1][0]);

			float scaleY = (float)hit_height(pp_src[lvl + 1][0]) / hit_height(pp_src[lvl][0]);
			Ctrl_Launch(ctrl, Upscale, thr_space[lvl + 1], CTRL_BLK_DEF, pp_v[lvl][n_warps], scaleY, pp_v[lvl + 1][0]);
		}
	}
}

int main(int argc, char *argv[]) {
	Ctrl_Init(&argc, &argv);
	main_clock = omp_get_wtime();

	// 1. Taking arguments
	if (argc != 8) {
		fprintf(stderr, "Usage: %s <alpha> <nLevels> <nSolverIters> <nWarpIters> <nFrames> <frameDir> <config_file> \n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const float alpha    = atof(argv[1]); // smoothness if image brightness is not within [0,1] this paramter should be scaled appropriately (0.2f)
	const int   n_lvls   = atoi(argv[2]); // number of pyramid levels (5)
	const int   n_solves = atoi(argv[3]); // number of solver iterations on each level (500)
	const int   n_warps  = atoi(argv[4]); // number of warping iterations (3)
	const int   n_frames = atoi(argv[5]); // number of frames
	// find images
	const char *video_path = argv[6]; // path to directory containing the frames

	char *ctrl_conf_file = argv[7];

	__ctrl_block__(ctrl_conf_file) {
		// 2. Get controller object and print info
		int   n_ctrls = Ctrl_GetNCtrls();
		PCtrl ctrls[n_ctrls];
		for (int i = 0; i < n_ctrls; i++) {
			ctrls[i] = Ctrl_Get(i);
		}
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n ----------------------- ARGS ------------------------- \n");
		printf("\n ALPHA: %g", alpha);
		printf("\n LEVELS: %d", n_lvls);
		printf("\n SOLVERITERS: %d", n_solves);
		printf("\n WARPITERS: %d", n_warps);
		printf("\n FRAMES: %d", n_frames);
		printf("\n FRAMES PATH: %s", video_path);
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		Ctrl_PrintInfo();
		#ifndef _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n ---------------------------------------------------- \n");
		#endif // _CTRL_EXAMPLES_EXP_MODE_
		fflush(stdout);

		// allocs work partition
		InitWorkPartition();
		// add cut points

		switch (n_ctrls - 1) {
			case 1:
				AddCutPoint(0, 4, 2);
				break;
			case 2:
				// gfx900, v100
				// AddCutPoint(0, 3, 2);
				// AddCutPoint(1, 4, 2);

				// v100, gfx900
				// AddCutPoint(0, 4, 1);
				// AddCutPoint(1, 4, 2);

				// rtx4500 gfx1100
				AddCutPoint(0, 4, 0);
				AddCutPoint(1, 4, 2);

				// a100 rtx4500/gfx1100
				// AddCutPoint(0, 4, 1);
				// AddCutPoint(1, 4, 2);

				// rtx4500/gfx1100 a100
				// AddCutPoint(0, 3, 2);
				// AddCutPoint(1, 4, 3);
			case 3:
				// 2x rtx4500 1x a100
				AddCutPoint(0, 3, 0);
				AddCutPoint(1, 3, 2);
				AddCutPoint(2, 4, 2);
				break;
			case 4:
				// 1x gfx1100 2x rtx4500 1x a100
				AddCutPoint(0, 3, 0);
				AddCutPoint(1, 3, 2);
				AddCutPoint(2, 4, 0);
				AddCutPoint(3, 4, 2);
				break;
			default:
				fprintf(stderr, "ERROR: No config for %d devices\n", n_ctrls - 1);
				exit(EXIT_FAILURE);
		}

		// 3. Create tiles and alloc memory
		// get frames size
		char           frame_path[strlen(video_path) + 9];
		unsigned int   channels, w, h;
		unsigned char *aux = NULL;
		if (!__loadPPM(strcat(strcpy(frame_path, video_path), "/001.ppm"), &aux, &w, &h, &channels)) {
			exit(EXIT_FAILURE);
		}
		free(aux);

		// size of the pipeline, last ctrl is used for the norm
		int pipe_len = n_ctrls + 1;
		// prevent private allocs from last ctrl as that one is for the norm
		n_ctrls--;

		// alloc memory for frames
		HitTile_float frames[pipe_len];
		for (int i = 0; i < pipe_len; i++) {
			frames[i] = Ctrl_Domain(float, hitShapeSize(h, w));
			for (int cid = 0; cid < n_ctrls; cid++) {
				Ctrl_Alloc(ctrls[cid], frames[i], CTRL_MEM_ALIGNED);
			}
		}

		// ancillary tiles for computation, internal to each ctrl
		HitTile_float p_tmp[n_ctrls];
		HitTile_float p_du0[n_ctrls];
		HitTile_float p_dv0[n_ctrls];
		HitTile_float p_du1[n_ctrls];
		HitTile_float p_dv1[n_ctrls];
		HitTile_float p_Ix[n_ctrls];
		HitTile_float p_Iy[n_ctrls];
		HitTile_float p_Iz[n_ctrls];

		for (int cid = 0; cid < n_ctrls; cid++) {
			p_tmp[cid] = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			p_du0[cid] = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			p_dv0[cid] = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			p_du1[cid] = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			p_dv1[cid] = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			p_Ix[cid]  = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			p_Iy[cid]  = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			p_Iz[cid]  = Ctrl_DomainAlloc(ctrls[cid], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
		}

		// tiles for downscaled images, internal to each ctrl
		HitTile_float pp_src[n_lvls][n_ctrls];
		HitTile_float pp_tgt[n_lvls][n_ctrls];

		pp_src[n_lvls - 1][0] = frames[0]; // last lvl has same shape as the original image
		for (int lvl = n_lvls - 1; lvl > 0; --lvl) {
			int nh = hit_tileDimCard(pp_src[lvl][0], 0) / 2;
			int nw = hit_tileDimCard(pp_src[lvl][0], 1) / 2;

			HitShape new_shape = hitShapeSize(nh, nw);
			for (int cid = 0; cid < n_ctrls; cid++) {
				pp_src[lvl - 1][cid] = Ctrl_DomainAlloc(ctrls[cid], float, new_shape, CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
				pp_tgt[lvl - 1][cid] = Ctrl_DomainAlloc(ctrls[cid], float, new_shape, CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			}
		}

		// tiles for partial results, allocs acording to ComputeFlow docs
		HitTile_float pp_u[n_lvls][n_warps + 1];
		HitTile_float pp_v[n_lvls][n_warps + 1];
		for (int lvl = 0; lvl < n_lvls; lvl++) {
			HitShape shape = hit_tileShape(pp_src[lvl][0]);
			// alloc first tile for each lvl
			pp_u[lvl][0] = Ctrl_DomainAlloc(ctrls[Get_Cid(lvl, 0)], float, shape, CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			pp_v[lvl][0] = Ctrl_DomainAlloc(ctrls[Get_Cid(lvl, 0)], float, shape, CTRL_MEM_ALIGNED | CTRL_MEM_ALLOC_DEV);
			if (lvl != 0 && Get_Cid(lvl, 0) != Get_Cid(lvl - 1, n_warps - 1)) {
				// connection to ctrl of the prev lvl if different
				Ctrl_Alloc(ctrls[Get_Cid(lvl - 1, n_warps - 1)], pp_u[lvl][0], CTRL_MEM_ALIGNED);
				Ctrl_Alloc(ctrls[Get_Cid(lvl - 1, n_warps - 1)], pp_v[lvl][0], CTRL_MEM_ALIGNED);
			}
			for (int wi = 1; wi < n_warps + 1; wi++) {
				// if warps are handled by different ctrls create a new tile else copy previous
				if (wi < n_warps && Get_Cid(lvl, wi) != Get_Cid(lvl, wi - 1)) {
					pp_u[lvl][wi] = Ctrl_DomainAlloc(ctrls[Get_Cid(lvl, wi)], float, shape, CTRL_MEM_ALIGNED);
					pp_v[lvl][wi] = Ctrl_DomainAlloc(ctrls[Get_Cid(lvl, wi)], float, shape, CTRL_MEM_ALIGNED);
					Ctrl_Alloc(ctrls[Get_Cid(lvl, wi - 1)], pp_u[lvl][wi], CTRL_MEM_ALIGNED);
					Ctrl_Alloc(ctrls[Get_Cid(lvl, wi - 1)], pp_v[lvl][wi], CTRL_MEM_ALIGNED);
				} else {
					pp_u[lvl][wi] = pp_u[lvl][wi - 1];
					pp_v[lvl][wi] = pp_v[lvl][wi - 1];
				}
			}
		}

		HitTile_float u = Ctrl_DomainAlloc(ctrls[Get_Cid(n_lvls - 1, n_warps - 1)], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED);
		HitTile_float v = Ctrl_DomainAlloc(ctrls[Get_Cid(n_lvls - 1, n_warps - 1)], float, hit_tileShape(frames[0]), CTRL_MEM_ALIGNED);
		Ctrl_Alloc(ctrls[n_ctrls], u);
		Ctrl_Alloc(ctrls[n_ctrls], v);

		double *p_sum = (double *)malloc(sizeof(double) * (n_frames - 1));
		double *p_res = (double *)malloc(sizeof(double) * (n_frames - 1));

		// 4. Sync and start timer
		Ctrl_Synchronize();
		exec_clock = omp_get_wtime();

		// 5. Main computation loop
		int one = 1;
		Ctrl_HostTask(Load_frame, frames[0], video_path, one);
		for (int i = 2; i < n_frames + 1; i++) {
			HitTile_float src = frames[(i - 2) % pipe_len];
			HitTile_float tgt = frames[(i - 1) % pipe_len];
			Ctrl_HostTask(Load_frame, tgt, video_path, i);

			ComputeFlow(src, tgt, n_ctrls, alpha, n_lvls, n_warps, n_solves, u, v,
						pp_src, pp_tgt, pp_u, pp_v, p_tmp,
						p_du0, p_dv0, p_du1, p_dv1,
						p_Ix, p_Iy, p_Iz);
			Ctrl_Launch(ctrls[n_ctrls], Norm, CTRL_THREAD_NULL, CTRL_THREAD_NULL, u, v, p_sum, p_res, i);
		}

		// 6. Sync and stop timer
		Ctrl_Synchronize();
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

		// This should not be neccessary but there are weird issues with the freeing of shared tiles in opencl ctrls
		// Ctrl_Synchronize();

		// 7. Tile cleanup
		for (int i = 0; i < n_lvls - 1; ++i) {
			for (int cid = 0; cid < n_ctrls; cid++) {
				Ctrl_Free(ctrls[cid], pp_src[i][cid], pp_tgt[i][cid]);
			}
		}

		for (int i = 0; i < pipe_len; i++) {
			for (int cid = 0; cid < n_ctrls; cid++) {
				Ctrl_Free(ctrls[cid], frames[i]);
			}
		}

		for (int lvl = 0; lvl < n_lvls; lvl++) {
			for (int wi = 0; wi < n_warps - 1; wi++) {
				if (Get_Cid(lvl, wi) != Get_Cid(lvl, wi + 1)) {
					Ctrl_Free(ctrls[Get_Cid(lvl, wi)], pp_u[lvl][wi + 1], pp_v[lvl][wi + 1]);
					Ctrl_Free(ctrls[Get_Cid(lvl, wi)], pp_u[lvl][wi], pp_v[lvl][wi]);
				}
			}
			Ctrl_Free(ctrls[Get_Cid(lvl, n_warps - 1)], pp_u[lvl][n_warps - 1], pp_v[lvl][n_warps - 1]);
			if (lvl != n_lvls - 1 && Get_Cid(lvl, n_warps - 1) != Get_Cid(lvl + 1, 0)) {
				Ctrl_Free(ctrls[Get_Cid(lvl, n_warps - 1)], pp_u[lvl + 1][0], pp_v[lvl + 1][0]);
			}
		}

		Ctrl_Free(ctrls[Get_Cid(n_lvls - 1, n_warps - 1)], u, v);
		Ctrl_Free(ctrls[n_ctrls], u, v);

		for (int cid = 0; cid < n_ctrls; cid++) {
			Ctrl_Free(ctrls[cid], p_tmp[cid]);
			Ctrl_Free(ctrls[cid], p_du0[cid], p_dv0[cid]);
			Ctrl_Free(ctrls[cid], p_du1[cid], p_dv1[cid]);
			Ctrl_Free(ctrls[cid], p_Ix[cid], p_Iy[cid], p_Iz[cid]);
		}

		free(p_sum);
		free(p_res);

		DestroyWorkPartition();

		// 8. Destroy the controller
		Ctrl_EndBlock();
	}

	// 9. Stop main timer and print times
	main_clock = omp_get_wtime() - main_clock;

	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf("%lf, %lf\n", main_clock, exec_clock);
	#else // _CTRL_EXAMPLES_EXP_MODE_
	printf("\n ---------------------- TIMERS ---------------------- \n\n");
	printf(" Clock main: %lf\n", main_clock);
	printf(" Clock exec: %lf\n", exec_clock);
	printf("\n ---------------------------------------------------- \n");
	#endif // _CTRL_EXAMPLES_EXP_MODE_

	Ctrl_Finalize();
	return EXIT_SUCCESS;
}
