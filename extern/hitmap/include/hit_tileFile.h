/**
 * Hierarchical tiling of multi-dimensional dense and sparse structures.
 * Reading/Writing tiles using the MPI File functionalities
 *
 * @file hit_tileFile.h
 * @ingroup Tiling
 * @version 1.8
 * @author Arturo Gonzalez-Escribano
 * @date Mar 2013, May 2018, Aug 2023
 */

/*
 * <license>
 * 
 * Hitmap v1.4
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
 * Copyright (c) 2007-2024, Trasgo Group, Universidad de Valladolid.
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
 * Constant for chosing an option at runtime with environment variables or dynamic information
 * @hideinitializer
 */
#define HIT_FILE_RUNTIME	-100

/**
 * Constant for read operation
 * @hideinitializer
 */
#define HIT_FILE_READ		0

/**
 * Constant for write operation
 * @hideinitializer
 */
#define HIT_FILE_WRITE		1

/* MODES FOR POSITION OF THE TILES IN THE FILE: COORDINATE SYSTEM */
/** 
 * Constant for read/write file operations in array files (with array coordinates)
 * @hideinitializer
 */
#define HIT_FILE_ARRAY		0
/** 
 * Constant for read/write file operations in tile files (with tile coordinates)
 * @hideinitializer
 */
#define HIT_FILE_TILE		1
/** 
 * Constant for read/write operations from/to a user selected tile file
 * @hideinitializer
 */
#define HIT_FILE_TILE_SELECT	2

/* MODES FOR BINARY/TEXT FILES */
/**
 * Constant for read/write operations on binary files
 * @hideinitializer
 */
#define HIT_FILE_BINARY		0
/**
 * Constant for read/write operations on text files
 * @hideinitializer
 */
#define HIT_FILE_TEXT		1

/**
 * Constant for files with no header, only data
 * @hideinitializer
 */
#define HIT_FILE_NO_HEADER	0
/**
 * Constant for files with meta-data header before data
 * @hideinitializer
 */
#define HIT_FILE_HEADER		1

/**
 * Constant for unknown type in binary files, data is red/written as type-size bytes
 * @hideinitializer
 */
#define HIT_FILE_TYPE_UNKNOWN	-1
/** 
 * Constant for read/write file operations with characters 
 * @hideinitializer
 */
#define HIT_FILE_CHAR		0
/** 
 * Constant for read/write file operations with short integer numbers.
 * @hideinitializer
 */
#define HIT_FILE_SHORT		1
/** 
 * Constant for read/write file operations with integer numbers.
 * @hideinitializer
 */
#define HIT_FILE_INT		2
/** 
 * Constant for read/write file operations with long integer numbers.
 * @hideinitializer
 */
#define HIT_FILE_LONG		3
/** 
 * Constant for read/write file operations with float numbers
 * @hideinitializer
 */
#define HIT_FILE_FLOAT		4
/** 
 * Constant for read/write file operations with double numbers
 * @hideinitializer
 */
#define HIT_FILE_DOUBLE		5



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
	hit_tileFileInternal( var, HIT_FILE_BINARY, HIT_FILE_WRITE, coord, HIT_FILE_RUNTIME, HIT_FILE_TYPE_UNKNOWN, 0, 0, HIT_FILE_RUNTIME, file, NULL, #var, __FILE__, __LINE__)

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
	hit_tileFileInternal( var, HIT_FILE_BINARY, HIT_FILE_READ, coord, HIT_FILE_RUNTIME, HIT_FILE_TYPE_UNKNOWN, 0, 0, HIT_FILE_RUNTIME, file, NULL, #var, __FILE__, __LINE__)


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
 * 						\arg \c HIT_FILE_CHAR	For char elements.
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
	hit_tileFileInternal( var, HIT_FILE_TEXT, HIT_FILE_WRITE, coord, HIT_FILE_RUNTIME, datatype, s1, s2, HIT_FILE_RUNTIME, file, NULL, #var, __FILE__, __LINE__)

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
 * 						\arg \c HIT_FILE_CHAR	For char elements.
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
	hit_tileFileInternal( var, HIT_FILE_TEXT, HIT_FILE_READ, coord, HIT_FILE_RUNTIME, datatype, s1, s2, HIT_FILE_RUNTIME, file, NULL, #var, __FILE__, __LINE__)


/**
 * Write the data elements of the array tile to a file with chosen options.
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
 * @param[in] file	\e char*	Base name of the file.
 * @param[in] format	Flag for binary or text formats.
 * 				\arg \c HIT_FILE_BINARY The elements are written in binary format
 * 				\arg \c HIT_FILE_TEXT The elements are written as text (see formatSizes below)
 * 				\arg \c HIT_FILE_RUNTIME The format is chosen at runtime with an evironment variable
 * @param[in] coord	Flag for the coordinate system to use.
* 				\arg \c HIT_FILE_TILE The elements are written to the file in
 * 					row-major order in tile coordinates. 
 * 				\arg \c HIT_FILE_ARRAY The elements are written to the file in
 * 					row-major order in array coordinates. If the program
 * 					is executed in parallel, with a shared file-system, 
 * 					and each process has a partition of the array, this
 * 					option produces a single file with the whole array written
 * 					in parallel.
 * 				\arg \c HIT_FILE_RUNTIME The coordinate system is chosen at runtime with an evironment variable
 * @param[in] header	Flag to read/write a meta-data header
 * 				\arg \c HIT_FILE_NO_HEADER Do not read/write metadata
 * 				\arg \c HIT_FILE_HEADER Read/write metadata
 * 				\arg \c HIT_FILE_RUNTIME The use of metadata is chosen at runtime with an evironment variable
 * @param[in] datatype	Flag for the type of elements. The following ones are supported:
 * 						\arg \c HIT_FILE_CHAR	For char elements.
 * 						\arg \c HIT_FILE_INT	For int elements.
 * 						\arg \c HIT_FILE_LONG	For long int elements.
 * 						\arg \c	HIT_FILE_FLOAT	For float elements.
 * 						\arg \c	HIT_FILE_DOUBLE	For double elements.
 * 						\arg \c	HIT_FILE_TYPE_UNKNOWN	Only for binary format, data is sequentialized and read/write as a sequence of bytes
 * @param[in] formatSize1	For text files: Number of characters for each data item
 * @param[in] formatSize2	For text files with floating point data: Number of decimals
 *
 * @retval	int		 True if the variable has assigned memory (allocated, or selection chain 
 * 						of an allocated variable). False otherwise.
 */
#define hit_tileFileWriteOptions( var, fileNamePrefix, fileNameSuffix, fileRank, format, coord, header, datatype, formatSize1, formatSize2 )    \
	hit_tileFileInternal( var, format, HIT_FILE_WRITE, coord, header, datatype, formatSize1, formatSize2, fileRank, fileNamePrefix, fileNameSuffix, #var, __FILE__, __LINE__)

/**
 * Read the data elements of the array tile from a file with chosen options.
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
 * @param[in] file	\e char*	Base name of the file.
 * @param[in] format	Flag for binary or text formats.
 * 				\arg \c HIT_FILE_BINARY The elements are red in binary format
 * 				\arg \c HIT_FILE_TEXT The elements are red as text (see formatSizes below)
 * 				\arg \c HIT_FILE_RUNTIME The format is chosen at runtime with an evironment variable
 * @param[in] coord	Flag for the coordinate system to use.
* 				\arg \c HIT_FILE_TILE The elements are red from the file in
 * 					row-major order in tile coordinates. 
 * 				\arg \c HIT_FILE_ARRAY The elements are red from the file in
 * 					row-major order in array coordinates. If the program
 * 					is executed in parallel, with a shared file-system, 
 * 					and each process has a partition of the array, this
 * 					option produces a single file with the whole array written
 * 					in parallel.
 * 				\arg \c HIT_FILE_RUNTIME The coordinate system is chosen at runtime with an evironment variable
 * @param[in] header	Flag to read/write a meta-data header
 * 				\arg \c HIT_FILE_NO_HEADER Do not read/write metadata
 * 				\arg \c HIT_FILE_HEADER Read/write metadata
 * 				\arg \c HIT_FILE_RUNTIME The use of metadata is chosen at runtime with an evironment variable
 * @param[in] datatype	Flag for the type of elements. The following ones are supported:
 * 						\arg \c HIT_FILE_CHAR	For char elements.
 * 						\arg \c HIT_FILE_INT	For int elements.
 * 						\arg \c HIT_FILE_LONG	For long int elements.
 * 						\arg \c	HIT_FILE_FLOAT	For float elements.
 * 						\arg \c	HIT_FILE_DOUBLE	For double elements.
 * 						\arg \c	HIT_FILE_TYPE_UNKNOWN	Only for binary format, data is sequentialized and read/write as a sequence of bytes
 * @param[in] formatSize1	For text files: Number of characters for each data item
 * @param[in] formatSize2	For text files with floating point data: Number of decimals
 *
 * @retval	int		 True if the variable has assigned memory (allocated, or selection chain 
 * 						of an allocated variable). False otherwise.
 */
#define hit_tileFileReadOptions( var, fileNamePrefix, fileNameSuffix, fileRank, format, coord, header, datatype, formatSize1, formatSize2 )    \
	hit_tileFileInternal( var, format, HIT_FILE_READ, coord, header, datatype, formatSize1, formatSize2, fileRank, fileNamePrefix, fileNameSuffix, #var, __FILE__, __LINE__)

/**
 * Print usage of environment variables to control file options
 */
void hit_filePrintUsage();

/**@}*/

/* File IO operations: Internal declarations and functions */
typedef struct {
	size_t header_size;
	int fileFormat;
	int tileMode;
	int type;
	size_t typeSize;
	int formatSize1;
	int formatSize2;
	int rank;
	int nprocs;
	int num_dims;
	int cards[ HIT_MAXDIMS ];
	HitShape tileShape;
} HitTile_FileHeaderInfo;

HitTile_FileHeaderInfo hit_tileFileReadHeader( const char *fileName );
HitTile_FileHeaderInfo hit_tileFileStreamReadHeader( FILE *file, const char *fileName );
size_t hit_tileFileWriteHeader( const char *fileName, HitTile_FileHeaderInfo );
size_t hit_tileFileStreamWriteHeader( FILE *file, HitTile_FileHeaderInfo );

int hit_tileFileInternal( void * varP, int fileFormat, int fileMode, int tileMode, int fileHeader, int type, int formatSize1, int formatSize2, int fileRank, const char *fileNamePrefix, const char *fileNameSuffix, const char *debugVarName, const char *debugCodeFile, int debugCodeLine);


#ifdef __cplusplus
	}
#endif

/* END OF HEADER FILE _HitTile_ */
#endif
