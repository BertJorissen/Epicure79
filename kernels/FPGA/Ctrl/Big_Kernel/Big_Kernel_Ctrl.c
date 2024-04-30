#define CTRL_FPGA_KERNEL_FILE

#include "Ctrl.h"
CTRL_KERNEL_FUNCTION(big_kernel, FPGA, DEFAULT, PIPELINE(TASK), PARAMS(IO, IO, OUT, IO, OUT, IN, IO, IO, IO, IO, IO, IO), KHitTile_long tile1, KHitTile_long tile2, KHitTile_long tile3, KHitTile_long tile4, KHitTile_long tile5, KHitTile_long tile6, KHitTile_long dummy1, KHitTile_long dummy2, KHitTile_long dummy3, KHitTile_long dummy4, KHitTile_long dummy5, KHitTile_long dummy6)