# <license>
# 
# Controller v2.1
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

# Common OpenCL options
if (SUPPORT_OPENCL_GPU)
	set(CTRL_EXAMPLES_OPENCL_GPU_PROFILING OFF) # Profiling with this
	set(CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK OFF) # Check errors with this
	set(CTRL_EXAMPLES_OPENCL_GPU_DEBUG OFF) # Debug whit this
	set(CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER OFF) # Complete ifo about events
	set(CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0 "16") # Localsize 0
	set(CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1 "16") # Localsize 1
	set(CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_2 "0") # Localsize 0
endif (SUPPORT_OPENCL_GPU)

if (SUPPORT_FPGA)
	set(CTRL_EXAMPLES_FPGA_PROFILING OFF) # Profiling with this
	set(CTRL_EXAMPLES_FPGA_ERROR_CHECK OFF) # Check errors with this
	set(CTRL_EXAMPLES_FPGA_DEBUG OFF) # Debug whit this
	set(CTRL_EXAMPLES_FPGA_TEST_MODE OFF) # Test results style with this
	set(CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER OFF) # Complete ifo about events
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0 "16") # Localsize 0
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1 "16") # Localsize 1
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2 "0") # Localsize 0
endif (SUPPORT_FPGA)

if (SUPPORT_CUDA)
	set(CTRL_EXAMPLES_CUDA_ERROR_CHECK OFF) # Check errors with this
	set(CTRL_EXAMPLES_CUDA_DEBUG OFF) # Debug whit this
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_0 "16") # Block Size 0
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_1 "16") # Block Size 1
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_2 "0") # Block Size 1
endif (SUPPORT_CUDA)

if (SUPPORT_CPU)
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_0 "16") # Block Size 0
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_1 "16") # Block Size 1
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_2 "0") # Block Size 1
endif (SUPPORT_CPU)

# Apply options
if (CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK)

if (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)

if (CTRL_EXAMPLES_OPENCL_GPU_DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_DEBUG_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_DEBUG_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_DEBUG)

if (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)

if (SUPPORT_OPENCL_GPU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_2} ")
endif(SUPPORT_OPENCL_GPU)

if (CTRL_EXAMPLES_FPGA_ERROR_CHECK)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_ERROR_CHECK_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_ERROR_CHECK_ ")
endif (CTRL_EXAMPLES_FPGA_ERROR_CHECK)

if (CTRL_EXAMPLES_FPGA_PROFILING)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_PROFILING_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_PROFILING_ ")
endif (CTRL_EXAMPLES_FPGA_PROFILING)

if (CTRL_EXAMPLES_FPGA_DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_DEBUG_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_DEBUG_ ")
endif (CTRL_EXAMPLES_FPGA_DEBUG)

if (CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER_ ")
endif (CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER)

if (SUPPORT_FPGA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
endif(SUPPORT_FPGA)

if (CTRL_EXAMPLES_CUDA_ERROR_CHECK)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
endif (CTRL_EXAMPLES_CUDA_ERROR_CHECK)

if (CTRL_EXAMPLES_CUDA_DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_DEBUG_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_DEBUG_ ")
endif (CTRL_EXAMPLES_CUDA_DEBUG)

if (SUPPORT_CUDA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_2} ")
	set(CUDA_PROPAGATE_HOST_FLAGS ON)
endif (SUPPORT_CUDA)

if (SUPPORT_CPU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CPU_BLOCKSIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CPU_BLOCKSIZE_2} ")
endif (SUPPORT_CPU)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_SOBEL)
	message(STATUS "SOBEL_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "SOBEL_LIBS = ${CTRL_LIBS}")
	message(STATUS "SOBEL_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "SOBEL_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif (CTRL_CMAKE_VERBOSE_EXAMPLES_SOBEL)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

if (SUPPORT_OPENCL_GPU)
	include_directories(${CTRL_INCLUDE_DIRS})

	add_executable(Sobel_YUV_OpenCL_Gpu_Ctrl_FTF src/Sobel_YUV_OpenCL_Gpu_Ctrl_FTF.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ctrl_MTF src/Sobel_YUV_OpenCL_Gpu_Ctrl_MTF.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ctrl_MTM src/Sobel_YUV_OpenCL_Gpu_Ctrl_MTM.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ctrl_FTM src/Sobel_YUV_OpenCL_Gpu_Ctrl_FTM.c)
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ctrl_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ctrl_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ctrl_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ctrl_FTM ${CTRL_LIBS})

	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Async_FTF src/Sobel_YUV_OpenCL_Gpu_Ref_Async_FTF.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Async_MTF src/Sobel_YUV_OpenCL_Gpu_Ref_Async_MTF.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Async_MTM src/Sobel_YUV_OpenCL_Gpu_Ref_Async_MTM.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Async_FTM src/Sobel_YUV_OpenCL_Gpu_Ref_Async_FTM.c)
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Async_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Async_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Async_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Async_FTM ${CTRL_LIBS})
	
	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Sync_FTF src/Sobel_YUV_OpenCL_Gpu_Ref_Sync_FTF.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Sync_MTF src/Sobel_YUV_OpenCL_Gpu_Ref_Sync_MTF.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Sync_MTM src/Sobel_YUV_OpenCL_Gpu_Ref_Sync_MTM.c)
	add_executable(Sobel_YUV_OpenCL_Gpu_Ref_Sync_FTM src/Sobel_YUV_OpenCL_Gpu_Ref_Sync_FTM.c)
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Sync_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Sync_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Sync_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_OpenCL_Gpu_Ref_Sync_FTM ${CTRL_LIBS})
endif (SUPPORT_OPENCL_GPU)

if (SUPPORT_FPGA)
	if(COMPILE_KERNELS)
		option(COMPILE_KERNEL_SOBEL "COMPILE_KERNEL_SOBEL" ON)
	else(COMPILE_KERNELS)
		option(COMPILE_KERNEL_SOBEL "COMPILE_KERNEL_SOBEL" OFF)
	endif(COMPILE_KERNELS)

	include_directories(${CTRL_INCLUDE_DIRS})

	execute_process(COMMAND ${CMAKE_C_COMPILER} ${KERNELS_PATH}/FPGA/parser_ctrlkernel.c -o ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	execute_process(COMMAND ${KERNELS_PATH}/FPGA/parser_ctrlkernel host ${CTRL_KERNELS_PATH_FPGA}/Sobel_Operation/Sobel_Operation_Ctrl.cl ${CTRL_KERNELS_PATH_FPGA}/Sobel_Operation/Sobel_Operation_Ctrl.c)

	execute_process(COMMAND rm ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	include_directories(src/ ${KERNELS_PATH})

	# OPENCL
	add_executable(Sobel_YUV_FPGA_Ctrl4 ${CTRL_KERNELS_PATH_FPGA}/Sobel_Operation/Sobel_Operation_Ctrl.c src/Sobel_YUV_FPGA_Ctrl.c)
	#add_executable(Sobel_YUV_FPGA_Ctrl4 src/sobel_ctrl_fix_from_cuda.c)
	target_link_libraries(Sobel_YUV_FPGA_Ctrl4 ${CTRL_LIBS})


	add_executable(Sobel_YUV_FPGA_Ref_Async src/Sobel_YUV_FPGA_Ref_Async.c)
	add_executable(Sobel_YUV_FPGA_Ref_Sync src/Sobel_YUV_FPGA_Ref_Sync.c)

	target_link_libraries(Sobel_YUV_FPGA_Ref_Async ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_FPGA_Ref_Sync ${CTRL_LIBS})
endif (SUPPORT_FPGA)

# Cuda
if (SUPPORT_CUDA)
	add_executable(Sobel_YUV_Cuda_Ctrl_FTF src/Sobel_YUV_Cuda_Ctrl_FTF.cu)
	add_executable(Sobel_YUV_Cuda_Ctrl_MTF src/Sobel_YUV_Cuda_Ctrl_MTF.cu)
	add_executable(Sobel_YUV_Cuda_Ctrl_MTM src/Sobel_YUV_Cuda_Ctrl_MTM.cu)
	add_executable(Sobel_YUV_Cuda_Ctrl_FTM src/Sobel_YUV_Cuda_Ctrl_FTM.cu)
	target_link_libraries(Sobel_YUV_Cuda_Ctrl_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ctrl_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ctrl_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ctrl_FTM ${CTRL_LIBS})

	add_executable(Sobel_YUV_Cuda_Ref_Async_FTF src/Sobel_YUV_Cuda_Ref_Async_FTF.cu)
	add_executable(Sobel_YUV_Cuda_Ref_Async_MTF src/Sobel_YUV_Cuda_Ref_Async_MTF.cu)
	add_executable(Sobel_YUV_Cuda_Ref_Async_MTM src/Sobel_YUV_Cuda_Ref_Async_MTM.cu)
	add_executable(Sobel_YUV_Cuda_Ref_Async_FTM src/Sobel_YUV_Cuda_Ref_Async_FTM.cu)
	target_link_libraries(Sobel_YUV_Cuda_Ref_Async_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ref_Async_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ref_Async_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ref_Async_FTM ${CTRL_LIBS})

	add_executable(Sobel_YUV_Cuda_Ref_Sync_FTF src/Sobel_YUV_Cuda_Ref_Sync_FTF.cu)
	add_executable(Sobel_YUV_Cuda_Ref_Sync_MTF src/Sobel_YUV_Cuda_Ref_Sync_MTF.cu)
	add_executable(Sobel_YUV_Cuda_Ref_Sync_MTM src/Sobel_YUV_Cuda_Ref_Sync_MTM.cu)
	add_executable(Sobel_YUV_Cuda_Ref_Sync_FTM src/Sobel_YUV_Cuda_Ref_Sync_FTM.cu)
	target_link_libraries(Sobel_YUV_Cuda_Ref_Sync_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ref_Sync_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ref_Sync_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cuda_Ref_Sync_FTM ${CTRL_LIBS})
endif (SUPPORT_CUDA)

# Cpu
if (SUPPORT_CPU)
	add_executable(Sobel_YUV_Cpu_Ctrl_FTF src/Sobel_YUV_Cpu_Ctrl_FTF.c)
	add_executable(Sobel_YUV_Cpu_Ctrl_MTF src/Sobel_YUV_Cpu_Ctrl_MTF.c)
	add_executable(Sobel_YUV_Cpu_Ctrl_MTM src/Sobel_YUV_Cpu_Ctrl_MTM.c)
	add_executable(Sobel_YUV_Cpu_Ctrl_FTM src/Sobel_YUV_Cpu_Ctrl_FTM.c)
	target_link_libraries(Sobel_YUV_Cpu_Ctrl_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cpu_Ctrl_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cpu_Ctrl_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cpu_Ctrl_FTM ${CTRL_LIBS})

	add_executable(Sobel_YUV_Cpu_Ref_Sync_FTF src/Sobel_YUV_Cpu_Ref_Sync_FTF.c)
	add_executable(Sobel_YUV_Cpu_Ref_Sync_MTF src/Sobel_YUV_Cpu_Ref_Sync_MTF.c)
	add_executable(Sobel_YUV_Cpu_Ref_Sync_MTM src/Sobel_YUV_Cpu_Ref_Sync_MTM.c)
	add_executable(Sobel_YUV_Cpu_Ref_Sync_FTM src/Sobel_YUV_Cpu_Ref_Sync_FTM.c)
	target_link_libraries(Sobel_YUV_Cpu_Ref_Sync_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cpu_Ref_Sync_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cpu_Ref_Sync_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Cpu_Ref_Sync_FTM ${CTRL_LIBS})
endif (SUPPORT_CPU)
