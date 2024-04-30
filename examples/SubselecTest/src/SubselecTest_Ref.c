/**
 * @file Ref.c
 * @author Trasgo Group
 * @brief SubselecTest: Reference Hitmap version
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

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "Ctrl.h"

hit_tileNewType(int);

void some_kernel(HitTile_int tile) {
	for (int i = 0; i < hit_tileDimCard(tile, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(tile, 1); j++) {
			hitStrided(tile, i, j) *= hitStrided(tile, i, j);
		}
	}
}

void init_tile(HitTile_int tile) {
	for (int i = 0; i < hit_tileDimCard(tile, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(tile, 1); j++) {
			hit(tile, i, j) = i * hit_tileDimCard(tile, 1) + j;
		}
	}
}

void print_matrix(HitTile_int tile) {
	for (int i = 0; i < hit_tileDimCard(tile, 0); i++) {
		for (int j = 0; j < hit_tileDimCard(tile, 1); j++) {
			printf("%5d ", hit(tile, i, j));
		}
		printf("\n");
	}
	printf("\n");
	fflush(stdout);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "\nUsage: %s <size>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int SIZE = atoi(argv[1]);

	HitTile_int matrix, subselec_whole, subselec_rows, subselec_cols, subselec_submatrix;
	HitTile_int subselec_stride, subselec_corners, subselec_null;

	HitShape matrix_shape             = hitShapeSize(SIZE, SIZE);
	HitShape subselec_whole_shape     = HIT_SHAPE_WHOLE;
	HitShape subselec_rows_shape      = hitShapeSize(SIZE / 4, SIZE);
	HitShape subselec_cols_shape      = hitShapeSize(SIZE, SIZE / 4);
	HitShape subselec_submatrix_shape = hitShape((SIZE / 4, 3 * SIZE / 4 - 1, 1), (SIZE / 4, 3 * SIZE / 4 - 1, 1));
	HitShape subselec_stride_shape    = hitShape((0, SIZE - 1, 2), (0, SIZE - 1, 2));
	HitShape subselec_corners_shape   = hitShape((0, SIZE - 1, SIZE - 1), (0, SIZE - 1, SIZE - 1));
	HitShape subselec_null_shape      = HIT_SHAPE_NULL;

	hit_tileDomainShapeAlloc(&matrix, int, matrix_shape);

	hit_tileSelect(&subselec_whole, &matrix, subselec_whole_shape);
	hit_tileSelect(&subselec_rows, &matrix, subselec_rows_shape);
	hit_tileSelect(&subselec_cols, &matrix, subselec_cols_shape);
	hit_tileSelect(&subselec_submatrix, &matrix, subselec_submatrix_shape);
	hit_tileSelect(&subselec_stride, &matrix, subselec_stride_shape);
	hit_tileSelect(&subselec_corners, &matrix, subselec_corners_shape);
	hit_tileSelect(&subselec_null, &matrix, subselec_null_shape);

	init_tile(matrix);

	printf("=== ORIGINAL MATRIX ===\n\n");
	print_matrix(matrix);

	some_kernel(subselec_whole);

	printf("=== WHOLE SUBSELECTION MODIFIED ===\n\n");
	print_matrix(matrix);

	init_tile(matrix);

	some_kernel(subselec_rows);

	printf("=== ROWS SUBSELECTION MODIFIED ===\n\n");
	print_matrix(matrix);

	init_tile(matrix);

	some_kernel(subselec_cols);

	printf("=== COLUMNS SUBSELECTION MODIFIED ===\n\n");
	print_matrix(matrix);

	init_tile(matrix);

	some_kernel(subselec_submatrix);

	printf("=== SUBMATRIX SUBSELECTION MODIFIED ===\n\n");
	print_matrix(matrix);

	init_tile(matrix);

	some_kernel(subselec_stride);

	printf("=== STRIDED SUBSELECTION MODIFIED ===\n\n");
	print_matrix(matrix);

	init_tile(matrix);

	some_kernel(subselec_corners);

	printf("=== CORNERS SUBSELECTION MODIFIED ===\n\n");
	print_matrix(matrix);

	init_tile(matrix);

	some_kernel(subselec_null);

	printf("=== NULL SUBSELECTION MODIFIED ===\n\n");
	print_matrix(matrix);

	return EXIT_SUCCESS;
}
