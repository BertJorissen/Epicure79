#include "kernels_params.h"

// TODO move this to utils using cudalib default
CTRL_KERNEL(Zero, GENERIC, DEFAULT, CTRL_KPARAMS(zero_params), {
	hit(tile, thr_i, thr_j) = 0;
})