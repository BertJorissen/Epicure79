Parallel stencil skeleton kernel generator
====
This folder contains the tool to automatically generate stencil kernels to use with the parallel stencil skeleton. To generate a kernel, use `parallel_stencil_skeleton_kernel_generator.sh`, providing the stencil description file as argument.

Kernel description files are written using C syntax, and must contain the following three elements:
 - The shape of the stencil, as a HitShape: `HitShape shp_stencil = hitShape(...);`
 - The stencil pattern of weights of the neighbors, as an array of floats: `float patt_stencil[] = { ... };`
 - The division factor for the weighted sum, as a float: `float factor_stencil = ...;`
Multiple stencils can be defined in the same description file. For more information on the structure of the stencil description files, read the provided examples.

**NOTE:** the parallel stencil skeleton has a generic kernel that can compute any kind of stencil representable by the specified three elements, without the need to define a specific kernel code. That kernel is used by the skeleton function when a `NULL` value is provided as the parameter for the pointer to the kernel to compute. However, any kernel code generated with this tool will achieve higher performances than those reachable by the generic kernel.

Restrictions
---
Currently, the tool presents the following restrictions:
 - The stencil shape must be written in just one line.
 - The division factor must be written in just one line.
 - The stencil pattern may be written in multiple lines, but everything up to and including the left curly bracket (`{`) that begins the array must be written in the very first line.
 - The name for the stencil shape must start with `shp`.
 - The name for the stencil pattern must start with `patt`.
 - The name for the division factor must start with `factor`.
 - The tool doesn't check for syntax or lexical errors. Erroneous descriptions will generate kernel code. This erroneous code will cause compile-time errors when compiling the final program.
 - There is no way to specify the order in which floating point operations are generated in the kernel code. This is important when validating the results of the generated kernel, as the same floating point operations executed in different order will produce different results (floating point number operations are not associative).
 - Currently, only CUDA-compatible kernels can be generated.
