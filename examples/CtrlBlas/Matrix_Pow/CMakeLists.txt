# <license>
# 
# Controller v4.0
# 
# This software is provided to enhance knowledge and encourage progress in the scientific
# community. It should be used only for research and educational purposes. Any reproduction
# or use for commercial purpose, public redistribution, in source or binary forms, with or 
# without modifications, is NOT ALLOWED without the previous authorization of the copyright 
# holder. The origin of this software must not be misrepresented; you must not claim that you
# wrote the original software. If you use this software for any purpose (e.g. publication),
# a reference to the software package and the authors must be included.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
# All rights reserved.
# 
# More information on http://trasgo.infor.uva.es/
# 
# </license>
cmake_minimum_required (VERSION 3.17)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_POWER_OF)
	message(STATUS "MATRIX_POWER_OF_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "MATRIX_POWER_OF_LIBS = ${CTRL_LIBS}")
	message(STATUS "MATRIX_POWER_OF_CTRL_C vars = ${CMAKE_C_FLAGS}")
	if (SUPPORT_CUDA)
		message(STATUS "MATRIX_POWER_OF_CTRL_NVCC vars = ${CUDA_NVCC_FLAGS}")
	endif (SUPPORT_CUDA)
endif(CTRL_CMAKE_VERBOSE_EXAMPLES_MATRIX_POWER_OF)

# Include directories
include_directories(${CTRL_INCLUDE_DIRS})

# Cpu
if (SUPPORT_CPU AND MKL)
	add_executable(Matrix_Pow_Blas_Cpu_Ref_MKL src/Matrix_Pow_Blas_Cpu_Ref_MKL.c)
	target_link_libraries(Matrix_Pow_Blas_Cpu_Ref_MKL ${CTRL_LIBS})

	add_executable(Matrix_Pow_Blas_Cpu_Ctrl src/Matrix_Pow_Blas_Cpu_Ctrl.c)
	target_link_libraries(Matrix_Pow_Blas_Cpu_Ctrl ${CTRL_LIBS})
endif (SUPPORT_CPU AND MKL)

# Cuda
if (SUPPORT_CUDA)
	if(CUBLAS)
		add_executable(Matrix_Pow_Blas_Cuda_Ref_Cublas src/Matrix_Pow_Blas_Cuda_Ref_Cublas.c)
		target_link_libraries(Matrix_Pow_Blas_Cuda_Ref_Cublas ${CTRL_LIBS})
	endif(CUBLAS)
	
	if(MAGMA)
		add_executable(Matrix_Pow_Blas_Cuda_Ref_Magma src/Matrix_Pow_Blas_Cuda_Ref_Magma.c)
		target_link_libraries(Matrix_Pow_Blas_Cuda_Ref_Magma ${CTRL_LIBS})
	endif(MAGMA)
	
	if(CUBLAS OR MAGMA)
		add_executable(Matrix_Pow_Blas_Cuda_Ctrl src/Matrix_Pow_Blas_Cuda_Ctrl.c)
		target_link_libraries(Matrix_Pow_Blas_Cuda_Ctrl ${CTRL_LIBS})
	endif(CUBLAS OR MAGMA)
endif(SUPPORT_CUDA)