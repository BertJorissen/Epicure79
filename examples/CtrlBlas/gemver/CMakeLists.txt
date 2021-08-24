cmake_minimum_required (VERSION 3.17)

# Common OpenCL options
if (SUPPORT_CUDA)
	set(CTRL_EXAMPLES_CUDA_ERROR_CHECK OFF) # Check errors with this
	set(CTRL_EXAMPLES_CUDA_DEBUG OFF) # Debug whit this
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_0 "16") # Block Size 0
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_1 "16") # Block Size 1
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_2 "0") # Block Size 1
endif(SUPPORT_CUDA)

# Apply options
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
endif(SUPPORT_CUDA)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_GEMVER)
	message(STATUS "GEMVER_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "GEMVER_LIBS = ${CTRL_LIBS}")
	message(STATUS "GEMVER_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "GEMVER_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
	if (SUPPORT_CUDA)
		message(STATUS "GEMVER_CTRL_NVCC vars = ${CUDA_NVCC_FLAGS}")
	endif (SUPPORT_CUDA)
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_GEMVER)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# Cpu
if (SUPPORT_CPU AND MKL)
	add_executable(gemver_Blas_Cpu_Ctrl src/gemver_Blas_Cpu_Ctrl.c)
	target_link_libraries(gemver_Blas_Cpu_Ctrl ${CTRL_LIBS})

	add_executable(gemver_Blas_Cpu_Ref_MKL src/gemver_Blas_Cpu_Ref_MKL.c)
	target_link_libraries(gemver_Blas_Cpu_Ref_MKL ${CTRL_LIBS})
endif (SUPPORT_CPU AND MKL)

# Cuda
if (SUPPORT_CUDA)
	if (CUBLAS)
		add_executable(gemver_Blas_Cuda_Ref_Cublas src/gemver_Blas_Cuda_Ref_Cublas.c)
		target_link_libraries(gemver_Blas_Cuda_Ref_Cublas ${CTRL_LIBS})
	endif (CUBLAS)

	if(MAGMA)
		add_executable(gemver_Blas_Cuda_Ref_Magma src/gemver_Blas_Cuda_Ref_Magma.c)
		target_link_libraries(gemver_Blas_Cuda_Ref_Magma ${CTRL_LIBS})
	endif(MAGMA)
	
	if(CUBLAS OR MAGMA)
		add_executable(gemver_Blas_Cuda_Ctrl src/gemver_Blas_Cuda_Ctrl.c)
		target_link_libraries(gemver_Blas_Cuda_Ctrl ${CTRL_LIBS})
	endif(CUBLAS OR MAGMA)
endif(SUPPORT_CUDA)