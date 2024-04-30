#define CTRL_FPGA_KERNEL_FILE

#include "Ctrl.h"
Ctrl_NewType(long);

CTRL_KERNEL_FUNCTION(big_kernel, FPGA, DEFAULT, PIPELINE(TASK), PARAMS(IO, IO, OUT, IO, OUT, IN, IO, IO, IO, IO, IO, IO), KHitTile_long tile1, KHitTile_long tile2, KHitTile_long tile3, KHitTile_long tile4, KHitTile_long tile5, KHitTile_long tile6, KHitTile_long dummy1, KHitTile_long dummy2, KHitTile_long dummy3, KHitTile_long dummy4, KHitTile_long dummy5, KHitTile_long dummy6) {
	hit(tile1, thread_id.x, thread_id.y) *= hit(tile1, thread_id.x, thread_id.y);
	hit(tile2, thread_id.y, thread_id.x) = -hit(tile2, thread_id.y, thread_id.x);
	hit(tile3, thread_id.x, thread_id.y) = 0;
	hit(tile4, thread_id.y, thread_id.x) %= 2;
	hit(tile5, thread_id.x, thread_id.y) = hit(tile6, thread_id.y, thread_id.x);
}
