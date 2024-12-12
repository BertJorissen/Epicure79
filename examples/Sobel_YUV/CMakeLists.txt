cmake_minimum_required (VERSION 3.20)

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
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_OPENCL_GPU_LOCAL_SIZE_2} ")
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
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOCAL_SIZE_0=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_0} -DLOCAL_SIZE_1=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_1} -DLOCAL_SIZE_2=${CTRL_EXAMPLES_FPGA_LOCAL_SIZE_2} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
endif(SUPPORT_FPGA)

if (SUPPORT_CUDA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_2} ")
endif (SUPPORT_CUDA)

if (SUPPORT_HIP)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_HIP_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_HIP_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_HIP_BLOCKSIZE_2} ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_HIP_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_HIP_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_HIP_BLOCKSIZE_2} ")
endif (SUPPORT_HIP)

if (SUPPORT_CPU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CPU_BLOCKSIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CPU_BLOCKSIZE_2} ")
endif (SUPPORT_CPU)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_SOBEL)
	message(STATUS "SOBEL_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "SOBEL_LIBS = ${CTRL_LIBS}")
	message(STATUS "SOBEL_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "SOBEL_CTRL_CXX vars = ${CMAKE_CXX_FLAGS}")
	message(STATUS "SOBEL_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif (CTRL_CMAKE_VERBOSE_EXAMPLES_SOBEL)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

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

# HIP
if (SUPPORT_HIP)
	add_executable(Sobel_YUV_Hip_Ctrl_FTF src/Sobel_YUV_Hip_Ctrl_FTF.cpp)
	add_executable(Sobel_YUV_Hip_Ctrl_MTF src/Sobel_YUV_Hip_Ctrl_MTF.cpp)
	add_executable(Sobel_YUV_Hip_Ctrl_MTM src/Sobel_YUV_Hip_Ctrl_MTM.cpp)
	add_executable(Sobel_YUV_Hip_Ctrl_FTM src/Sobel_YUV_Hip_Ctrl_FTM.cpp)
	target_link_libraries(Sobel_YUV_Hip_Ctrl_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ctrl_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ctrl_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ctrl_FTM ${CTRL_LIBS})

	add_executable(Sobel_YUV_Hip_Ref_Async_FTF src/Sobel_YUV_Hip_Ref_Async_FTF.cpp)
	add_executable(Sobel_YUV_Hip_Ref_Async_MTF src/Sobel_YUV_Hip_Ref_Async_MTF.cpp)
	add_executable(Sobel_YUV_Hip_Ref_Async_MTM src/Sobel_YUV_Hip_Ref_Async_MTM.cpp)
	add_executable(Sobel_YUV_Hip_Ref_Async_FTM src/Sobel_YUV_Hip_Ref_Async_FTM.cpp)
	target_link_libraries(Sobel_YUV_Hip_Ref_Async_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ref_Async_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ref_Async_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ref_Async_FTM ${CTRL_LIBS})

	add_executable(Sobel_YUV_Hip_Ref_Sync_FTF src/Sobel_YUV_Hip_Ref_Sync_FTF.cpp)
	add_executable(Sobel_YUV_Hip_Ref_Sync_MTF src/Sobel_YUV_Hip_Ref_Sync_MTF.cpp)
	add_executable(Sobel_YUV_Hip_Ref_Sync_MTM src/Sobel_YUV_Hip_Ref_Sync_MTM.cpp)
	add_executable(Sobel_YUV_Hip_Ref_Sync_FTM src/Sobel_YUV_Hip_Ref_Sync_FTM.cpp)
	target_link_libraries(Sobel_YUV_Hip_Ref_Sync_FTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ref_Sync_MTF ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ref_Sync_MTM ${CTRL_LIBS})
	target_link_libraries(Sobel_YUV_Hip_Ref_Sync_FTM ${CTRL_LIBS})
endif (SUPPORT_HIP)

if (SUPPORT_OPENCL)
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
endif (SUPPORT_OPENCL)

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
