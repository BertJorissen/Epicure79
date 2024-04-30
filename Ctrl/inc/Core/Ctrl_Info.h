#ifndef _CTRL_CORE_INFO_H_
#define _CTRL_CORE_INFO_H_
/**
 * @file Ctrl_Info.h
 * @author Trasgo Group
 * @brief Struct for the information of the device attached to a ctrl.
 * @version 2.1
 * @date 2021-04-26
 *
 * @copyright This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @copyright Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * @copyright More information on http://trasgo.infor.uva.es/
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
		.mem_transfers   = 0     \
	}

#endif // _CTRL_CORE_INFO_H_
