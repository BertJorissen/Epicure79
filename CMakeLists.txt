cmake_minimum_required (VERSION 3.20)

project (Ctrl LANGUAGES C CXX)

# Options

# Architectures
option(SUPPORT_CPU "SUPPORT_CPU" ON) # Support for CPU Ctrl
option(SUPPORT_CUDA "SUPPORT_CUDA" OFF) # Support for CUDA Ctrl
option(SUPPORT_HIP "SUPPORT_HIP" OFF) # Support for HIP Ctrl
option(SUPPORT_OPENCL "SUPPORT_OPENCL" OFF) # Support for OpenCL GPU Ctrl
option(SUPPORT_FPGA "SUPPORT_FPGA" OFF) # Support for OpenCL FPGA Ctrl
option(SUPPOPRT_INTEL "SUPPORT_INTEL" OFF) # Use Intel platform in FPGA backend
option(SUPPORT_XILINX "SUPPORT_XILINX" OFF) # Use Xilinx platform in FPGA backend

if(SUPPORT_HIP AND SUPPORT_CUDA)
  set(SUPPORT_HIP ON)
  set(SUPPORT_CUDA OFF)
  message(STATUS "Warning: CUDA backend compilation has been deactivated. CUDA and HIP have non-compatible declarations on their header files. This will be solved in a future release.")
  message(STATUS "Deafult: Compile only HIP backend.")
endif(SUPPORT_HIP AND SUPPORT_CUDA)

if(SUPPORT_INTEL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_INTEL_KERNELS")
endif(SUPPORT_INTEL)
if(SUPPORT_XILINX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XILINX_KERNELS")
endif(SUPPORT_XILINX)


# BLAS library support
option(CUBLAS "CUBLAS" OFF)
option(HIPBLAS "HIPBLAS" OFF)
option(MKL "MKL" OFF)
option(MAGMA "MAGMA" OFF)

# Paths to stuff
set(ROCM_PATH "/opt/rocm" CACHE PATH "Path to rocm installation")
set(OPENCL_PATH "/usr/local/cuda" CACHE PATH "Path to OpenCL installation")
#TODO @sergioalo move magma lib to a common dir
set(MAGMA_ROOT "/frontend/sergioalo/magma-2.6.0-gcc-10.3-mkl-2021.2.0-cuda-11.3" CACHE PATH "Path to magma installation")

# Examples selector
option(USE_EXPERIMENTATION_EXAMPLES "USE_EXPERIMENTATION_EXAMPLES" OFF) # Short output, easier to parse automatically

# Verbose
set(CTRL_CMAKE_VERBOSE OFF) # Debug cmake
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Use of debbuger
option(CTRL_DEBUG "CTRL_DEBUG" OFF) # Debug mode of Ctrl for external debuggers (ex. gdb, valdgrind)

# Profiling
option(PROFILING_ENABLED "enable profiling" OFF) # Enable profiling host tasks for OpenCL AMD and CUDA 

# Error checking
option(CTRL_ERROR_CHECK "enable error checking" ON) # Enable error checking for cuda,opencl and hip operations
if(CTRL_ERROR_CHECK)
	set(OPENCL_GPU_ERROR_CHECK ON) # Activate OpenCL error checking
	set(CUDA_ERROR_CHECK ON) # Activate CUDA error checking
	set(HIP_ERROR_CHECK ON) # Activate HIP error checking

	set(CTRL_EXAMPLES_CUDA_ERROR_CHECK ON) # Activate CUDA error checking on reference examples
	set(CTRL_EXAMPLES_HIP_ERROR_CHECK ON) # Activate HIP error checking on reference examples
	set(CTRL_EXAMPLES_OPENCL_GPU_ERROR_CHECK ON) # Activate OpenCL error checking on reference examples
	set(CTRL_EXAMPLES_FPGA_ERROR_CHECK ON) # Activate OpenCL error checking on reference examples
endif(CTRL_ERROR_CHECK)

# Extra options OpenCL
add_compile_definitions( CL_TARGET_OPENCL_VERSION=200 )
set(OPENCL_GPU_PROFILING OFF) # Enable profiling on OpenCL GPU Ctrl
set(OPENCL_GPU_PROFILING_VERBOSE OFF) # generate all the info from all the events to a file
set(OPENCL_GPU_TEST_OUTPUT OFF) # Print in test mode (for script parse)
set(OPENCL_GPU_PROFILING_N_READ_TASKS "5000") # Numero de tareas de lectura 
set(OPENCL_GPU_PROFILING_N_WRITE_TASKS "5000") # Numero de tareas de escritura
set(OPENCL_GPU_PROFILING_N_KERNEL_TASKS "5000") # Numero de tareas de kernels

# Extra options FPGA
option(FPGA_PROFILING "FPGA_PROFILING" OFF)
if(FPGA_PROFILING)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFPGA_PROFILING" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFPGA_PROFILING" )
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DFPGA_PROFILING" )
endif(FPGA_PROFILING)

option(FPGA_EMULATION "FPGA_EMULATION" ON)
if(FPGA_EMULATION)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFPGA_EMULATION" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFPGA_EMULATION" )
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DFPGA_EMULATION" )
endif(FPGA_EMULATION)

#Enable to compile FPGA kernels
option(COMPILE_KERNELS "COMPILE_KERNELS" ON)
# Choose the version you wish to compile
option(COMPILE_CTRL "COMPILE_CTRL" ON)
option(COMPILE_REF "COMPILE_REF" ON)

option(COMPILE_INTEL "COMPILE_INTEL" ON)
option(COMPILE_XILINX "COMPILE_XILINX" OFF)

# Extra options Verbose
if (CTRL_CMAKE_VERBOSE)
    set(CTRL_CMAKE_VERBOSE_MAIN ON) # Debug cmake Main (this)
    set(CTRL_CMAKE_VERBOSE_CTRL ON) # Debug cmake Ctrl (this)
    set(CTRL_CMAKE_VERBOSE_EXAMPLES ON) # Debug cmake Examples (this)
endif(CTRL_CMAKE_VERBOSE)

# Common flags and libs
if(CTRL_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g3 -O0 -Wall -D_CTRL_DEBUG_" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0 -Wall -D_CTRL_DEBUG_" )
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -g3 -O0 -Wall -D_CTRL_DEBUG_" )
else(CTRL_DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3 -Wall " )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall " )
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -O3 -Wall " )
endif(CTRL_DEBUG)

# External libraries
set(CTRL_LIBS ${CTRL_LIBS} -lm)

# Hitmap
set(HITMAP_PATH "${PROJECT_SOURCE_DIR}/extern/hitmap")

find_library(HITMAPLIB hit HINTS ${HITMAP_PATH}/lib)
if(NOT HITMAPLIB)
	execute_process(COMMAND make -j 12 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extern/hitmap)
	find_library(HITMAPLIB hit HINTS ${HITMAP_PATH}/lib)
endif(NOT HITMAPLIB)

set(HITMAP_INCLUDE_PATHS ${HITMAP_INCLUDE_PATHS} ${HITMAP_PATH}/include)
set(HITMAP_INCLUDE_PATHS ${HITMAP_INCLUDE_PATHS} ${HITMAP_PATH}/extern/iohb1.0)
set(HITMAP_INCLUDE_PATHS ${HITMAP_INCLUDE_PATHS} ${HITMAP_PATH}/extern/matrix_io)
set(HITMAP_INCLUDE_PATHS ${HITMAP_INCLUDE_PATHS} ${HITMAP_PATH}/extern/metis/Lib)

set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${HITMAP_INCLUDE_PATHS})
set(CTRL_LIBS ${CTRL_LIBS} ${HITMAPLIB})

# Versions
set(VERSION_MAJOR    3)
set(VERSION_MINOR    0)
set(VERSION_REVISION 1)

# MPI
find_package(MPI REQUIRED)
if (MPI_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMPICH_SKIP_MPICXX -DMPI_NO_CPPBIND -DSGIMPI ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMPICH_SKIP_MPICXX -DMPI_NO_CPPBIND -DSGIMPI ")
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DMPICH_SKIP_MPICXX -DMPI_NO_CPPBIND -DSGIMPI ")
    set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${MPI_C_INCLUDE_DIRS})
    set(CTRL_LIBS ${CTRL_LIBS} MPI::MPI_C MPI::MPI_CXX )
endif(MPI_FOUND)

# OpenMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} ")
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} ${OpenMP_CXX_FLAGS} ")
    set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${OpenMP_C_INCLUDE_DIRS})
	if(NOT SUPPORT_HIP)
		set(CTRL_LIBS ${CTRL_LIBS} OpenMP::OpenMP_C)
	endif(NOT SUPPORT_HIP)
endif(OPENMP_FOUND)

# HWLOC
if(DEFINED SKIP_HWLOC)
    message(STATUS "Skipping hwloc")
else()
    set(CTRL_LIBS ${CTRL_LIBS} "-lhwloc")
endif()

# CUDA
if (SUPPORT_CUDA)
	enable_language(CUDA)
	set(CMAKE_CUDA_HOST_COMPILER "${CMAKE_CXX_COMPILER}")
	set(CMAKE_CUDA_ARCHITECTURES "50;72;80;89")
	find_package(CUDAToolkit REQUIRED)
	set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${CUDAToolkit_INCLUDE_DIRS})
	set(CTRL_LIBS ${CTRL_LIBS} CUDA::cudart)
endif (SUPPORT_CUDA)

# OpenCL
if (SUPPORT_OPENCL OR SUPPORT_FPGA)
	list(APPEND CMAKE_PREFIX_PATH ${OPENCL_PATH})
	find_package(OpenCL REQUIRED)
	set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${OpenCL_INCLUDE_DIRS})
	set(CTRL_LIBS ${CTRL_LIBS} OpenCL::OpenCL)
endif (SUPPORT_OPENCL OR SUPPORT_FPGA)

if (SUPPORT_HIP)
    set(CMAKE_CXX_COMPILER "hipcc")
    if(NOT DEFINED HIP_HIPCC_EXECUTABLE or "${HIP_HIPCC_EXECUTABLE}" STREQUAL "")
        message(STATUS "HIP_HIPCC_EXECUTABLE not set, using default CMAKE_C_COMPILER and CMAKE_C_LINKER")
    else ()
        set(CMAKE_C_COMPILER ${HIP_HIPCC_EXECUTABLE})
        set(CMAKE_C_LINKER   ${HIP_HIPCC_EXECUTABLE})
    endif ()
    list(APPEND CMAKE_PREFIX_PATH ${ROCM_PATH})

	# AMD arquitectures to use, check if this is alredy set in HIP_OFFLOAD_ARCH
    if(DEFINED HIP_OFFLOAD_ARCH)
        # Check if --offload-arch is in HIP_OFFLOAD_ARCH
        if(HIP_OFFLOAD_ARCH MATCHES "--offload-arch")
            # already set, we're good
            set(HIP_OFFLOAD_ARCH "${HIP_OFFLOAD_ARCH}")
        else()
            set(HIP_OFFLOAD_ARCH "${HIP_OFFLOAD_ARCH} --offload-arch=gfx900 --offload-arch=gfx1100")
        endif()
    else()
        set(HIP_OFFLOAD_ARCH "${HIP_OFFLOAD_ARCH} --offload-arch=gfx900 --offload-arch=gfx1100")
    endif()
    set(HIP_CMAKE_CXX_FLAGS "${HIP_CMAKE_CXX_FLAGS} ${HIP_OFFLOAD_ARCH}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HIP_CMAKE_CXX_FLAGS}")

	# FIXME @sergioalo hip functions return codes are ignored on all examples
	add_compile_options(-Wno-unused-result)

    # Find hip
    find_package(hip REQUIRED)
	set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${HIP_INCLUDE_DIRS})
	set(CTRL_LIBS ${CTRL_LIBS} hip::host)

	if(HIPBLAS)
		find_package(HIPBLAS REQUIRED)
		set(CTRL_LIBS ${CTRL_LIBS} roc::hipblas)
	endif(HIPBLAS)

    # Compiling for an AMD platform.
    add_definitions(-D__HIP_PLATFORM_AMD__)
endif (SUPPORT_HIP)

# Verbose
if (CTRL_CMAKE_VERBOSE_MAIN)
    message(STATUS "VERBOSE = ${CMAKE_VERBOSE_MAKEFILE}")
    message(STATUS "CPU = ${SUPPORT_CPU}")
    message(STATUS "CUDA = ${SUPPORT_CUDA}")
    message(STATUS "HIP = ${SUPPORT_HIP}")
    message(STATUS "OPENCL_GPU = ${SUPPORT_OPENCL}")
    message(STATUS "OPENCL_FPGA = ${SUPPORT_FPGA}")
    message(STATUS "C vars = ${CMAKE_C_FLAGS}")
    message(STATUS "CUDA vars = ${CMAKE_CUDA_FLAGS}")
    message(STATUS "CXX vars = ${CMAKE_CXX_FLAGS}")
    message(STATUS "HEADERS = ${CTRL_HEADERS}")
    message(STATUS "INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
    message(STATUS "SOURCES = ${CTRL_SOURCES}")
    message(STATUS "LIBS = ${CTRL_LIBS}")
endif(CTRL_CMAKE_VERBOSE_MAIN)

# Directories
add_subdirectory(Ctrl)

# Verbose
if (CTRL_CMAKE_VERBOSE_MAIN)
    message(STATUS "VERBOSE = ${CMAKE_VERBOSE_MAKEFILE}")
    message(STATUS "CPU = ${SUPPORT_CPU}")
    message(STATUS "CUDA = ${SUPPORT_CUDA}")
    message(STATUS "HIP = ${SUPPORT_HIP}")
    message(STATUS "OPENCL_GPU = ${SUPPORT_OPENCL}")
    message(STATUS "C vars = ${CMAKE_C_FLAGS}")
    message(STATUS "CXX vars = ${CMAKE_CXX_FLAGS}")
    message(STATUS "HEADERS = ${CTRL_HEADERS}")
    message(STATUS "INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
    message(STATUS "SOURCES = ${CTRL_SOURCES}")
    message(STATUS "LIBS = ${CTRL_LIBS}")
endif(CTRL_CMAKE_VERBOSE_MAIN)

# Ctrl.h directory
set(CTRL_HEADER_PATH ${PROJECT_SOURCE_DIR}/Ctrl/inc)
if(USE_EXPERIMENTATION_EXAMPLES)
    set(CTRL_EXPERIMENTATION_MODE ON)
endif(USE_EXPERIMENTATION_EXAMPLES)

# Examples
add_subdirectory(examples)

# Parallel stencil skeleton
add_subdirectory(Epsilod)

# Verbose
if (CTRL_CMAKE_VERBOSE_MAIN)
    message(STATUS "VERBOSE = ${CMAKE_VERBOSE_MAKEFILE}")
    message(STATUS "CPU = ${SUPPORT_CPU}")
    message(STATUS "CUDA = ${SUPPORT_CUDA}")
    message(STATUS "HIP = ${SUPPORT_HIP}")
    message(STATUS "OPENCL_GPU = ${SUPPORT_OPENCL}")
    message(STATUS "OPENCL_FPGA = ${SUPPORT_FPGA}")
    message(STATUS "C vars = ${CMAKE_C_FLAGS}")
    message(STATUS "CXX vars = ${CMAKE_CXX_FLAGS}")
    message(STATUS "HEADERS = ${CTRL_HEADERS}")
    message(STATUS "INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
    message(STATUS "SOURCES = ${CTRL_SOURCES}")
    message(STATUS "LIBS = ${CTRL_LIBS}")
endif(CTRL_CMAKE_VERBOSE_MAIN)

if (COMPILE_KERNELS)
    set(CTRL_KERNELS_PATH ${PROJECT_SOURCE_DIR}/kernels)
    add_subdirectory(kernels/FPGA)
endif (COMPILE_KERNELS)
