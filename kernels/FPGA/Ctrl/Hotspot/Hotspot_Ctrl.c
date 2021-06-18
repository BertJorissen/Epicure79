#define CTRL_FPGA_KERNEL_FILE

#include "hotspot_common.h"
#include "Ctrl.h"
CTRL_KERNEL_FUNCTION(Hotspot, FPGA, DEFAULT, PIPELINE( TASK ), PARAMS( IN, IN, OUT, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL ), 
			KHitTile_float pwr_in, KHitTile_float src, KHitTile_float dst,
			int grid_cols, int grid_rows, float sdc, float Rx_1, float Ry_1, float Rz_1, int comp_exit)