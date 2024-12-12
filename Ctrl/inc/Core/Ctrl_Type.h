#ifndef _CTRL_CORE_TYPE_H_
#define _CTRL_CORE_TYPE_H_
/**
 * @file Ctrl_Type.h
 * @brief Ctrl types.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Tipos de dispositivos que pueden gestionar los controladores.
 */
typedef enum {
	CTRL_TYPE_NULL = 0,
	CTRL_TYPE_CPU,
	CTRL_TYPE_CUDA,
	CTRL_TYPE_HIP,
	CTRL_TYPE_OPENCL_GPU,
	CTRL_TYPE_FPGA
} Ctrl_Type;

#ifdef __cplusplus
}
#endif

#endif // _CTRL_CORE_TYPE_H_