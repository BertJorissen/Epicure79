#include "Ctrl.h"
#include "Texture_Test_Ctrl.h"

#ifdef _PROFILING_ENABLED_
#ifdef _CTRL_ARCH_CUDA_
#include "nvToolsExt.h"
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_OPENCL_GPU_
#include <roctx.h>
#endif // _CTRL_ARCH_OPENCL_GPU_
#endif //_PROFILING_ENABLED_

Ctrl_NewType(float);

CTRL_KERNEL(TexTest, CUDA, DEFAULT, CTRL_KPARAMS(textest_params), {
	hit(matrix_result, thr_i, thr_j) = tex2D<float>(matrix_a.ext.cuda.tex, thr_j, thr_i);
});

CTRL_KERNEL(TexTest, OPENCLGPU, DEFAULT, CTRL_KPARAMS(textest_params), {
	hit(matrix_result, thr_i, thr_j) = read_imagef(matrix_a_img, matrix_a_smp, (int2)(thr_j, thr_i)).x;
});