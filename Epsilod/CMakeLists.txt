cmake_minimum_required (VERSION 3.17)

# Apply options
if (CTRL_EXAMPLES_CUDA_ERROR_CHECK)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
endif (CTRL_EXAMPLES_CUDA_ERROR_CHECK)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES)
	message(STATUS "PARALLEL_STENCIL_SKELETON_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "PARALLEL_STENCIL_SKELETON_LIBS = ${CTRL_LIBS}")
	message(STATUS "PARALLEL_STENCIL_SKELETON_CTRL_C vars = ${CMAKE_C_FLAGS}")
	if (SUPPORT_CUDA)
		message(STATUS "PARALLEL_STENCIL_SKELETON_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
	endif (SUPPORT_CUDA)
endif(CTRL_CMAKE_VERBOSE_EXAMPLES)
	
# Include directories and libs
include_directories(${CTRL_INCLUDE_DIRS} src/)
set(CTRL_LIBS ${CTRL_LIBS} Ctrl)

# Compile kernels with nvcc if CUDA is active. If not, compile them with the C compiler
if (SUPPORT_CUDA)
	set(EPSILOD_KERNELS "src/epsilod_kernels.cu")
	set(TEST_KERNEL_SOURCE "src/test_parallelStencilSkeleton_kernels.cu")
else (SUPPORT_CUDA)
	set(EPSILOD_KERNELS "src/epsilod_kernels.c")
	set(TEST_KERNEL_SOURCE "src/test_parallelStencilSkeleton_kernels.c")
endif(SUPPORT_CUDA)

add_executable(test_parallelStencilSkeleton ${EPSILOD_KERNELS} ${TEST_KERNEL_SOURCE} src/test_parallelStencilSkeleton.c src/epsilod.c)
target_link_libraries(test_parallelStencilSkeleton ${CTRL_LIBS})

