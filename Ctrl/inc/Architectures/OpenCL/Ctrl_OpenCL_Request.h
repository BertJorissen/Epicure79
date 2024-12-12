#ifndef _CTRL_OPENCL_REQUEST_H_
#define _CTRL_OPENCL_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_OpenCL_Request.h
 * @brief Specific OpenCL GPU implementation for a request.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

/**
 * Linked list contining information about kernels defined.
 *
 * This is used to preload kernels when the ctrl is created.
 */
typedef struct Ctrl_OpenCLGpu_KernelParams {
	cl_kernel  *p_kernel;      /**< Pointer to the kernels. Of size num ctrls of type OpenCL GPU. */
	cl_program *p_program;     /**< Pointer to the programs. Of size num ctrls of type OpenCL GPU. */
	const char *p_kernel_name; /**< String containing the kernel's name */
	char       *p_kernel_raw;  /**< String containing kernel's code */

	struct Ctrl_OpenCLGpu_KernelParams *p_next; /**< Pointer to the next element in the list */
} Ctrl_OpenCLGpu_KernelParams;

/**
 * Null value for \e Ctrl_FPGA_KernelParams
 * @hideinitializer
 */
#define CTRL_OPENCLGPU_KERNELPARAMS_NULL \
	{.p_kernel = NULL, .p_program = NULL, .p_kernel_name = NULL, .p_kernel_raw = NULL, .p_next = NULL}

/**
 * initial node of the kernel info.
 */
extern Ctrl_OpenCLGpu_KernelParams OpenCLGpu_initial_kp;

/**
 * \brief OpenCL_GPU specific info needed to execute a kernel.
 */
typedef struct {
	cl_command_queue *queue;               /**< Queue to launch the kernel to */
	cl_device_id     *device_id;           /**< Id of the device to execute the kernel*/
	cl_event         *p_last_kernel_event; /**< Event to record this kernel into */
	int               n_arguments;         /**< Number of arguments passed to the user defined kernel */
	char             *p_roles;             /**< List of roles of the arguments passed to the kernel */
	uint16_t         *p_displacements;     /**< Displacement of parameter over arguments array */
	int               type_id;             /**< Id of the ctrl respect to other OpenCL Gpu ctrls */
} Ctrl_OpenCL_Request;
///@endcond
#endif // _CTRL_OPENCL_REQUEST_H_