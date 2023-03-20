EPSILOD
====
This folder contains the software and some case study programs used in the work ["EPSILOD: Efficient parallel skeleton for generic iterative stencil computations in distributed GPUs"](https://link.springer.com/article/10.1007/s11227-022-05040-y), presented as an article to The Journal of Supercomputing. This directory is subdivided in the following directories:
 * `src` contains the source code for EPSILOD (the Parallel Stencil Skeleton), which uses the Controllers and Hitmap libraries. The source code for those libraries can be found in other folders of this repository. EPSILOD is automatically compiled by CMake when compiling the Controllers library (see the README.md file in the parent directory). It also contains the tool to generate optimized stencil kernels to use with the parallel skeleton from a stencil description file, also known as KGT (kernel generator tool).
 * `comparison_codes` contains the following programs used to test the relative performance of the parallel skeleton:
   - a native CUDA implementation of the Jacobi 2D 4-point stencil, used to compare the performance achieved with the parallel skeleton in terms of Gflops (as seen in one of the tables of the poster submitted to SC 2021 as previous work).
   - a Celerity implementation of the Jacobi 2D 4-point stencil.

Citation
----
If you use EPSILOD or do research based on it, please cite it as below.

 * [EPSILOD: efficient parallel skeleton for generic iterative stencil computations in distributed GPUs](https://link.springer.com/article/10.1007/s11227-022-05040-y)
```BibTeX
@article{deCastro2023:EPSILOD,
				author={de Castro, Manuel
				and Santamaria-Valenzuela, Inmaculada
				and Torres, Yuri
				and Gonzalez-Escribano, Arturo
				and Llanos, Diego R.},
				title={EPSILOD: efficient parallel skeleton for generic iterative stencil computations in distributed GPUs},
				journal={The Journal of Supercomputing},
				year={2023},
				month={Jan},
				issn={1573-0484},
				doi={10.1007/s11227-022-05040-y},
}

```

