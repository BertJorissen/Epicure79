#include "Ctrl_Kernels.h"

Ctrl_NewType( float );

CTRL_KERNEL(Add, GENERIC, DEFAULT, int n_iter, KHitTile_float A, KHitTile_float B, KHitTile_float C,
{
	for (int k = 0; k < n_iter; k++) {
		hit(C, thread_id.x, thread_id.y) =
			hit(C, thread_id.x, thread_id.y) +
			hit(A, thread_id.x, thread_id.y) +
			hit(B, thread_id.x, thread_id.y);
	}
});
