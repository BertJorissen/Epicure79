cmake_minimum_required (VERSION 3.20)

set(SOLVE_CHAR "-DSOLVECHAR_0=32 -DSOLVECHAR_1=6 ")
set(WARP_CHAR "-DWARPCHAR_0=32 -DWARPCHAR_1=6 ")
set(DERIV_CHAR "-DDERIVCHAR_0=32 -DDERIVCHAR_1=6 ")
set(SCALE_CHAR "-DSCALECHAR_0=32 -DSCALECHAR_1=8 ")
set(ADD_CHAR "-DADDCHAR=256 ")

# Apply options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}   ${SOLVE_CHAR} ${WARP_CHAR} ${DERIV_CHAR} ${SCALE_CHAR} ${ADD_CHAR} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${SOLVE_CHAR} ${WARP_CHAR} ${DERIV_CHAR} ${SCALE_CHAR} ${ADD_CHAR} ")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS}  ${SOLVE_CHAR} ${WARP_CHAR} ${DERIV_CHAR} ${SCALE_CHAR} ${ADD_CHAR} ")

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_HSOPTICALFLOW)
	message(STATUS "HSOPTICALFLOW_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "HSOPTICALFLOW_LIBS = ${CTRL_LIBS}")
	message(STATUS "HSOPTICALFLOW_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "HSOPTICALFLOW_CTRL_CXX vars = ${CMAKE_CXX_FLAGS}")
	message(STATUS "HSOPTICALFLOW_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_HSOPTICALFLOW)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# Compilation dependences
# Main code
# set(CTRL_SOURCE_FILES src/HSOpticalFlow_Ctrl.c)
# Kernels
if (SUPPORT_CUDA)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/addKernel.cu)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/derivativesKernel.cu)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/downscaleKernel.cu)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/solverKernel.cu)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/upscaleKernel.cu)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/warpingKernel.cu)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/zeroKernel.cu)
elseif(SUPPORT_HIP)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/addKernel.cpp)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/derivativesKernel.cpp)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/downscaleKernel.cpp)
set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/solverKernel.cpp)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/upscaleKernel.cpp)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/warpingKernel.cpp)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/zeroKernel.cpp)
else (SUPPORT_CUDA)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/addKernel.c)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/derivativesKernel.c)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/downscaleKernel.c)
set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/solverKernel.c)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/upscaleKernel.c)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/warpingKernel.c)
	set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/zeroKernel.c)
endif(SUPPORT_CUDA)

set(CTRL_SOURCE_FILES ${CTRL_SOURCE_FILES} src/ctrl_kernels/normKernel.c)

add_executable(HSOpticalFlow_Ctrl src/HSOpticalFlow_Ctrl.c ${CTRL_SOURCE_FILES})
target_link_libraries(HSOpticalFlow_Ctrl ${CTRL_LIBS})

add_executable(HSOpticalFlow_Ctrl_nopin src/HSOpticalFlow_Ctrl_nopin.c ${CTRL_SOURCE_FILES})
target_link_libraries(HSOpticalFlow_Ctrl_nopin ${CTRL_LIBS})

add_executable(HSOpticalFlow_Ctrl_Multi src/HSOpticalFlow_Ctrl_Multi.c ${CTRL_SOURCE_FILES})
target_link_libraries(HSOpticalFlow_Ctrl_Multi ${CTRL_LIBS})
	
if(SUPPORT_CPU)
	add_executable(HSOpticalFlow_Cpu_Ref src/HSOpticalFlow_Cpu_Ref.c)
	target_link_libraries(HSOpticalFlow_Cpu_Ref ${CTRL_LIBS})
	add_executable(HSOpticalFlow_Cpu_Ref_Multi src/HSOpticalFlow_Cpu_Ref_Multi.c)
	target_link_libraries(HSOpticalFlow_Cpu_Ref_Multi ${CTRL_LIBS})
endif(SUPPORT_CPU)

if(SUPPORT_CUDA)
	add_executable(HSOpticalFlow_Cuda_Ref src/HSOpticalFlow_Cuda_Ref.cu)
	target_link_libraries(HSOpticalFlow_Cuda_Ref ${CTRL_LIBS})
	add_executable(HSOpticalFlow_Cuda_Ref_pin src/HSOpticalFlow_Cuda_Ref_pin.cu)
	target_link_libraries(HSOpticalFlow_Cuda_Ref_pin ${CTRL_LIBS})
	add_executable(HSOpticalFlow_Cuda_Ref_Multi_nopin src/HSOpticalFlow_Cuda_Ref_Multi_nopin.cu)
	target_link_libraries(HSOpticalFlow_Cuda_Ref_Multi_nopin ${CTRL_LIBS})
	add_executable(HSOpticalFlow_Cuda_Ref_Multi src/HSOpticalFlow_Cuda_Ref_Multi.cu)
	target_link_libraries(HSOpticalFlow_Cuda_Ref_Multi ${CTRL_LIBS})
endif(SUPPORT_CUDA)

if(SUPPORT_HIP)
	add_executable(HSOpticalFlow_Hip_Ref src/HSOpticalFlow_Hip_Ref.cpp)
	target_link_libraries(HSOpticalFlow_Hip_Ref ${CTRL_LIBS})
	add_executable(HSOpticalFlow_Hip_Ref_pin src/HSOpticalFlow_Hip_Ref_pin.cpp)
	target_link_libraries(HSOpticalFlow_Hip_Ref_pin ${CTRL_LIBS})
	add_executable(HSOpticalFlow_Hip_Ref_Multi_nopin src/HSOpticalFlow_Hip_Ref_Multi_nopin.cpp)
	target_link_libraries(HSOpticalFlow_Hip_Ref_Multi_nopin ${CTRL_LIBS})
	add_executable(HSOpticalFlow_Hip_Ref_Multi src/HSOpticalFlow_Hip_Ref_Multi.cpp)
	target_link_libraries(HSOpticalFlow_Hip_Ref_Multi ${CTRL_LIBS})
endif(SUPPORT_HIP)

if(SUPPORT_OPENCL)
	add_executable(HSOpticalFlow_OpenCL_Gpu_Ref_Multi src/HSOpticalFlow_OpenCL_Gpu_Ref_Multi.c)
	target_link_libraries(HSOpticalFlow_OpenCL_Gpu_Ref_Multi ${CTRL_LIBS})

	add_executable(HSOpticalFlow_OpenCL_Gpu_Ref_Multi_img src/HSOpticalFlow_OpenCL_Gpu_Ref_Multi_img.c)
	target_link_libraries(HSOpticalFlow_OpenCL_Gpu_Ref_Multi_img ${CTRL_LIBS})
endif(SUPPORT_OPENCL)
