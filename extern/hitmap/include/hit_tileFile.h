/**
 * Hierarchical tiling of multi-dimensional dense and sparse structures.
 * Reading/Writing tiles using the MPI File functionalities
 *
 * @file hit_tileFile.h
 * @ingroup Tiling
 * @version 1.7
 * @author Arturo Gonzalez-Escribano
 * @date Mar 2013, May 2018
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

#ifndef _HitTileFile_
#define _HitTileFile_

#include "hit_shape.h"
#include "hit_sshape.h"
#include "hit_tile.h"
#include "hit_tileP.h"
#include "hit_mpi.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @name Input/Output for dense array tiles.
 */
/**@{
 * @ingroup FileOps
 */

/**
 * Write the data elements of the array tile to a binary file.
 *
 * The variable should have been allocated, or it must be a selection of an allocated variable.
 * The ancestor array coordinates should start at 0, and should not have stride.
 *
 * The file is completely rewritten. Only the elements in the
 * domain space of the tile are overwritten. The rest of the file positions are not initialized.
 *
 * @hideinitializer
 * 
 * @param[in] var	\e HitTile* A pointer to a HitTile derived type variable.
 * @param[in] file	\e char*	Name of the file.
 * @param[in] coord	Flag for the coordinate system to use.
 * 						\arg \c HIT_FILE_TILE The elements are written on the file in
 * 								row-major order in tile coordinates. 
 * 						\arg \c HIT_FILE_ARRAY The elements are written on the file in
 * 								row-major order in array coordinates. If the program
 * 								is executed in parallel, with a shared file-system, 
 * 								and each process has a partition of the array, this
 * 								option produces a single file with the whole array written
 * 								in parallel.
 *
 * @retval	int		 True if the variable has assigned memory (allocated, or selection chain 
 * 						of an allocated variable). False otherwise.
 */
#define hit_tileFileWrite( var, file, coord )    \
	hit_tileFileInternal( var, HIT_FILE_BINARY, HIT_FILE_WRITE, coord, HIT_FILE_UNKNOWN, 0, 0, file, #var, __FILE__, __LINE__)

/**
 * Read the data elements of the array tile from a binary file.
 *
 * The variable should have been allocated, or it must be a selection of an allocated variable.
 * The ancestor array coordinates should start at 0, and should not have stride.
 *
 * @hideinitializer
 * 
 * @param[in,out] var	\e HitTile* A pointer to a HitTile derived type variable.
 * @param[in] file	\e char*	Name of the file.
 * @param[in] coord	Flag for the coordinate system to use.
 * 						\arg \c HIT_FILE_TILE The elements are read from the file in
 * 								row-major order in tile coordinates. 
 * 						\arg \c HIT_FILE_ARRAY The elements are read from the file in
 * 								row-major order in array coordinates. If the program
 * 								is executed in parallel, with a shared file-system or
 * 								with the same file copied on each file system, 
 * 								and each process has a partition of the array, this
 * 								option allows to read whole distributed array
 * 								in parallel.
 *
 * @retval	int		 True if the variable has assigned memory (allocated, or selection chain 
 * 						of an allocated variable). False otherwise.
 */
#define hit_tileFileRead( var, file, coord )    \
	hit_tileFileInternal( var, HIT_FILE_BINARY, HIT_FILE_READ, coord, HIT_FILE_UNKNOWN, 0, 0, file, #var, __FILE__, __LINE__)


/**
 * Write the data elements of the array tile to a text file.
 *
 * The variable should have been allocated, or it must be a selection of an allocated variable.
 * The ancestor array coordinates should start at 0, and should not have stride.
 *
 * The file is completely rewritten. Only the elements in the
 * domain space of the tile are overwritten. The rest of the file positions are not initialized.
 *
 * @hideinitializer
 * 
 * @param[in] var	\e HitTile* A pointer to a HitTile derived type variable.
 * @param[in] file	\e char*	Name of the file.
 * @param[in] coord	Flag for the coordinate system to use.
 * 						\arg \c HIT_FILE_TILE The elements are written on the file in
 * 								row-major order in tile coordinates. 
 * 						\arg \c HIT_FILE_ARRAY The elements are written on the file in
 * 								row-major order in array coordinates. If the program
 * 								is executed in parallel, with a shared file-system, 
 * 								and each process has a partition of the array, this
 * 								option produces a single file with the whole array written
 * 								in parallel.
 * @param[in] datatype	Flag for the type of elements. The following ones are supported:
 * 						\arg \c HIT_FILE_INT	For int elements.
 * 						\arg \c HIT_FILE_LONG	For long int elements.
 * 						\arg \c	HIT_FILE_FLOAT	For float elements.
 * 						\arg \c	HIT_FILE_DOUBLE	For double elements.
 * @param[in] s1	\e int	Width for the format of the numbers
 * @param[in] s2	\e int	Precision for the format of the numbers (ignored for integers or longs)
 *
 * @retval	int		 True if the variable has assigned memory (allocated, or selection chain 
 * 						of an allocated variable). False otherwise.
 */
#define hit_tileTextFileWrite( var, file, coord, datatype, s1, s2 )    \
	hit_tileFileInternal( var, HIT_FILE_TEXT, HIT_FILE_WRITE, coord, datatype, s1, s2, file, #var, __FILE__, __LINE__)

/**
 * Read the data elements of the array tile from a text file.
 *
 * The variable should have been allocated, or it must be a selection of an allocated variable.
 * The ancestor array coordinates should start at 0, and should not have stride.
 *
 * @hideinitializer
 * 
 * @param[in] var	\e HitTile* A pointer to a HitTile derived type variable.
 * @param[in] file	\e char*	Name of the file.
 * @param[in] coord	Flag for the coordinate system to use.
 * 						\arg \c HIT_FILE_TILE The elements are read from the file in
 * 								row-major order in tile coordinates. 
 * 						\arg \c HIT_FILE_ARRAY The elements are read from the file in
 * 								row-major order in array coordinates. If the program
 * 								is executed in parallel, with a shared file-system or
 * 								with the same file copied on each file system, 
 * 								and each process has a partition of the array, this
 * 								option allows to read whole distributed array
 * 								in parallel.
 * @param[in] datatype	Flag for the type of elements. The following ones are supported:
 * 						\arg \c HIT_FILE_INT	For int elements.
 * 						\arg \c HIT_FILE_LONG	For long int elements.
 * 						\arg \c	HIT_FILE_FLOAT	For float elements.
 * 						\arg \c	HIT_FILE_DOUBLE	For double elements.
 * @param[in] s1	\e int	Width for the format of the numbers
 * @param[in] s2	\e int	Precision for the format of the numbers (ignored for integers or longs)
 *
 * @retval	int		 True if the variable has assigned memory (allocated, or selection chain 
 * 						of an allocated variable). False otherwise.
 */
#define hit_tileTextFileRead( var, file, coord, datatype, s1, s2 )    \
	hit_tileFileInternal( var, HIT_FILE_TEXT, HIT_FILE_READ, coord, datatype, s1, s2, file, #var, __FILE__, __LINE__)

/**@}*/

#ifdef __cplusplus
	}
#endif

/* END OF HEADER FILE _HitTile_ */
#endif
