cmake_minimum_required (VERSION 3.17)

set(CTRL_EXAMPLES_BLOCKSIZE "16") # Block Size

# Apply options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_BLOCKSIZE} ")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE=${CTRL_EXAMPLES_BLOCKSIZE} ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_2BUF ")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_2BUF ")

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_CHAIN_MATMUL)
	message(STATUS "CHAIN_MATMUL_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "CHAIN_MATMUL_LIBS = ${CTRL_LIBS}")
	message(STATUS "CHAIN_MATMUL_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "CHAIN_MATMUL_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_CHAIN_MATMUL)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# cpu arch is needed for the norm calc
if(SUPPORT_CPU)
	# Compilation dependences
	# Main code
	set(CTRL_SOURCE_FILES src/Chain_MatMult_Multidev_Ctrl.c)
	# kernels
	if (SUPPORT_CUDA)
		set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/Chain_MatMult_Ctrl_Kernels.cu)
	else (SUPPORT_CUDA)
		set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/Chain_MatMult_Ctrl_Kernels.c)
	endif(SUPPORT_CUDA)

	add_executable(Chain_MatMult_Multidev_Ctrl ${CTRL_SOURCE_FILES})
	target_link_libraries(Chain_MatMult_Multidev_Ctrl ${CTRL_LIBS})
endif(SUPPORT_CPU)
	
if(SUPPORT_CUDA)
	add_executable(Chain_MatMult_Cuda_Ref_Async src/Chain_MatMult_Cuda_Ref_Async.cu)
	target_link_libraries(Chain_MatMult_Cuda_Ref_Async ${CTRL_LIBS})
endif(SUPPORT_CUDA)
	
if(SUPPORT_OPENCL)
	add_executable(Chain_MatMult_OpenCL_Gpu_Ref_Async src/Chain_MatMult_OpenCL_Gpu_Ref_Async.c)
	target_link_libraries(Chain_MatMult_OpenCL_Gpu_Ref_Async ${CTRL_LIBS})
endif(SUPPORT_OPENCL)
