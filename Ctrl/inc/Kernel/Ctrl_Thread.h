#ifndef _CTRL_KERNEL_THREAD_H_
#define _CTRL_KERNEL_THREAD_H_
/**
 * @file Ctrl_Thread.h
 * @author Trasgo Group
 * @brief Thread block structs and macros.
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

/**
 * @brief Stores the absolute identifier of the thread in each dimension. 
 */
typedef struct {
	unsigned int dims;	/**< Number of dims for this thread */
	unsigned int x;		/**< Identifier of this thread in the 1st */
	unsigned int y;		/**< Identifier of this thread in the 2nd */
	unsigned int z;		/**< Identifier of this thread in the 3rd */
} Ctrl_Thread;

#define CTRL_THREAD_STRINGIFY " typedef struct { unsigned int dims; unsigned int x; unsigned int y; unsigned int z; } Ctrl_Thread; "

/**
 * Null value for \e Ctrl_Thread
 * @hideinitializer
 */
#define CTRL_THREAD_NULL ((Ctrl_Thread){ .dims = 0, .x = 0, .y = 0, .z = 0 })

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
#define Ctrl_ThreadInit( name, ... ) \
		Ctrl_ThreadInitN( name, CTRL_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__)

#define Ctrl_ThreadInitN( name, dims, ...) Ctrl_ThreadInitN2( name, dims, __VA_ARGS__ )

#define Ctrl_ThreadInitN2(threads, ndims, ... ) Ctrl_Thread##ndims(threads, ndims, __VA_ARGS__ )

#define Ctrl_Thread1(threads, ndims, param1) Ctrl_Thread2(threads, ndims, param1, 1)
#define Ctrl_Thread2(threads, ndims, param1, param2) Ctrl_Thread3(threads, ndims, param1, param2, 1)
#define Ctrl_Thread3(threads, ndims, param1, param2, param3) threads.dims = ndims; threads.x = param1; threads.y = param2; threads.z = param3;

#endif // _CTRL_KERNEL_THREAD_H_
