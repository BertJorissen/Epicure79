#ifndef _CTRL_CORE_KHITTILE_H_
#define _CTRL_CORE_KHITTILE_H_
/**
 * @file Ctrl_KHitTile.h
 * @brief KhitTile structure definitions.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#define CTRL_FPGA_STRUCT_ALIGNMENT 16
///@cond INTERNAL
#ifndef CTRL_FPGA_KERNEL_FILE

// TODO @sergioalo probably we should avoid dependencies from here to cuda/ocl headers
#ifdef _CTRL_ARCH_CUDA_
#include <cuda_runtime_api.h>
#endif // _CTRL_ARCH_CUDA_

#ifdef _CTRL_ARCH_HIP_
#include <hip/hip_runtime_api.h>
#endif // _CTRL_ARCH_HIP_

#ifdef _CTRL_ARCH_OPENCL_GPU_
#include <CL/cl.h>
#endif // _CTRL_ARCH_OPENCL_GPU_

/**
 * Holds backend specific extra fields
 */
typedef union Ctrl_KHitTile_Ext {
	#ifdef _CTRL_ARCH_CUDA_
	struct {
		cudaTextureObject_t tex; /**< For texture use */
	} cuda;
	#endif // _CTRL_ARCH_CUDA_
	#ifdef _CTRL_ARCH_HIP_
	struct {
		hipTextureObject_t tex; /**< For texture use */
	} hip;
	#endif // _CTRL_ARCH_HIP_
	#ifdef _CTRL_ARCH_OPENCL_GPU_
	struct {
		cl_mem     tex; /**< For texture use */
		cl_sampler smp; /**< For texture use */
	} ocl;
	#endif // _CTRL_ARCH_OPENCL_GPU_
} Ctrl_KHitTile_Ext;

/**
 * Stripped down version of an abstract \e HitTile so it`s more suitable for use in kernels
 */
typedef struct {
	void             *data;            /**< Pointer to the data held by the tile. */
	int               origAcumCard[4]; /**< Dimension accumulated cardinalities. */
	int               card[3];         /**< Dimension cardinalities. */
	int               offset;          /**< Offset to original data. For hierarchical subselections. */
	Ctrl_KHitTile_Ext ext;             /**< Backend specific extra fields  */
} KHitTile;
#endif

/* @author: Gabriel Rodriguez-Canal
   @brief: KHitTile wrapper to isolate tile coordinates and offset from data, as structures with pointers cannot be passed as arguments */
#ifdef _CTRL_ARCH_FPGA_
typedef struct __attribute__((packed)) __attribute__((aligned(CTRL_FPGA_STRUCT_ALIGNMENT))) {
	int origAcumCard[4];
	int card[3];
	int offset;
} KHitTile_fpga_wrapper;
#endif
#ifdef _CTRL_ARCH_OPENCL_GPU_
typedef struct {
	int origAcumCard[4];
	int card[3];
	int offset;
} KHitTile_opencl_wrapper;
#endif

#ifndef CTRL_FPGA_KERNEL_FILE
/**
 * Generate particular polymorphic types of KHitTile as well as appropiate wrappers for OpenCL GPU and FPGA.
 * @hideinitializer
 *
 * @param type Name of a valid native or derived C type.
 *
 * @see Ctrl_NewType, KHitTile
 */
#define hit_ktileNewType(type)             \
	typedef struct {                       \
		type             *data;            \
		int               origAcumCard[4]; \
		int               card[3];         \
		int               offset;          \
		Ctrl_KHitTile_Ext ext;             \
	} KHitTile_##type;                     \
	typedef struct {                       \
		int origAcumCard[4];               \
		int card[3];                       \
		int offset;                        \
	} fpga_wrapper_KHitTile_##type;        \
	CTRL_KTILE_VARS(type);

#define CTRL_KTILE_VARS(type)                                                                              \
	static const char *raw_ktile_KHitTile_##type __attribute__((unused))     = CTRL_MACRO_STRINGIFY(type); \
	static const char *raw_def_ktile_KHitTile_##type __attribute__((unused)) = CTRL_MACRO_STRINGIFY(       \
		typedef struct {                                                                                   \
			__global type *data;                                                                           \
			int            origAcumCard[4];                                                                \
			int            card[3];                                                                        \
		} KHitTile_##type##_write;                                                                         \
		typedef struct {                                                                                   \
			__global const type *data;                                                                     \
			int                  origAcumCard[4];                                                          \
			int                  card[3];                                                                  \
		} KHitTile_##type##_read;                                                                          \
		typedef struct {                                                                                   \
			int origAcumCard[4];                                                                           \
			int card[3];                                                                                   \
			int offset;                                                                                    \
		} KHitTile_##type##_wrapper;);                                                                     \
	static bool raw_added_ktile_KHitTile_##type __attribute__((unused)) = false;

#else // CTRL_FPGA_KERNEL_FILE

#if !defined( CTRL_HOST_COMPILE ) && !defined( CTRL_FPGA_BIN_NAME )
#define hit_ktileNewType(type)                                                                    \
	typedef struct __attribute__((packed)) __attribute__((aligned(CTRL_FPGA_STRUCT_ALIGNMENT))) { \
		__global type *data;                                                                      \
		int            origAcumCard[4];                                                           \
		int            card[3];                                                                   \
		int            offset;                                                                    \
	} KHitTile_##type;                                                                            \
                                                                                                  \
	typedef struct {                                                                              \
		int origAcumCard[4];                                                                      \
		int card[3];                                                                              \
		int offset;                                                                               \
	} fpga_wrapper_KHitTile_##type;                                                               \
                                                                                                  \
	typedef __global void *restrict data_KHitTile_##type;
#endif // !CTRL_HOST_COMPILE && !CTRL_FPGA_BIN_NAME

#endif // CTRL_FPGA_KERNEL_FILE

///@endcond
#endif // _CTRL_CORE_KHITTILE_H_
