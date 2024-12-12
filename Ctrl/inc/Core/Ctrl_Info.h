#ifndef _CTRL_CORE_INFO_H_
#define _CTRL_CORE_INFO_H_
/**
 * @file Ctrl_Info.h
 * @brief Struct for the information of the device attached to a ctrl.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#define CTRL_MAX_DEV_NAME 256

/**
 * Struct containing information from the device asociated with a ctrl.
 * Some fields may be empty depending on the type of ctrl.
 */
typedef struct Ctrl_Info {
	char *type;                             /**< Type of the ctrl */
	char  device_name[CTRL_MAX_DEV_NAME];   /**< Name of the device */
	char  platform_name[CTRL_MAX_DEV_NAME]; /**< Name of the platform of the device (only for OpenCL GPU and FPGA ctrls)*/
	char *exec_mode;                        /**< Execution mode (only for FPGA ctrls)*/
	int   host_affinity;                    /**< Index of numa node to use as host */
	int   n_kernel_queues;                  /**< Number of kernel queues in this device (empty on CPU ctrls)*/
	int   n_threads;                        /**< Number of threads used to execute kernels (only for CPU ctrls)*/
	int   numa_range_min;                   /**< End of the range of numa nodes of the device. Inclusive. Indexes as reported by hwloc. (only for CPU ctrls)*/
	int   numa_range_max;                   /**< End of the range of numa nodes of the device. Not inclusive. Indexes as reported by hwloc. (only for CPU ctrls)*/
	int   mem_transfers;                    /**< Boolean. Ctrl only uses host memory and memory copies are a noop. (only for CPU ctrls)*/
} Ctrl_Info;

/**
 * Null value for \e Ctrl_Info
 * @hideinitializer
 */
#define CTRL_INFO_NULL           \
	{                            \
		.type            = NULL, \
		.device_name     = {0},  \
		.platform_name   = {0},  \
		.exec_mode       = NULL, \
		.host_affinity   = 0,    \
		.n_kernel_queues = 0,    \
		.n_threads       = 0,    \
		.numa_range_min  = 0,    \
		.numa_range_max  = 0,    \
		.mem_transfers   = 0}

#endif // _CTRL_CORE_INFO_H_
