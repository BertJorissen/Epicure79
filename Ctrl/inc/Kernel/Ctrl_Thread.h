#ifndef _CTRL_KERNEL_THREAD_H_
#define _CTRL_KERNEL_THREAD_H_
/**
 * @file Ctrl_Thread.h
 * @brief Thread block structs and macros.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

/**
 * @brief Stores the absolute identifier of the thread in each dimension.
 */
typedef struct {
	unsigned int dims; /**< Number of dims for this thread */
	unsigned int i;    /**< Identifier of this thread in the 1st */
	unsigned int j;    /**< Identifier of this thread in the 2nd */
	unsigned int k;    /**< Identifier of this thread in the 3rd */
} Ctrl_Thread;

#define CTRL_THREAD_STRINGIFY " typedef struct { unsigned int dims; unsigned int i; unsigned int j; unsigned int k; } Ctrl_Thread; "

/**
 * Null value for \e Ctrl_Thread
 * @hideinitializer
 */
#define CTRL_THREAD_NULL ((Ctrl_Thread){.dims = 0, .i = 0, .j = 0, .k = 0})

/**
 * Initialize thread block \p name
 * @hideinitializer
 *
 * @param name thread block to initialize.
 * @param ... values for each dimension desired. Maximum of 3 dimensions allowed.
 *
 * @pre Maximum of 3 params in \p ...
 * @pre Values in \p ... must be positive integers
 *
 * @see Ctrl_Thread
 */
#define Ctrl_ThreadInit(name, ...) \
	Ctrl_ThreadInitN(name, CTRL_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)

#define Ctrl_ThreadInitN(name, dims, ...) Ctrl_ThreadInitN2(name, dims, __VA_ARGS__)

#define Ctrl_ThreadInitN2(threads, ndims, ...) Ctrl_Thread##ndims(threads, ndims, __VA_ARGS__)

#define Ctrl_Thread1(threads, ndims, param1)         Ctrl_Thread2(threads, ndims, param1, 1)
#define Ctrl_Thread2(threads, ndims, param1, param2) Ctrl_Thread3(threads, ndims, param1, param2, 1)
#define Ctrl_Thread3(threads, ndims, param1, param2, param3) \
	threads.dims = ndims;                                    \
	threads.i    = param1;                                   \
	threads.j    = param2;                                   \
	threads.k    = param3;

#endif // _CTRL_KERNEL_THREAD_H_
