cmake_minimum_required (VERSION 3.17)

# Common CUDA options
if (SUPPORT_CUDA)
	set(CTRL_EXAMPLES_CUDA_ERROR_CHECK OFF) # Check errors with this
	set(CTRL_EXAMPLES_CUDA_DEBUG OFF) # Debug whit this
	set(CUDA_PROPAGATE_HOST_FLAGS ON)
endif(SUPPORT_CUDA)

# Apply options
if (CTRL_EXAMPLES_CUDA_ERROR_CHECK)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
endif (CTRL_EXAMPLES_CUDA_ERROR_CHECK)

if (CTRL_EXAMPLES_CUDA_DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_DEBUG_")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_DEBUG_")
endif (CTRL_EXAMPLES_CUDA_DEBUG)
	
# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES)
	message(STATUS "PARALLEL_STENCIL_SKELETON_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "PARALLEL_STENCIL_SKELETON_LIBS = ${CTRL_LIBS}")
	message(STATUS "PARALLEL_STENCIL_SKELETON_CTRL_C vars = ${CMAKE_C_FLAGS}")
	if (SUPPORT_CUDA)
		message(STATUS "PARALLEL_STENCIL_SKELETON_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
	endif (SUPPORT_CUDA)
endif(CTRL_CMAKE_VERBOSE_EXAMPLES)
	
# Include directories
include_directories(${CTRL_INCLUDE_DIRS})
	
set(CTRL_LIBS ${CTRL_LIBS} Ctrl)
	
# Compile for any architecture (GENERIC)
if (SUPPORT_CUDA)
	set(PSS_KERNEL_SOURCE "src/epsilodKernels.cu")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -gencode arch=compute_35,code=sm_35 -gencode arch=compute_52,code=sm_52 -gencode arch=compute_70,code=sm_70")
else (SUPPORT_CUDA)
	set(PSS_KERNEL_SOURCE "src/epsilodKernels.c")
endif(SUPPORT_CUDA)

add_executable(epsilod ${PSS_KERNEL_SOURCE} src/epsilod.c)
target_link_libraries(epsilod ${CTRL_LIBS})
