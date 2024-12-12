#ifndef _CTRL_CORE_CONFIG_H_
#define _CTRL_CORE_CONFIG_H_
///@cond INTERNAL
/**
 * @file Ctrl_Config.h
 * @brief Struct for the configuration of a Ctrl.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Core/Ctrl_Type.h"

typedef struct Ctrl_Config {
	Ctrl_Type type;
	char     *args;
} Ctrl_Config;

///@endcond
#endif // _CTRL_CORE_CONFIG_H_