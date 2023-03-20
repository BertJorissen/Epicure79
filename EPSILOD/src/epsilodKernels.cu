/*
 * epsilodKernels.cu
 * 	Ctrl example
 * 	Stencil code: Any dimensions, stencil as a pattern of weights.
 *   Kernel code.
 *
 * v1.0
 * (c) 2019-2023, Arturo Gonzalez-Escribano, Yuri Torres de la Sierra, Manuel de Castro Caballero
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
 * Copyright (c) 2007-2023, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * More informatrixion on http://trasgo.infor.uva.es/
 *
 * </license>
 */

#include "Ctrl_Kernels.h"

Ctrl_NewType(float);

CTRL_KERNEL(updateCell_1dNC4, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, {
	int x = thread_id_x;

	hit(matrix, x) = (0.5 * (hit(matrixCopy, x - 2) + hit(matrixCopy, x + 2)) +
					  hit(matrixCopy, x - 1) + hit(matrixCopy, x + 1)) /
					 3;
});

CTRL_KERNEL(updateCell_1dC2, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, {
	int x = thread_id_x;

	hit(matrix, x) = (hit(matrixCopy, x - 1) + hit(matrixCopy, x + 1)) / 2;
});

/* A.1. CELL UPDATE 4-POINT */
CTRL_KERNEL(updateCell_4, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, {
	int x = thread_id_x;
	int y = thread_id_y;

	hit(matrix, x, y) = (hit(matrixCopy, x - 1, y) +
						 hit(matrixCopy, x + 1, y) +
						 hit(matrixCopy, x, y - 1) +
						 hit(matrixCopy, x, y + 1)) /
						4;
});

/* A.2. CELL UPDATE 9-POINT */
CTRL_KERNEL(updateCell_9, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, {
	int x = thread_id_x;
	int y = thread_id_y;

	hit(matrix, x, y) = (4 * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x + 1, y) + hit(matrixCopy, x, y - 1) + hit(matrixCopy, x, y + 1)) +
						 (hit(matrixCopy, x - 1, y - 1) + hit(matrixCopy, x + 1, y - 1) + hit(matrixCopy, x - 1, y + 1) + hit(matrixCopy, x + 1, y + 1))) /
						20;
});

/* A.3. CELL UPDATE 9-POINT, NON-COMPACT, NO CORNERS */
CTRL_KERNEL(updateCell_NC9, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, {
	int x = thread_id_x;
	int y = thread_id_y;

	hit(matrix, x, y) = ((hit(matrixCopy, x - 2, y) + hit(matrixCopy, x + 2, y) + hit(matrixCopy, x, y - 2) + hit(matrixCopy, x, y + 2)) +
						 4 * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x + 1, y) + hit(matrixCopy, x, y - 1) + hit(matrixCopy, x, y + 1))) /
						20;
});

/* A.3. CELL UPDATE 5-POINT, FORWARD RIGHT-DOWN, ONE ELEMENT IN THE CORNER */
CTRL_KERNEL(updateCell_F5, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, {
	int x = thread_id_x;
	int y = thread_id_y;

	hit(matrix, x, y) = (2.0f * (hit(matrixCopy, x - 1, y) + hit(matrixCopy, x, y - 1)) +
						 (hit(matrixCopy, x - 2, y) + hit(matrixCopy, x, y - 2)) +
						 .5f * hit(matrixCopy, x - 1, y - 1)) /
						6.5f;
});

/* A.2. CELL UPDATE 3D 27-POINT */
CTRL_KERNEL(updateCell_3d27, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, {
	int x = thread_id_x;
	int y = thread_id_y;
	int z = thread_id_z;

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

/* A.3. CELL UPDATE DEFAULT STENCIL */
CTRL_KERNEL(updateCell_default_1D, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const KHitTile_float weight, const int begin_x, const int end_x, const float factor, {
	int x = thread_id_x;

	float sum = 0;

	int i;
	for (i = begin_x; i <= end_x; i++) {
		if (!hit(weight, i - begin_x)) continue;
		sum += hit(matrixCopy, x + i) * hit(weight, i - begin_x);
	}
	hit(matrix, x) = sum / factor;
});

CTRL_KERNEL(updateCell_default_2D, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const KHitTile_float weight, const int begin_x, const int begin_y, const int end_x, const int end_y, const float factor, {
	int x = thread_id_x;
	int y = thread_id_y;

	float sum = 0;

	int i;
	int j;
	for (i = begin_x; i <= end_x; i++)
		for (j = begin_y; j <= end_y; j++) {
			if (!hit(weight, i - begin_x, j - begin_y)) continue;
			sum += hit(matrixCopy, x + i, y + j) * hit(weight, i - begin_x, j - begin_y);
		}
	hit(matrix, x, y) = sum / factor;
});

CTRL_KERNEL(updateCell_default_3D, GENERIC, DEFAULT, KHitTile_float matrix, const KHitTile_float matrixCopy, const KHitTile_float weight, const int begin_x, const int begin_y, const int begin_z, const int end_x, const int end_y, const int end_z, const float factor, {
	int x = thread_id_x;
	int y = thread_id_y;
	int z = thread_id_z;

	float sum = 0;

	int i;
	int j;
	int k;
	for (i = begin_x; i <= end_x; i++)
		for (j = begin_y; j <= end_y; j++)
			for (k = begin_z; k <= end_z; k++) {
				if (!hit(weight, i - begin_x, j - begin_y, k - begin_z)) continue;
				sum += hit(matrixCopy, x + i, y + j, z + k) * hit(weight, i - begin_x, j - begin_y, k - begin_z);
			}
	hit(matrix, x, y, z) = sum / factor;
});
