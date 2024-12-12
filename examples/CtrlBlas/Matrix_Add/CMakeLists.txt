cmake_minimum_required (VERSION 3.20)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_ADD)
	message(STATUS "MATRIX_ADD_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "MATRIX_ADD_LIBS = ${CTRL_LIBS}")
	message(STATUS "MATRIX_ADD_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "MATRIX_ADD_CTRL_CXX vars = ${CMAKE_CXX_FLAGS}")
	message(STATUS "MATRIX_ADD_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_ADD)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# Cpu
if (SUPPORT_CPU AND MKL)
	add_executable(Matrix_Add_Blas_Cpu_Ctrl src/Matrix_Add_Blas_Cpu_Ctrl.c)
	target_link_libraries(Matrix_Add_Blas_Cpu_Ctrl ${CTRL_LIBS})

	add_executable(Matrix_Add_Blas_Cpu_Ref_MKL src/Matrix_Add_Blas_Cpu_Ref_MKL.c)
	target_link_libraries(Matrix_Add_Blas_Cpu_Ref_MKL ${CTRL_LIBS})
endif (SUPPORT_CPU AND MKL)

# Cuda
if (SUPPORT_CUDA)
	if(CUBLAS)
		add_executable(Matrix_Add_Blas_Cuda_Ref_Cublas src/Matrix_Add_Blas_Cuda_Ref_Cublas.c)
		target_link_libraries(Matrix_Add_Blas_Cuda_Ref_Cublas ${CTRL_LIBS})
	endif(CUBLAS)
	
	if(MAGMA)
		add_executable(Matrix_Add_Blas_Cuda_Ref_Magma src/Matrix_Add_Blas_Cuda_Ref_Magma.c)
		target_link_libraries(Matrix_Add_Blas_Cuda_Ref_Magma ${CTRL_LIBS})
	endif(MAGMA)
	
	if(CUBLAS OR MAGMA)
		add_executable(Matrix_Add_Blas_Cuda_Ctrl src/Matrix_Add_Blas_Cuda_Ctrl.c)
		target_link_libraries(Matrix_Add_Blas_Cuda_Ctrl ${CTRL_LIBS})
	endif(CUBLAS OR MAGMA)
endif(SUPPORT_CUDA)