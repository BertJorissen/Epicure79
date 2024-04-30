/*
 * EPSILOD: test_parallelStencilSkeleton_kernels.cu
 * 	Example with several key stencils
 *	Kernels code.
 *
 * v1.1
 * (c) 2019-2021, Arturo Gonzalez-Escribano, Yuri Torres de la Sierra, Manuel de Castro Caballero
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
 * Copyright (c) 2007-2021, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * More informatrixion on http://trasgo.infor.uva.es/
 *
 * </license>
 */

#include "epsilod_ext_type.h"

#include "Ctrl.h"

Ctrl_NewType(float);

/* A.1. 1D NON-COMPACT RADIUS 2 */
CTRL_KERNEL(updateCell_1dNC4, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;

	hit(matrix, x) = (0.5 * (hit(matrixCopy, x - 2) + hit(matrixCopy, x + 2)) +
					  hit(matrixCopy, x - 1) + hit(matrixCopy, x + 1)) /
					 3;
});

/* A.1. 1D COMPACT RADIUS 1 */
CTRL_KERNEL(updateCell_1dC2, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;

	hit(matrix, x) = (hit(matrixCopy, x - 1) + hit(matrixCopy, x + 1)) / 2;
});

/* B.1. 2D COMPACT, RADIUS 1: 4-POINT STAR, NO CORNERS */
CTRL_KERNEL(updateCell_4, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = (hit(matrixCopy, x - 1, y) +
						 hit(matrixCopy, x + 1, y) +
						 hit(matrixCopy, x, y - 1) +
						 hit(matrixCopy, x, y + 1)) /
						4;
});

/* B.2. 2D COMPACT, RADIUS 1: 9-POINT STAR, CORNERS INCLUDED */
CTRL_KERNEL(updateCell_9, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = (4 * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x + 1, y) + hit(matrixCopy, x, y - 1) + hit(matrixCopy, x, y + 1)) +
						 (hit(matrixCopy, x - 1, y - 1) + hit(matrixCopy, x + 1, y - 1) + hit(matrixCopy, x - 1, y + 1) + hit(matrixCopy, x + 1, y + 1))) /
						20;
});

/* B.3. 2D NON-COMPACT, RADIUS 2: 9-POINT STAR, NO CORNERS */
CTRL_KERNEL(updateCell_NC9, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = ((hit(matrixCopy, x - 2, y) + hit(matrixCopy, x + 2, y) + hit(matrixCopy, x, y - 2) + hit(matrixCopy, x, y + 2)) +
						 4 * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x + 1, y) + hit(matrixCopy, x, y - 1) + hit(matrixCopy, x, y + 1))) /
						20;
});

/* B.3. 2D NON-COMPACT, NON-SYMMETRIC. RADIUS 2: 5-POINT STAR
 * FORWARD-DOWN WITH ONE CORNER ELEMENT */
CTRL_KERNEL(updateCell_F5, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;

	hit(matrix, x, y) = (2.0f * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x, y - 1)) +
						 (hit(matrixCopy, x - 2, y) + hit(matrixCopy, x, y - 2)) +
						 .5f * hit(matrixCopy, x - 1, y - 1)) /
						6.5f;
});

/* C.1. 3D COMPACT. RADIUS 1: 27-POINT STAR */
CTRL_KERNEL(updateCell_3d27, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const Epsilod_ext ext_params, {
	int x = thr_i;
	int y = thr_j;
	int z = thr_k;

	hit(matrix, x, y, z) = (hit(matrixCopy, x - 1, y - 1, z - 1) +
							hit(matrixCopy, x - 1, y - 1, z) +
							hit(matrixCopy, x - 1, y - 1, z + 1) +
							hit(matrixCopy, x - 1, y, z - 1) +
							hit(matrixCopy, x - 1, y, z) +
							hit(matrixCopy, x - 1, y, z + 1) +
							hit(matrixCopy, x - 1, y + 1, z - 1) +
							hit(matrixCopy, x - 1, y + 1, z) +
							hit(matrixCopy, x - 1, y + 1, z + 1) +
							hit(matrixCopy, x, y - 1, z - 1) +
							hit(matrixCopy, x, y - 1, z) +
							hit(matrixCopy, x, y - 1, z + 1) +
							hit(matrixCopy, x, y, z - 1) +
							hit(matrixCopy, x, y, z) +
							hit(matrixCopy, x, y, z + 1) +
							hit(matrixCopy, x, y + 1, z - 1) +
							hit(matrixCopy, x, y + 1, z) +
							hit(matrixCopy, x, y + 1, z + 1) +
							hit(matrixCopy, x + 1, y - 1, z - 1) +
							hit(matrixCopy, x + 1, y - 1, z) +
							hit(matrixCopy, x + 1, y - 1, z + 1) +
							hit(matrixCopy, x + 1, y, z - 1) +
							hit(matrixCopy, x + 1, y, z) +
							hit(matrixCopy, x + 1, y, z + 1) +
							hit(matrixCopy, x + 1, y + 1, z - 1) +
							hit(matrixCopy, x + 1, y + 1, z) +
							hit(matrixCopy, x + 1, y + 1, z + 1)) /
						   27;
});
