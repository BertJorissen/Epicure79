#define CTRL_FPGA_KERNEL_FILE

#include "Ctrl.h"
CTRL_KERNEL_FUNCTION(Sobel, FPGA, DEFAULT, PIPELINE(TASK), PARAMS(IN, OUT, INVAL, INVAL), KHitTile_BYTE frame_in, KHitTile_BYTE frame_out,
					 const int iterations, const unsigned int threshold)