cmake_minimum_required (VERSION 3.20)

# Common OpenCL options
if (SUPPORT_OPENCL)
	set(CTRL_EXAMPLES_OPENCL_GPU_PROFILING OFF) # Profiling with this
	set(CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER OFF) # Complete ifo about events
	set(CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE "16") # Localsize
endif (SUPPORT_OPENCL)

if (SUPPORT_FPGA)
	set(CTRL_EXAMPLES_FPGA_PROFILING OFF) # Profiling with this
	set(CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER OFF) # Complete ifo about events
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0 "16") # Localsize 0
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1 "16") # Localsize 1
	set(CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2 "0") # Localsize 0
endif (SUPPORT_FPGA)

if (SUPPORT_CUDA)
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE "16") # Block Size
endif(SUPPORT_CUDA)

if (SUPPORT_HIP)
	set(CTRL_EXAMPLES_HIP_BLOCKSIZE "16") # Block Size
endif(SUPPORT_HIP)

if (SUPPORT_CPU)
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE "16") # Block Size
endif(SUPPORT_CPU)

# Apply options
if (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)

if (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)

if (CTRL_EXAMPLES_FPGA_PROFILING)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_PROFILING_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_PROFILING_ ")
endif (CTRL_EXAMPLES_FPGA_PROFILING)

if (CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER_ ")
endif (CTRL_EXAMPLES_FPGA_GENERATE_VISUAL_PROFILER)

if (SUPPORT_OPENCL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DLOCAL_SIZE=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE} ")
endif(SUPPORT_OPENCL)

if (SUPPORT_FPGA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2} -DKERNELTYPE=NDRANGE -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2} -DKERNELTYPE=NDRANGE -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
endif(SUPPORT_FPGA)

if (SUPPORT_CUDA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_CUDA_BLOCKSIZE} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_CUDA_BLOCKSIZE} ")
endif(SUPPORT_CUDA)

if (SUPPORT_HIP)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_HIP_BLOCKSIZE} ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_HIP_BLOCKSIZE} ")
endif(SUPPORT_HIP)

if (SUPPORT_CPU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_CPU_BLOCKSIZE} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_CPU_BLOCKSIZE} ")
endif(SUPPORT_CPU)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_POWER_OF)
	message(STATUS "MATRIX_POWER_OF_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "MATRIX_POWER_OF_LIBS = ${CTRL_LIBS}")
	message(STATUS "MATRIX_POWER_OF_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "MATRIX_POWER_OF_CTRL_CXX vars = ${CMAKE_CXX_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_POWER_OF)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# Cuda
if (SUPPORT_CUDA)
	add_executable(Matrix_Power_Of_Cuda_Ctrl src/Matrix_Power_Of_Cuda_Ctrl.cu)
	target_link_libraries(Matrix_Power_Of_Cuda_Ctrl ${CTRL_LIBS})

	add_executable(Matrix_Power_Of_Cuda_Ref_Async src/Matrix_Power_Of_Cuda_Ref_Async.cu)
	target_link_libraries(Matrix_Power_Of_Cuda_Ref_Async ${CTRL_LIBS})
	
	add_executable(Matrix_Power_Of_Cuda_Ref_Sync src/Matrix_Power_Of_Cuda_Ref_Sync.cu)
	target_link_libraries(Matrix_Power_Of_Cuda_Ref_Sync ${CTRL_LIBS})
endif(SUPPORT_CUDA)

# Hip
if (SUPPORT_HIP)
	add_executable(Matrix_Power_Of_Hip_Ctrl src/Matrix_Power_Of_Hip_Ctrl.cpp)
	target_link_libraries(Matrix_Power_Of_Hip_Ctrl ${CTRL_LIBS})

	add_executable(Matrix_Power_Of_Hip_Ref_Async src/Matrix_Power_Of_Hip_Ref_Async.cpp)
	target_link_libraries(Matrix_Power_Of_Hip_Ref_Async ${CTRL_LIBS})
	
	add_executable(Matrix_Power_Of_Hip_Ref_Sync src/Matrix_Power_Of_Hip_Ref_Sync.cpp)
	target_link_libraries(Matrix_Power_Of_Hip_Ref_Sync ${CTRL_LIBS})
endif(SUPPORT_HIP)

#FPGA
if (SUPPORT_FPGA)
	if(COMPILE_KERNELS)
		option(COMPILE_KERNEL_MULT "COMPILE_KERNEL_MULT" ON)
	else(COMPILE_KERNELS)
		option(COMPILE_KERNEL_MULT "COMPILE_KERNEL_MULT" OFF)
	endif(COMPILE_KERNELS)
	
	execute_process(COMMAND ${CMAKE_C_COMPILER} ${KERNELS_PATH}/FPGA/parser_ctrlkernel.c -o ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	execute_process(COMMAND ${KERNELS_PATH}/FPGA/parser_ctrlkernel host ${CTRL_KERNELS_PATH_FPGA}/Mult/Mult_Ctrl.cl ${CTRL_KERNELS_PATH_FPGA}/Mult/Mult_Ctrl.c)

	execute_process(COMMAND rm ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	include_directories(src/ ${KERNELS_PATH})
	add_executable(Matrix_Pow_FPGA_Ctrl ${CTRL_KERNELS_PATH_FPGA}/Mult/Mult_Ctrl.c src/Matrix_Power_Of_FPGA_Ctrl.c)
	target_link_libraries(Matrix_Pow_FPGA_Ctrl ${CTRL_LIBS})

	add_executable(Matrix_Pow_FPGA_Ref_Async src/Matrix_Power_Of_FPGA_Ref_Async.c)
	add_executable(Matrix_Pow_FPGA_Ref_Sync src/Matrix_Power_Of_FPGA_Ref_Sync.c)
	

	target_link_libraries(Matrix_Pow_FPGA_Ref_Async ${CTRL_LIBS})
	target_link_libraries(Matrix_Pow_FPGA_Ref_Sync ${CTRL_LIBS})
endif (SUPPORT_FPGA)

# OpenCL
if (SUPPORT_OPENCL)
	add_executable(Matrix_Power_Of_OpenCL_Gpu_Ctrl src/Matrix_Power_Of_OpenCL_Gpu_Ctrl.c)
	target_link_libraries(Matrix_Power_Of_OpenCL_Gpu_Ctrl ${CTRL_LIBS})

	add_executable(Matrix_Power_Of_OpenCL_Gpu_Ref_Async src/Matrix_Power_Of_OpenCL_Gpu_Ref_Async.c)
	target_link_libraries(Matrix_Power_Of_OpenCL_Gpu_Ref_Async ${CTRL_LIBS})

	add_executable(Matrix_Power_Of_OpenCL_Gpu_Ref_Sync src/Matrix_Power_Of_OpenCL_Gpu_Ref_Sync.c)
	target_link_libraries(Matrix_Power_Of_OpenCL_Gpu_Ref_Sync ${CTRL_LIBS})
endif (SUPPORT_OPENCL)

# Cpu
if (SUPPORT_CPU)
	add_executable(Matrix_Power_Of_Cpu_Ctrl src/Matrix_Power_Of_Cpu_Ctrl.c)
	target_link_libraries(Matrix_Power_Of_Cpu_Ctrl ${CTRL_LIBS})

	add_executable(Matrix_Power_Of_Cpu_Ref_Sync src/Matrix_Power_Of_Cpu_Ref_Sync.c)
	target_link_libraries(Matrix_Power_Of_Cpu_Ref_Sync ${CTRL_LIBS})
endif (SUPPORT_CPU)
