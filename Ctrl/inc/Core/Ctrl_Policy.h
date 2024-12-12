#ifndef _CTRL_CORE_POLICY_H_
#define _CTRL_CORE_POLICY_H_
/**
 * @file Ctrl_Policy.h
 * @brief Ctrl_policy enum declaration.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

/**
 * Policy used by the ctrl when executing tasks
 */
typedef enum {
	CTRL_POLICY_SYNC,   /**< Synchronous policy, no task overlapping */
	CTRL_POLICY_ASYNC,  /**< Asycnronous policy, task overlapping when possible */
	CTRL_POLICY_DEFAULT /**< Policy specified in the environment */
} Ctrl_Policy;

#endif // _CTRL_CORE_POLICY_H_