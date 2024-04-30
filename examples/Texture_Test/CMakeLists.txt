cmake_minimum_required (VERSION 3.17)

set(CTRL_EXAMPLES_BLOCKSIZE_0 "16") # Block Size 0
set(CTRL_EXAMPLES_BLOCKSIZE_1 "16") # Block Size 1
set(CTRL_EXAMPLES_BLOCKSIZE_2 "0") # Block Size 1

# Apply options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_BLOCKSIZE_2} ")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_BLOCKSIZE_2} ")

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_TEXTURE_TEST)
	message(STATUS "TEXTURE_TEST_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "TEXTURE_TEST_LIBS = ${CTRL_LIBS}")
	message(STATUS "TEXTURE_TEST_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "TEXTURE_TEST_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_TEXTURE_TEST)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# Compilation dependences
# Main code
set(CTRL_SOURCE_FILES src/Texture_Test_Ctrl.c)
# Generic kernel/s
if (SUPPORT_CUDA)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/Texture_Test_Ctrl_Kernels.cu)
elseif(SUPPORT_HIP)	
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/Texture_Test_Ctrl_Kernels.cpp)
else (SUPPORT_CUDA)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/Texture_Test_Ctrl_Kernels.c)
endif(SUPPORT_CUDA)

if(SUPPORT_CUDA)
	add_executable(Texture_Test_Cuda_Ref src/Texture_Test_Cuda_Ref.cu)
	target_link_libraries(Texture_Test_Cuda_Ref ${CTRL_LIBS})
endif(SUPPORT_CUDA)

if(SUPPORT_OPENCL)
	add_executable(Texture_Test_OpenCL_GPU_Ref src/Texture_Test_OpenCL_GPU_Ref.c)
	target_link_libraries(Texture_Test_OpenCL_GPU_Ref ${CTRL_LIBS})
endif(SUPPORT_OPENCL)

add_executable(Texture_Test_Ctrl ${CTRL_SOURCE_FILES})
target_link_libraries(Texture_Test_Ctrl ${CTRL_LIBS})