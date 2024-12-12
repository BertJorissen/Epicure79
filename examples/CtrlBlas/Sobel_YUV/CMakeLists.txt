cmake_minimum_required (VERSION 3.20)

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

# Cpu
if (SUPPORT_CPU AND MKL)
	add_executable(Sobel_YUV_Blas_Cpu_Ref_MKL src/Sobel_YUV_Blas_Cpu_Ref_MKL.c)
	target_link_libraries(Sobel_YUV_Blas_Cpu_Ref_MKL ${CTRL_LIBS})

	add_executable(Sobel_YUV_Blas_Cpu_Ctrl src/Sobel_YUV_Blas_Cpu_Ctrl.c)
	target_link_libraries(Sobel_YUV_Blas_Cpu_Ctrl ${CTRL_LIBS})
endif (SUPPORT_CPU AND MKL)

# Cuda
if (SUPPORT_CUDA)
	if(CUBLAS)
		add_executable(Sobel_YUV_Blas_Cuda_Ref_Cublas src/Sobel_YUV_Blas_Cuda_Ref_Cublas.c)
		target_link_libraries(Sobel_YUV_Blas_Cuda_Ref_Cublas ${CTRL_LIBS})
	endif(CUBLAS)
	
	if(MAGMA)
		add_executable(Sobel_YUV_Blas_Cuda_Ref_Magma src/Sobel_YUV_Blas_Cuda_Ref_Magma.c)
		target_link_libraries(Sobel_YUV_Blas_Cuda_Ref_Magma ${CTRL_LIBS})
	endif(MAGMA)
	
	if(CUBLAS OR MAGMA)
		add_executable(Sobel_YUV_Blas_Cuda_Ctrl src/Sobel_YUV_Blas_Cuda_Ctrl.c)
		target_link_libraries(Sobel_YUV_Blas_Cuda_Ctrl ${CTRL_LIBS})
	endif(CUBLAS OR MAGMA)
endif (SUPPORT_CUDA)