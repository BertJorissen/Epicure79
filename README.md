Controllers library
====
Controllers is a library written in C99 that provides a simplified way to program application that can exploit heterogeneous computational platforms including accelerators and/or multi-core CPUs.
Controllers uses dependence analysis on a sequence of data-transfer, host computation and kernel execution operations, automatically overlapping them whenever possible.
The runtime library supports several technologies and lower-level programming models (such as OpenMP, CUDA, or OpenCL), choosing the proper kernel for any device at runtime. This avoids the need to recompile the whole application to execute it using a different device.
The solution maps the execution control directly to the coprocessor driver resources, avoiding the use of a full scheduler or task-graph analysis techniques. This derives in a high efficiency and minimum overhead introduced by the control system.

Controllers currently supports 4 different backends for different accelerator architectures:
 * CUDA (for NVIDIA's GPUs)
 * OpenCL (for other GPUs, such as AMD)
 * CPU (for subsets of CPU cores using OpenMP)
 * FPGA (for Intel FPGAs using OpenCL and the Intel AOC compiler)

The backend for Intel XeonPhi devices has been discontinued in the latest version of Controller.

Prerequisites
----
Controllers has some core dependencies, which are always required and then support for a specific architecture might have some aditional dependencies.

Core dependencies:
 - MPI (3.x or higher)
 - OpenMP (4.x or higher)
 - hwloc (1.11.x)

CUDA architecture dependencies:
 - CUDA (9.x or higher)

OpenCL GPU architecture dependencies:
 - OpenCl

FPGA architecture dependencies:
 - OpenCL
 - aoc (20.3)

Compiling Controllers
----
Compilation is done using CMake (3.17 or higher).

```
mkdir -p build
cd build
cmake ..
make clean
make
```

When compiling Controllers, the supported architectures for which you have the proper compiler and libraries installed, should be specified either by changing default values in CMake files
```cmake
# Architectures
option(SUPPORT_CPU "SUPPORT_CPU" OFF)
option(SUPPORT_CUDA "SUPPORT_CUDA" OFF)
option(SUPPORT_OPENCL_GPU "SUPPORT_OPENCL_GPU" OFF)
option(SUPPORT_OPENCL_GPU_AMD "Use AMD platform" OFF)
option(SUPPORT_FPGA "SUPPORT_FPGA" OFF)
```
or by selecting them when running cmake like this:
```
cmake -DSUPPORT_CUDA=ON ..
```

To see the full list of configuration options and their default values run:
```
mkdir -p build
cd build
cmake -LH ..
```

Executing an example
```
./build/examples/Matrix_Add/Matrix_Add_Cuda_Ctrl 100 1 0 0 1
```

Generating documentation
----
Documentation for the Controllers library can be generated using Doxygen.
```
cd Ctrl/doc
doxygen Doxyfile
```

Contributing
----
Thanks for your interest in contributing! There are many ways to contribute to this project.

Feel free to contact us at trasgo@infor.uva.es to share your experience with us or visit our page https://trasgo.infor.uva.es/ to learn more about us.
If you find any bugs or problems, or want to request a new feature feel free to create an issue detailing it.

If you want to contribute code to the project get started [here](./CONTRIBUTE.md).

Relevant publications
---
Here are some of the more important publications about this project:

 - Efficient heterogeneous programming with FPGAs using the Controller model. G. Rodriguez-Canal, Y. Torres, F.J. Andújar, A. Gonzalez-Escribano. The Journal of Supercomputing, Springer, 2021. DOI: 10.1007/s11227-021-03792-7

 - Controllers: An abstraction to ease the use of hardware accelerators. A. Moretón-Fernández, H. Ortega-Arranz, A. Gonzalez-Escribano. The International Journal on High Performance Computing Aplications (IJHPCA), 32 (6), pag. 838-853, SAGE Journals, 2018. DOI: 10.1177/1094342017702962