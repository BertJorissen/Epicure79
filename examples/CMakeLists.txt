# <license>
# 
# Controller v4.0
# 
# This software is provided to enhance knowledge and encourage progress in the scientific
# community. It should be used only for research and educational purposes. Any reproduction
# or use for commercial purpose, public redistribution, in source or binary forms, with or 
# without modifications, is NOT ALLOWED without the previous authorization of the copyright 
# holder. The origin of this software must not be misrepresented; you must not claim that you
# wrote the original software. If you use this software for any purpose (e.g. publication),
# a reference to the software package and the authors must be included.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
# All rights reserved.
# 
# More information on http://trasgo.infor.uva.es/
# 
# </license>
cmake_minimum_required (VERSION 3.17)

# Compilation verbose info
if (CTRL_CMAKE_VERBOSE_EXAMPLES)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES_HOTSPOT ON)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_ADD ON)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_INCREMENT ON)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES_CHAIN_MATMUL ON)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_POWER_OF ON)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES_SOBEL ON)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES_HSOPTICALFLOW ON)
endif (CTRL_CMAKE_VERBOSE_EXAMPLES)

if (CTRL_EXPERIMENTATION_MODE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_EXP_MODE_ ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CTRL_EXAMPLES_EXP_MODE_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_EXP_MODE_ ")
endif (CTRL_EXPERIMENTATION_MODE)

if(SUPPORT_CUDA)
	if (CTRL_EXAMPLES_CUDA_ERROR_CHECK)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
		set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
	endif (CTRL_EXAMPLES_CUDA_ERROR_CHECK)
endif(SUPPORT_CUDA)

if(SUPPORT_OPENCL)
	if (CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK_ ")
		set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK_ ")
	endif (CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK)
endif(SUPPORT_OPENCL)

if(SUPPORT_HIP)
	if (CTRL_EXAMPLES_HIP_ERROR_CHECK)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_HIP_ERROR_CHECK_ ")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CTRL_EXAMPLES_HIP_ERROR_CHECK_ ")
	endif (CTRL_EXAMPLES_HIP_ERROR_CHECK)
endif(SUPPORT_HIP)

if(SUPPORT_FPGA)
	if (CTRL_EXAMPLES_FPGA_ERROR_CHECK)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_ERROR_CHECK_ ")
		set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_ERROR_CHECK_ ")
	endif (CTRL_EXAMPLES_FPGA_ERROR_CHECK)
endif(SUPPORT_FPGA)

set(CTRL_LIBS ${CTRL_LIBS} Ctrl)

if(SUPPORT_FPGA)
    #final '/' in the path must be kept in order for the compilation to work (strcat in Ctrl_FPGA_KernelProto.h)
    set(KERNELS_PATH ${PROJECT_SOURCE_DIR}/kernels)
    set(CTRL_KERNELS_PATH_FPGA ${PROJECT_SOURCE_DIR}/kernels/FPGA/Ctrl/)
    set(REF_KERNELS_PATH_FPGA ${PROJECT_SOURCE_DIR}/kernels/FPGA/Ref/)
endif(SUPPORT_FPGA)

if (PROFILING_ENABLED)
	if (SUPPORT_OPENCL OR SUPPORT_HIP)
		set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${ROCM_PATH}/include/roctracer)
		set(CTRL_LIBS ${CTRL_LIBS} ${ROCM_PATH}/lib/libroctx64.so)
	endif (SUPPORT_OPENCL OR SUPPORT_HIP)

	if (SUPPORT_CUDA)
		# NOTE: specific target for this was created in cmake 3.25
		set(CTRL_LIBS ${CTRL_LIBS} ${CUDAToolkit_LIBRARY_DIR}/libnvToolsExt.so)
	endif (SUPPORT_CUDA)

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_PROFILING_ENABLED_ ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_PROFILING_ENABLED_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_PROFILING_ENABLED_ ")
endif (PROFILING_ENABLED)

# Add examples
add_subdirectory(Mandelbrot_Animation)
add_subdirectory(Matrix_Add)
add_subdirectory(Matrix_Increment)
# add_subdirectory(Multi_Matrix_Add)
add_subdirectory(Matrix_Power_Of)
add_subdirectory(Hotspot)
add_subdirectory(Sobel_YUV)
add_subdirectory(CtrlBlas)
add_subdirectory(SubselecTest)
add_subdirectory(Chain_MatMult)
add_subdirectory(Texture_Test)
add_subdirectory(HSOpticalFlow)
