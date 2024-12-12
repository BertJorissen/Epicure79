/**
 * @file epsilod_ext_type.h
 * @brief Epsilod: Default type declaration for external/extra parameters
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#ifndef _EPSILOD_EXT_TYPES_H_
#define _EPSILOD_EXT_TYPES_H_

/* Default empty user type for extra parameters */
#ifndef CTRL_USER_TYPES
#define CTRL_USER_TYPES \
	typedef struct {    \
		int foo;        \
	} Epsilod_ext;
#endif

#endif // EPSILOD_EXT_TYPES_H
