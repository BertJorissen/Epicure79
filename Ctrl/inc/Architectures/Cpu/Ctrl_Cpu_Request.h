#ifndef _CTRL_CPU_REQUEST_H_
#define _CTRL_CPU_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_Cpu_Request.h
 * @brief Specific Cpu implementation for a request.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */
/**
 * \brief CPU specific info needed to execute a kernel.
 */
typedef struct {
	int n_cores; /**< Number of threads to be used when executing the kernel */
} Ctrl_Cpu_Request;
///@endcond
#endif // _CTRL_CPU_REQUEST_H_