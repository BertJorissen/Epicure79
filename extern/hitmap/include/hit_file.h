/**
 *
 * Types and functions to work with files.
 *
 * @file hit_file.h
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @date Jul 2011
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

#ifndef _HitFile_
#define _HitFile_

#include <stdarg.h>
#include "hit_shape.h"
#include "hit_tile.h"
#include "hit_error.h"

#ifndef INCLUDE_IOHB__H
	#define INCLUDE_IOHB__H
    #include "iohb.h"
#endif

#ifndef INCLUDE_MATRIX_IO__H
	#define INCLUDE_MATRIX_IO__H
	#include "matrix_io.h"
#endif

#ifndef INCLUDE_METIS__H
    #define INCLUDE_METIS__H
    #include "metis.h"
#endif



/**
 * Reads the structure of an Harwell-Boeing graph.
 * @fn HitShape hit_fileHBRead(const char * hbfile)
 * @note Only the root processor (hit_Rank == 0) read the graph.
 * @memberof HitSparseShape
 * @param hbfile HB file path.
 * @return A new sparse HitShape.
 * @note Returns a CSR shape.
 */
 #define hit_fileHBRead(hbfile)       hit_fileHBRead_toCSR_Internal((hbfile), 1, (hit_Rank), __FILE__, __LINE__)


/**
 * Reads the structure of an Harwell-Boeing matrix.
 * @fn HitShape hit_fileHBMatrixRead(const char * hbfile)
 * @note Only the root processor (hit_Rank == 0) read the graph.
 * @memberof HitSparseShape
 * @param hbfile HB file path.
 * @return A new sparse HitShape.
 * @note Returns a CSR shape.
 */
 #define hit_fileHBMatrixRead(hbfile) hit_fileHBRead_toCSR_Internal((hbfile), 0, (hit_Rank), __FILE__, __LINE__)




/**
 * Reads the structure of an Harwell-Boeing graph or matrix.
 * @note Only the root processor (hit_Rank == 0) read the graph.
 * @memberof HitSparseShape
 * @param fileName HB file path.
 * @param create_graph Graph or matrix mode.
 * @param rank Processor rank.
 * @param file
 * @param line
 * @return A new sparse HitCShape.
 */
HitShape hit_fileHBRead_toCSR_Internal(const char *fileName, int create_graph, int rank,
		                          const char * file, int line);



#define hit_fileMMMatrixRead(hbfile) hit_fileMMRead_toCSR_Internal((hbfile), 0, (hit_Rank), __FILE__, __LINE__)
HitShape hit_fileMMRead_toCSR_Internal(const char *fileName, int create_graph, int rank,
		                          const char * file, int line);
/**
 * Reads the structure of an Harwell-Boeing graph or matrix.
 * @note Only the root processor (hit_Rank == 0) read the graph.
 * @memberof HitSparseShape
 * @param fileName HB file path.
 * @param create_graph Graph or matrix mode.
 * @param rank Processor rank.
 * @param file
 * @param line
 * @return A new sparse HitBShape.
 */
HitShape hit_fileHBRead_toBitmap_Internal(const char *fileName, int create_graph, int rank,
        const char * file, int line);



/**
 * Reads the structure of an Harwell-Boeing graph.
 * @fn HitShape hit_fileHBRead(const char * hbfile)
 * @note Only the root processor (hit_Rank == 0) read the graph.
 * @memberof HitSparseShape
 * @param hbfile HB file path.
 * @return A new sparse HitShape.
 * @note Returns a Bitmap shape.
 */
#define hit_fileHBReadBitmap(hbfile) hit_fileHBRead_toBitmap_Internal((hbfile), 1, (hit_Rank), __FILE__, __LINE__)

/**
 * Reads the structure of an Harwell-Boeing matrix.
 * @fn HitShape hit_fileHBRead(const char * hbfile)
 * @note Only the root processor (hit_Rank == 0) read the graph.
 * @memberof HitSparseShape
 * @param hbfile HB file path.
 * @return A new sparse HitShape.
 * @note Returns a Bitmap shape.
 */
#define hit_fileHBMatrixReadBitmap(hbfile) hit_fileHBRead_toBitmap_Internal((hbfile), 0, (hit_Rank), __FILE__, __LINE__)

/**
 * Reads the number of vertices from a Harwell-Boeing graph file.
 * @fn HitShape hit_fileHBVerticesl(const char * hbfile)
 * @param hbfile The path of the file.
 * @return The number of vertices of the sparse file.
 */
#define hit_fileHBVertices(hbfile) hit_fileHBVerticesInternal((hbfile),(hit_Rank),__FILE__,__LINE__)

/**
 * Reads the number of vertices from a Harwell-Boeing graph file.
 * @internal
 * @param hbfile The path of the file.
 * @param rank The rank of the processor.
 * @param file The file of code.
 * @param line The number of line of the code.
 * @return The number of vertices of the sparse file.
 */
int hit_fileHBVerticesInternal(const char * hbfile, int rank, const char * file, int line);


/**
 * Reads a Harwell-Boeing graph and writes it in a square adjancency matrix.
 * @fn int hit_fileHBReadDense(const char * hbfile, void * tileP)
 *
 * @note Needs a HitTile of integers.
 *
 * @param hbfile The path of the file.
 * @param rank The rank of the processor.
 * @param tileP The tile to store the graph.
 * @param file The file of code.
 * @param line The number of line of the code.
 */
#define hit_fileHBReadDense(hbfile,tileP) hit_fileHBReadDenseInternal((hbfile),(hit_Rank),(tileP),__FILE__,__LINE__)

/**
 * Reads a Harwell-Boeing graph and writes it in a square adjancency matrix.
 * @note Needs a HitTile of integers.
 * @internal
 * @param hbfile The path of the file.
 * @param rank The rank of the processor.
 * @param tileP The tile to store the graph.
 * @param file The file of code.
 * @param line The number of line of the code.
 */
int hit_fileHBReadDenseInternal(const char * hbfile, int rank, void * tileP, const char * file, int line);


/**
 * Output function for CSR and Bitmap shapes to HB.
 * @note http://people.sc.fsu.edu/~jburkardt/data/hb/hb.html
 * @todo @javfres Add another output function for Dense.
 */
void hit_fileHBWriteInternal(const char * hbfile, HitShape shape, int rank, const char * file, int line);
#define hit_fileHBWrite(hbfile,shape) hit_fileHBWriteInternal((hbfile),(shape),(hit_Rank),__FILE__,__LINE__)
void hit_fileHBWriteBitmapInternal(const char * hbfile, HitShape shape, int rank, const char * file, int line);
#define hit_fileHBWriteBitmap(hbfile,shape) hit_fileHBWriteBitmapInternal((hbfile),(shape),(hit_Rank),__FILE__,__LINE__)


/**
 * @todo @javfres Adapt the CSR input/output functions to the new sparse structure.
 * The new sparse structure support bipartite graphs with two different vertex lists.
 * This avoids coping all the vertices for the source and target of the edge.
 * This functions do not consider this change. In fact, HB format do not support it neither
 * so a conversion is needed.
 */
HitShape hit_fileCSRReadInternal(const char * csrfile, int rank, const char * cfile, int line);
void hit_fileCSRWriteInternal(const char * csrfile, HitShape shape, int rank, const char * cfile, int line);
#define hit_fileCSRRead(csrfile) hit_fileCSRReadInternal((csrfile),(hit_Rank),__FILE__,__LINE__)
#define hit_fileCSRWrite(csrfile,shape) hit_fileCSRWriteInternal((csrfile),(shape),(hit_Rank),__FILE__,__LINE__)




/* END OF HEADER FILE _HitFile_ */
#endif
