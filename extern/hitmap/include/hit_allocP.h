/**
 * @ingroup Dev
 * @brief Hitmap functions to allocate memory.
 *
 * This is a set of macro-functions that allocate memory and check the result. 
 * They print an error message if the allocate routine fails. 
 * This is an interface for Hitmap developers.
 *
 * These macros include a cast of the result pointers for compatibility with C++.
 * In C langauge the cast is not needed because the stdlib malloc function should return
 * a void pointer.
 *
 * @file hit_allocP.h
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @date Aug 2015
 * 
 */

/*
 * <license>
 * 
 * Hitmap v1.3
 * 
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Copyright (c) 2007-2021, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#ifndef _HitAllocP_
#define _HitAllocP_


#include <stdlib.h>
#include "hit_error.h"


/**
 * Allocate memory.
 *
 * It allocates a memory block of size bytes, without a specific type.
 *
 * @version 1.0
 * @author Arturo Gonzalez-Escribano
 * 
 * @param[out] ptr  Pointer to the allocated memory.
 * @param[in]  size Total size of the allocated memory.
 *
*/
#define hit_vmalloc(ptr, size) \
{ \
	ptr = (void *)malloc((size_t)(size)); \
	if(ptr == NULL) hit_errInternal(__func__,"Memory allocation request failed","",__FILE__,__LINE__) \
}

/**
 * Allocate an array of elements of a given type.
 *
 * It allocates a memory block of nmemb elements of the chosen type. It does not initialize
 * the elements to a any predefined value.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * 
 * @param[out] ptr   Pointer to the allocated memory.
 * @param[in]  type  Type of the elements of the array.
 * @param[in]  nmemb Number of elements to allocate.
 *
*/
#define hit_malloc(ptr, type, nmemb) \
{ \
	ptr = (type *)malloc(sizeof(type)* (size_t)(nmemb)); \
	if(ptr == NULL) hit_errInternal(__func__,"Memory allocation request failed","",__FILE__,__LINE__) \
}


/**
 * Allocate and initialize to zero an array of elements of a given type.
 *
 * It allocates a memory block of nmemb elements of the chosen type. It initializes
 * the elements using the calloc(3) function.
 * 
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * 
 * @param[out] ptr   Pointer to the allocated memory.
 * @param[in]  type  Type of the elements of the array.
 * @param[in]  nmemb Number of elements to allocate.
 */
#define hit_calloc(ptr, type, nmemb) \
{ \
	ptr = (type *)calloc( (size_t)(nmemb), sizeof(type)); \
	if(ptr == NULL) hit_errInternal(__func__,"Memory allocation request failed","",__FILE__,__LINE__) \
}

/**
 * Change the number of elements of an array.
 *
 * It changes the number of elements of an allocated array of a given type, using the
 * reallocate(3) function. The size change may imply a reallocation of the whole array.
 *
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * 
 * @param[out] ptr   Pointer to the reallocated memory.
 * @param[in]  type  Type of the elements of the array.
 * @param[in]  nmemb New number of elements in the array.
 */
#define hit_realloc(ptr, type, nmemb) \
{ \
	ptr = (type *)realloc(ptr, sizeof(type)* (size_t)(nmemb)); \
	if(ptr == NULL) hit_errInternal(__func__,"Memory rellocation request failed","",__FILE__,__LINE__) \
}


/**
 * Free a previous allocated block of memory.
 *
 * This function frees a block of memory previously allocated and pointed by ptr.
 *
 * @version 2.0
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * 
 * @param[in,out] ptr Pointer to the allocated memory.
 */
#define hit_free(ptr) \
{ \
	free(ptr); \
	ptr = NULL; \
}



/* END OF HEADER FILE _HitAllocP_ */
#endif
