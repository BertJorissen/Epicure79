# <license>
# 
# Controller v2.1
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

# Common options
if (SUPPORT_CUDA)
	set(CTRL_EXAMPLES_CUDA_ERROR_CHECK OFF) # Check errors with this
	set(CTRL_EXAMPLES_CUDA_DEBUG OFF) # Debug whit this
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_0 "16") # Block Size 0
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_1 "16") # Block Size 1
	set(CTRL_EXAMPLES_CUDA_BLOCKSIZE_2 "0") # Block Size 1
endif (SUPPORT_CUDA)

if (SUPPORT_CPU)
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_0 "16") # Block Size 0
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_1 "16") # Block Size 1
	set(CTRL_EXAMPLES_CPU_BLOCKSIZE_2 "0") # Block Size 1
endif (SUPPORT_CPU)

# Apply options
if (CTRL_EXAMPLES_CUDA_ERROR_CHECK)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_ERROR_CHECK_ ")
endif (CTRL_EXAMPLES_CUDA_ERROR_CHECK)

if (CTRL_EXAMPLES_CUDA_DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CTRL_EXAMPLES_CUDA_DEBUG_ ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -D_CTRL_EXAMPLES_CUDA_DEBUG_ ")
endif (CTRL_EXAMPLES_CUDA_DEBUG)

if (SUPPORT_CUDA)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CUDA_BLOCKSIZE_2} ")
	set(CUDA_PROPAGATE_HOST_FLAGS ON)
endif (SUPPORT_CUDA)

if (SUPPORT_CPU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CPU_BLOCKSIZE_2} ")
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBLOCKSIZE_0=${CTRL_EXAMPLES_CPU_BLOCKSIZE_0} -DBLOCKSIZE_1=${CTRL_EXAMPLES_CPU_BLOCKSIZE_1} -DBLOCKSIZE_2=${CTRL_EXAMPLES_CPU_BLOCKSIZE_2} ")
endif (SUPPORT_CPU)

# Verbose
if (CTRL_CMAKE_VERBOSE_EXAMPLES_SOBEL)
	message(STATUS "SOBEL_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "SOBEL_LIBS = ${CTRL_LIBS}")
	message(STATUS "SOBEL_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "SOBEL_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
	if (SUPPORT_CUDA)
		message(STATUS "SOBEL_CTRL_NVCC vars = ${CUDA_NVCC_FLAGS}")
	endif (SUPPORT_CUDA)
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