Parallel stencil skeleton
====
This folder contains the software, case study programs and experimental data used in the work "Towards an efficient parallel skeleton for generic iterative stencil computations in distributed GPUs", presented as a poster to the SC21. This folder is subdivided in the following directories:
 * `src` contains the source code for the parallel stencil skeleton developed, which uses the Controllers and Hitmap libraries. The source code for this libraries can be found in other folders of this repository. The code for the parallel stencil skeleton inside this directory is automatically compiled by CMake when compiling the Controllers library (see the README.md file in the parent directory). It also contains the tool to generate optimized stencil kernels to use with the parallel skeleton from a stencil description file.
 * `comparison_codes` contains the following programs used to test the relative performance of the parallel skeleton: 
   - a native CUDA implementation of the Jacobi 2D 4-point stencil, used to compare the performance achieved with the parallel skeleton in terms of Gflops (as seen in one of the tables submitted to SC 2021).
   - a Celerity implementation of the Jacobi 2D 4-point stencil.
