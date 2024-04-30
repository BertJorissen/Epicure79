#define CTRL_FPGA_KERNEL_FILE

#include "Ctrl.h"
Ctrl_NewType(long);

CTRL_KERNEL_FUNCTION(some_kernel, FPGA, DEFAULT, PIPELINE(TASK), PARAMS(IO), KHitTile_long tile) {
	hit(tile, thread_id.x, thread_id.y) *= hit(tile, thread_id.x, thread_id.y);
	// hitStrided(tile, thread_id.x, thread_id.y) *= hitStrided(tile, thread_id.x, thread_id.y);
}
