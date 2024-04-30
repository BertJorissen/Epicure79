#include "Ctrl.h"

Ctrl_NewType(float);

CTRL_KERNEL(Add, GENERIC, DEFAULT, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C, {
	for (int k = 0; k < n_iter; k++) {
		hit(C, thr_i, thr_j) =
			hit(C, thr_i, thr_j) +
			hit(A, thr_i, thr_j) +
			hit(B, thr_i, thr_j);
	}
});
