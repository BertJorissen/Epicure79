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

# Common OpenCL options
if (SUPPORT_OPENCL)
	set(CTRL_EXAMPLES_OPENCL_GPU_PROFILING OFF) # Profiling with this
	set(CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER OFF) # Complete ifo about events
	set(CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0 "16") # Localsize x j
	set(CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1 "16") # Localsize y i
endif (SUPPORT_OPENCL)

if (SUPPORT_FPGA)
	set(CTRL_EXAMPLES_FPGA_PROFILING OFF) # Profiling with this
	set(CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER OFF) # Complete ifo about events
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0 "16") # Localsize x j
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1 "16") # Localsize y i
endif (SUPPORT_FPGA)

if (SUPPORT_CUDA)
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_0 "16") # Block Size x j
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_1 "16") # Block Size y i
endif (SUPPORT_CUDA)

if (SUPPORT_HIP)
	set(CTRL_EXAMPLES_HIP_BLOCKSIZE_0 "16") # Block Size x j
	set(CTRL_EXAMPLES_HIP_BLOCKSIZE_1 "16") # Block Size y i
endif (SUPPORT_HIP)

if (SUPPORT_CPU)
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_0 "16") # Block Size x j
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_1 "16") # Block Size y i
endif (SUPPORT_CPU)

# Apply options
if (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)

if (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)

if (SUPPORT_OPENCL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1} ")
endif(SUPPORT_OPENCL)

if (CTRL_EXAMPLES_FPGA_PROFILING)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_PROFILING_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_PROFILING_ ")
endif (CTRL_EXAMPLES_FPGA_PROFILING)

if (CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER_ ")
endif (CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER)

if (SUPPORT_FPGA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
endif(SUPPORT_FPGA)

if (SUPPORT_CUDA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} ")
endif(SUPPORT_CUDA)

if (SUPPORT_HIP)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_HIP_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_HIP_BLOCKSIZE_1} ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_HIP_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_HIP_BLOCKSIZE_1} ")
endif(SUPPORT_HIP)

if (SUPPORT_CPU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} ")
endif(SUPPORT_CPU)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_HOTSPOT)
	message(STATUS "HOTSPOT_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "HOTSPOT_LIBS = ${CTRL_LIBS}")
	message(STATUS "HOTSPOT_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "HOTSPOT_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_HOTSPOT)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# OpenCL
if (SUPPORT_OPENCL)
	add_executable(Hotspot_OpenCL_Gpu_Ctrl src/Hotspot_OpenCL_Gpu_Ctrl.c)
	target_link_libraries(Hotspot_OpenCL_Gpu_Ctrl ${CTRL_LIBS})

	add_executable(Hotspot_OpenCL_Gpu_Ref_Async src/Hotspot_OpenCL_Gpu_Ref_Async.c)
	target_link_libraries(Hotspot_OpenCL_Gpu_Ref_Async ${CTRL_LIBS})
	
	add_executable(Hotspot_OpenCL_Gpu_Ref_Sync src/Hotspot_OpenCL_Gpu_Ref_Sync.c)
	target_link_libraries(Hotspot_OpenCL_Gpu_Ref_Sync ${CTRL_LIBS})
endif (SUPPORT_OPENCL)

#FPGA
if (SUPPORT_FPGA)
	if(COMPILE_KERNELS)
		option(COMPILE_KERNEL_HOTSPOT "COMPILE_KERNEL_HOTSPOT" ON)
	else(COMPILE_KERNELS)
		option(COMPILE_KERNEL_HOTSPOT "COMPILE_KERNEL_HOTSPOT" OFF)
	endif(COMPILE_KERNELS)
	
	execute_process(COMMAND ${CMAKE_C_COMPILER} ${KERNELS_PATH}/FPGA/parser_ctrlkernel.c -o ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	execute_process(COMMAND ${KERNELS_PATH}/FPGA/parser_ctrlkernel host ${CTRL_KERNELS_PATH_FPGA}/Hotspot/Hotspot_Ctrl.cl ${CTRL_KERNELS_PATH_FPGA}/Hotspot/Hotspot_Ctrl.c)

	execute_process(COMMAND rm ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	include_directories(src/ ${KERNELS_PATH})
	add_executable(Hotspot_FPGA_Ctrl ${CTRL_KERNELS_PATH_FPGA}/Hotspot/Hotspot_Ctrl.c src/Hotspot_FPGA_Ctrl.c)
	target_link_libraries(Hotspot_FPGA_Ctrl ${CTRL_LIBS})
	

	add_executable(Hotspot_FPGA_Ref_Async src/Hotspot_FPGA_Ref_Async.c)
	add_executable(Hotspot_FPGA_Ref_Sync src/Hotspot_FPGA_Ref_Sync.c)

	target_link_libraries(Hotspot_FPGA_Ref_Async ${CTRL_LIBS})
	target_link_libraries(Hotspot_FPGA_Ref_Sync ${CTRL_LIBS})
endif (SUPPORT_FPGA)

# Cuda
if (SUPPORT_CUDA)
	add_executable(Hotspot_Cuda_Ctrl src/Hotspot_Cuda_Ctrl.cu)
	target_link_libraries(Hotspot_Cuda_Ctrl ${CTRL_LIBS})

	add_executable(Hotspot_Cuda_Ref_Async src/Hotspot_Cuda_Ref_Async.cu)
	target_link_libraries(Hotspot_Cuda_Ref_Async ${CTRL_LIBS})

	add_executable(Hotspot_Cuda_Ref_Sync src/Hotspot_Cuda_Ref_Sync.cu)
	target_link_libraries(Hotspot_Cuda_Ref_Sync ${CTRL_LIBS})

endif (SUPPORT_CUDA)

# Hip
if (SUPPORT_HIP)
	# Delete de parameter -std=c99 needed for Controllers compilation
	string(REPLACE "-std=c99" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --offload-arch=gfx900")
	
	add_executable(Hotspot_Hip_Ctrl src/Hotspot_Hip_Ctrl.cpp)
	target_link_libraries(Hotspot_Hip_Ctrl ${CTRL_LIBS})

	add_executable(Hotspot_Hip_Ref_Async src/Hotspot_Hip_Ref_Async.cpp)
	target_link_libraries(Hotspot_Hip_Ref_Async ${CTRL_LIBS})

	add_executable(Hotspot_Hip_Ref_Sync src/Hotspot_Hip_Ref_Sync.cpp)
	target_link_libraries(Hotspot_Hip_Ref_Sync ${CTRL_LIBS})

endif (SUPPORT_HIP)

if(SUPPORT_CPU)
	add_executable(Hotspot_Cpu_Ctrl src/Hotspot_Cpu_Ctrl.c)
	target_link_libraries(Hotspot_Cpu_Ctrl ${CTRL_LIBS})

	add_executable(Hotspot_Cpu_Ref_Sync src/Hotspot_Cpu_Ref_Sync.c)
	target_link_libraries(Hotspot_Cpu_Ref_Sync ${CTRL_LIBS})
endif (SUPPORT_CPU)
