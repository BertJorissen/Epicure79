#include "../../../Utils/profiler_utils.h"
#include "kernels_params.h"
#include <math.h>
/* E. Host task to calculate and print the norm */
CTRL_KERNEL(Norm, CPU, DEFAULT, CTRL_KPARAMS(norm_params), {
	PROF_RANGEPUSH("Norm calc");

	double sum = 0;

	int h = hit_tileDimCard(u, 0);
	int w = hit_tileDimCard(u, 1);

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			sum += fabsf(hit(u, i, j)) + fabsf(hit(v, i, j));
		}
	}
	// -2 because frame refers to the target frame and frames start on 1
	p_sum[frame - 2] = sum;
	p_res[frame - 2] = sum / (double)(w * h);

	PROF_RANGEPOP();
});
