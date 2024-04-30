#ifndef _CTRL_FPGA_REQUEST_H_
#define _CTRL_FPGA_REQUEST_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA_Request.h
 * @author Gabriel Rodriguez-Canal
 * @brief Specific FPGA implementation for a request.
 * @version 4.0
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

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

/**
 * Linked list contining information about kernels defined.
 *
 * This is used to preload kernels when the ctrl is created.
 */
typedef struct Ctrl_FPGA_KernelParams {
	cl_kernel  *p_kernel;      /**< Pointer to the kernel. Of size num ctrls of type FPGA. */
	cl_program *p_program;     /**< Pointer to the program. Of size num ctrls of type FPGA. */
	const char *p_kernel_name; /**< String containing the kernel's name */
	char       *p_binary_name; /**< Path to kernel binary*/

	struct Ctrl_FPGA_KernelParams *p_next; /**< Pointer to the next element in the list */
} Ctrl_FPGA_KernelParams;

/**
 * Null value for \e Ctrl_FPGA_KernelParams
 * @hideinitializer
 */
#define CTRL_FPGA_KERNELPARAMS_NULL \
	{ .p_kernel = NULL, .p_program = NULL, .p_kernel_name = NULL, .p_binary_name = NULL, .p_next = NULL }

/**
 * initial node of the kernel info.
 */
extern Ctrl_FPGA_KernelParams FPGA_initial_kp;

/**
 * \brief FPGA specific info needed to execute a kernel.
 */
typedef struct {
	cl_command_queue *queue;               /**< Queue to launch the kernel to */
	cl_device_id     *device_id;           /**< Id of the device to execute the kernel*/
	cl_event         *p_last_kernel_event; /**< Event to record this kernel into */
	int               n_arguments;         /**< Number of arguments passed to the user defined kernel */
	char             *p_roles;             /**< List of roles of the arguments passed to the kernel */
	uint16_t         *p_displacements;     /**< Displacement of parameter over arguments array */
	int               type_id;             /**< Id of the ctrl respect to other FPGA ctrls */
} Ctrl_FPGA_Request;
///@endcond
#endif // _CTRL_FPGA_REQUEST_H_
