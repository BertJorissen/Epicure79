#ifndef _CTRL_FPGA_HELPER_H_
#define _CTRL_FPGA_HELPER_H_
///@cond INTERNAL
/**
 * @file Ctrl_FPGA_Helper.h
 * @author Trasgo Group
 * @brief Macros for error checking on OpenCL operations when debugging.
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

// @sergioalo formatter does not respect ifdef indentation so leaving it off in this file
// clang-format off
#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
	#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif // CL_USE_DEPRECATED_OPENCL_1_2_APIS

#ifdef _CTRL_OPENCL_GPU_PROFILING_
	#include <assert.h>
	#include <stdio.h>
#endif

#ifdef _CTRL_OPENCL_GPU_ERROR_CHECK_
	#include <assert.h>
	#include <stdio.h>
	
	/********************************************
	 ***** OpenCL FPGA util functions ******
	********************************************/

	#define CASE_RETURN_STRING(err) \
		case err:                   \
			return #err;
	static inline const char *clGetError(int error) {
		switch (error) {
			CASE_RETURN_STRING(CL_SUCCESS)
			CASE_RETURN_STRING(CL_DEVICE_NOT_FOUND)
			CASE_RETURN_STRING(CL_DEVICE_NOT_AVAILABLE)
			CASE_RETURN_STRING(CL_COMPILER_NOT_AVAILABLE)
			CASE_RETURN_STRING(CL_MEM_OBJECT_ALLOCATION_FAILURE)
			CASE_RETURN_STRING(CL_OUT_OF_RESOURCES)
			CASE_RETURN_STRING(CL_OUT_OF_HOST_MEMORY)
			CASE_RETURN_STRING(CL_PROFILING_INFO_NOT_AVAILABLE)
			CASE_RETURN_STRING(CL_MEM_COPY_OVERLAP)
			CASE_RETURN_STRING(CL_IMAGE_FORMAT_MISMATCH)
			CASE_RETURN_STRING(CL_IMAGE_FORMAT_NOT_SUPPORTED)
			CASE_RETURN_STRING(CL_BUILD_PROGRAM_FAILURE)
			CASE_RETURN_STRING(CL_MAP_FAILURE)
			CASE_RETURN_STRING(CL_MISALIGNED_SUB_BUFFER_OFFSET)
			CASE_RETURN_STRING(CL_COMPILE_PROGRAM_FAILURE)
			CASE_RETURN_STRING(CL_LINKER_NOT_AVAILABLE)
			CASE_RETURN_STRING(CL_LINK_PROGRAM_FAILURE)
			CASE_RETURN_STRING(CL_DEVICE_PARTITION_FAILED)
			CASE_RETURN_STRING(CL_KERNEL_ARG_INFO_NOT_AVAILABLE)
			CASE_RETURN_STRING(CL_INVALID_VALUE)
			CASE_RETURN_STRING(CL_INVALID_DEVICE_TYPE)
			CASE_RETURN_STRING(CL_INVALID_PLATFORM)
			CASE_RETURN_STRING(CL_INVALID_DEVICE)
			CASE_RETURN_STRING(CL_INVALID_CONTEXT)
			CASE_RETURN_STRING(CL_INVALID_QUEUE_PROPERTIES)
			CASE_RETURN_STRING(CL_INVALID_COMMAND_QUEUE)
			CASE_RETURN_STRING(CL_INVALID_HOST_PTR)
			CASE_RETURN_STRING(CL_INVALID_MEM_OBJECT)
			CASE_RETURN_STRING(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
			CASE_RETURN_STRING(CL_INVALID_IMAGE_SIZE)
			CASE_RETURN_STRING(CL_INVALID_SAMPLER)
			CASE_RETURN_STRING(CL_INVALID_BINARY)
			CASE_RETURN_STRING(CL_INVALID_BUILD_OPTIONS)
			CASE_RETURN_STRING(CL_INVALID_PROGRAM)
			CASE_RETURN_STRING(CL_INVALID_PROGRAM_EXECUTABLE)
			CASE_RETURN_STRING(CL_INVALID_KERNEL_NAME)
			CASE_RETURN_STRING(CL_INVALID_KERNEL_DEFINITION)
			CASE_RETURN_STRING(CL_INVALID_KERNEL)
			CASE_RETURN_STRING(CL_INVALID_ARG_INDEX)
			CASE_RETURN_STRING(CL_INVALID_ARG_VALUE)
			CASE_RETURN_STRING(CL_INVALID_ARG_SIZE)
			CASE_RETURN_STRING(CL_INVALID_KERNEL_ARGS)
			CASE_RETURN_STRING(CL_INVALID_WORK_DIMENSION)
			CASE_RETURN_STRING(CL_INVALID_WORK_GROUP_SIZE)
			CASE_RETURN_STRING(CL_INVALID_WORK_ITEM_SIZE)
			CASE_RETURN_STRING(CL_INVALID_GLOBAL_OFFSET)
			CASE_RETURN_STRING(CL_INVALID_EVENT_WAIT_LIST)
			CASE_RETURN_STRING(CL_INVALID_EVENT)
			CASE_RETURN_STRING(CL_INVALID_OPERATION)
			CASE_RETURN_STRING(CL_INVALID_GL_OBJECT)
			CASE_RETURN_STRING(CL_INVALID_BUFFER_SIZE)
			CASE_RETURN_STRING(CL_INVALID_MIP_LEVEL)
			CASE_RETURN_STRING(CL_INVALID_GLOBAL_WORK_SIZE)
			CASE_RETURN_STRING(CL_INVALID_PROPERTY)
			CASE_RETURN_STRING(CL_INVALID_IMAGE_DESCRIPTOR)
			CASE_RETURN_STRING(CL_INVALID_COMPILER_OPTIONS)
			CASE_RETURN_STRING(CL_INVALID_LINKER_OPTIONS)
			CASE_RETURN_STRING(CL_INVALID_DEVICE_PARTITION_COUNT)
			default:
				return "Unknown OpenCL error code";
		}
	}
	#undef CASE_RETURN_STRING

	#define OPENCL_ASSERT_OP( operation ) \
		{ \
			int aux_err = operation; \
			if (aux_err != CL_SUCCESS) fprintf(stderr, "[%s:%d] OpenCL error: %d (%s)\n", __FILE__, __LINE__, aux_err, clGetError(aux_err)); fflush(stderr); \
			assert(aux_err == CL_SUCCESS); \
		}

	#define OPENCL_ASSERT_ERROR( err ) \
		if (err != CL_SUCCESS) fprintf(stderr, "[%s:%d] OpenCL error: %d (%s)\n", __FILE__, __LINE__, err, clGetError(err)); fflush(stderr); \
		assert(err == CL_SUCCESS);
#else
	#define OPENCL_ASSERT_OP( operation ) operation ;
	#define OPENCL_ASSERT_ERROR( ... ) ;
#endif

#define CTRL_OPENCL_PROFILE_OP_READ   1
#define CTRL_OPENCL_PROFILE_OP_WRITE  2
#define CTRL_OPENCL_PROFILE_OP_KERNEL 3

#define CTRL_OPENCL_PROFILE_QUEUE_MAIN  0
#define CTRL_OPENCL_PROFILE_QUEUE_READ  1
#define CTRL_OPENCL_PROFILE_QUEUE_WRITE 2

#ifdef _CTRL_OPENCL_GPU_PROFILING_
	#define OPENCL_PROFILE_READ( event )	\
		p_ctrl->profiling_read_events[p_ctrl->i_read_task] = event; \
		OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->profiling_read_events[p_ctrl->i_read_task]) ); \
		p_ctrl->i_read_task++; \
		OPENCL_PROFILE_FIRST( event ); \
		OPENCL_PROFILE_LAST( event );

	#define OPENCL_PROFILE_WRITE( event )	\
		p_ctrl->profiling_write_events[p_ctrl->i_write_task] = event; \
		OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->profiling_write_events[p_ctrl->i_write_task]) ); \
		p_ctrl->i_write_task++; \
		OPENCL_PROFILE_FIRST( event ); \
		OPENCL_PROFILE_LAST( event );

	#define OPENCL_PROFILE_KERNEL( event ) \
		p_ctrl->profiling_kernel_events[p_ctrl->i_kernel_task] = event; \
		OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->profiling_kernel_events[p_ctrl->i_kernel_task]) ); \
		p_ctrl->i_kernel_task++; \
		OPENCL_PROFILE_FIRST( event ); \
		OPENCL_PROFILE_LAST( event );

	#define OPENCL_PROFILE_FIRST( event ) \
		if (!p_ctrl->has_first_profiling_event) { \
			p_ctrl->has_first_profiling_event = true; \
			p_ctrl->first_profiling_event = event; \
			OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->first_profiling_event) ); \
		}

	#define OPENCL_PROFILE_LAST( event ) \
		OPENCL_ASSERT_OP( clReleaseEvent(p_ctrl->last_profiling_event) ); \
		p_ctrl->last_profiling_event = event; \
		OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->last_profiling_event) );


	#ifdef _CTRL_OPENCL_GPU_PROFILING_VERBOSE_
		typedef struct {
			cl_event event;
			int queue;
			int	op;
		} visual_event;

		#define OPENCL_PROFILE_VISUAL( command_queue, visual_event, operation ) \
			p_ctrl->profiling_visual_events[p_ctrl->i_visual_task].event = visual_event; \
			p_ctrl->profiling_visual_events[p_ctrl->i_visual_task].queue = command_queue; \
			p_ctrl->profiling_visual_events[p_ctrl->i_visual_task].op = operation; \
			OPENCL_ASSERT_OP( clRetainEvent(p_ctrl->profiling_visual_events[p_ctrl->i_visual_task].event) ); \
			p_ctrl->i_visual_task++;	
	#else 
		#define OPENCL_PROFILE_VISUAL( queue, event, op ) 
	#endif
#else
	#define OPENCL_PROFILE_READ( ... )
	#define OPENCL_PROFILE_WRITE( ... )
	#define OPENCL_PROFILE_KERNEL( ... )
	#define OPENCL_PROFILE_FIRST( ... )
	#define OPENCL_PROFILE_LAST( ... )
	#define OPENCL_PROFILE_VISUAL( ... )
#endif
///@endcond 
#endif // _CTRL_FPGA_HELPER_H_
