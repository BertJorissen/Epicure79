cmake_minimum_required (VERSION 3.17)

# Common options
set(CTRL_EXAMPLES_BLOCKSIZE_0 "16") # Localsize/Blocksize x j
set(CTRL_EXAMPLES_BLOCKSIZE_1 "16") # Localsize/Blocksize y i
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_BLOCKSIZE_1} ")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_BLOCKSIZE_1} ")

# OpenCL options
if (SUPPORT_OPENCL)
	set(CTRL_EXAMPLES_OPENCL_GPU_PROFILING OFF) # Profiling with this
	set(CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER OFF) # Complete ifo about events
endif (SUPPORT_OPENCL)

# Apply options
if (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_PROFILING_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_PROFILING)

if (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER_ ")
endif (CTRL_EXAMPLES_OPENCL_GPU_GENERATE_VISUAL_PROFILER)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_ADD)
	message(STATUS "MATRIX_ADD_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "MATRIX_ADD_LIBS = ${CTRL_LIBS}")
	message(STATUS "MATRIX_ADD_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "MATRIX_ADD_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_ADD)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# Compilation dependences
# Main code
set(CTRL_SOURCE_FILES src/Matrix_Add_Ctrl.c)
# Generic kernel/s
if (SUPPORT_CUDA)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/kernels_generic_cuda.cu)
elseif(SUPPORT_HIP)	
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/kernels_generic_hip.cpp)
else (SUPPORT_CUDA)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/kernels_generic.c)
endif(SUPPORT_CUDA)

# Cpu
if (SUPPORT_CPU)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/kernels_cpulib.c)
	add_executable(Matrix_Add_Cpu_Ref src/Matrix_Add_Cpu_Ref.c)
	target_link_libraries(Matrix_Add_Cpu_Ref ${CTRL_LIBS})
endif (SUPPORT_CPU)

# Cuda
if (SUPPORT_CUDA)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/kernels_cudalib.c)
	add_executable(Matrix_Add_Cuda_Ref src/Matrix_Add_Cuda_Ref.cu)
	target_link_libraries(Matrix_Add_Cuda_Ref ${CTRL_LIBS})
endif(SUPPORT_CUDA)

# OpenCL
if (SUPPORT_OPENCL)
	add_executable(Matrix_Add_OpenCL_Gpu_Ref src/Matrix_Add_OpenCL_Gpu_Ref.c)
	target_link_libraries(Matrix_Add_OpenCL_Gpu_Ref ${CTRL_LIBS})
endif (SUPPORT_OPENCL)


# Executables: With lib kernels
add_executable(Matrix_Add_Ctrl_KLibs ${CTRL_SOURCE_FILES})
target_link_libraries(Matrix_Add_Ctrl_KLibs ${CTRL_LIBS})

# Executables: Only generic kernel
add_executable(Matrix_Add_Ctrl_KGeneric ${CTRL_SOURCE_FILES})
set_target_properties(Matrix_Add_Ctrl_KGeneric PROPERTIES COMPILE_FLAGS -D_CTRL_MATRIX_ADD_GENERIC_KERNEL_) 
target_link_libraries(Matrix_Add_Ctrl_KGeneric ${CTRL_LIBS})
