#define CTRL_FPGA_KERNEL_FILE

#include "Ctrl.h"
CTRL_KERNEL_FUNCTION(Mult, FPGA, DEFAULT, PIPELINE( NDRANGE, SIMD( 4, BLOCK_SIZE, BLOCK_SIZE, 1) ), PARAMS( OUT, IN, IN, INVAL, INVAL ), 
            KHitTile_float C, KHitTile_float A, KHitTile_float B, int A_width, int B_width)
