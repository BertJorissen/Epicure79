cmake_minimum_required (VERSION 3.17)

set(CTRL_EXAMPLES_BLOCKSIZE_0 "16") # Block Size 0
set(CTRL_EXAMPLES_BLOCKSIZE_1 "16") # Block Size 1
set(CTRL_EXAMPLES_BLOCKSIZE_2 "0") # Block Size 1

# Apply options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_BLOCKSIZE_2} ")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_BLOCKSIZE_2} ")

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_INCREMENT)
	message(STATUS "MATRIX_INCREMENT_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "MATRIX_INCREMENT_LIBS = ${CTRL_LIBS}")
	message(STATUS "MATRIX_INCREMENT_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "MATRIX_INCREMENT_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_INCREMENT)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# cpu arch is needed for the norm calc
if(SUPPORT_CPU)
	# Compilation dependences
	# Main code
	set(CTRL_SOURCE_FILES src/Matrix_Increment_Multidev_Ctrl.c)
	# kernels
	if (SUPPORT_CUDA)
		set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/Matrix_Increment_Ctrl_Kernels.cu)
	else (SUPPORT_CUDA)
		set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/Matrix_Increment_Ctrl_Kernels.c)
	endif(SUPPORT_CUDA)

	add_executable(Matrix_Increment_Multidev_Ctrl ${CTRL_SOURCE_FILES})
	target_link_libraries(Matrix_Increment_Multidev_Ctrl ${CTRL_LIBS})
endif(SUPPORT_CPU)
	
if(SUPPORT_CUDA)
	add_executable(Matrix_Increment_Cuda_Ref_Async src/Matrix_Increment_Cuda_Ref_Async.cu)
	target_link_libraries(Matrix_Increment_Cuda_Ref_Async ${CTRL_LIBS})
endif(SUPPORT_CUDA)
