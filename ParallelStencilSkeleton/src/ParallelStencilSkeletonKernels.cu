/*
* ParallelStencilSkeletonKernels.cu
* 	Ctrl example
* 	Stencil code: Any dimensions, stencil as a pattern of weights.
*   Kernel code.
*
* v1.0
* (c) 2019-2021, Arturo Gonzalez-Escribano, Yuri Torres de la Sierra, Manuel de Castro Caballero
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

#include "Ctrl_Kernels.h"

Ctrl_NewType( float );

CTRL_KERNEL_FUNCTION( updateCell_1dNC4, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy ) {
	int x = thread_id_x;
	hit( mat, x ) = ( 
		0.5 * ( hit( copy, x-2 ) + hit( copy, x+2 ) ) +
		hit( copy, x-1 ) + hit( copy, x+1 ) ) / 3; 

	CTRL_KERNEL_END(CUDA);
}

CTRL_KERNEL_FUNCTION( updateCell_1dC2, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy ) {
	int x = thread_id_x;
	hit( mat, x ) = ( 
		hit( copy, x-1 ) + hit( copy, x+1 ) ) / 2; 

	CTRL_KERNEL_END(CUDA);
}

/* A.1. CELL UPDATE 4-POINT */
CTRL_KERNEL_FUNCTION( updateCell_4, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy ) {
	int x = thread_id_x;
	int y = thread_id_y;

	hit( mat, x, y ) = ( 
		hit( copy, x-1, y ) +
		hit( copy, x+1, y ) +
		hit( copy, x, y-1 ) +
		hit( copy, x, y+1 ) ) / 4 ;
	
	CTRL_KERNEL_END(CUDA);
}

/* A.2. CELL UPDATE 9-POINT */
CTRL_KERNEL_FUNCTION( updateCell_9, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy ) {
	int x = thread_id_x;
	int y = thread_id_y;
	hit( mat, x, y ) = ( 
		4 * ( hit( copy, x-1, y ) + hit( copy, x+1, y ) + hit( copy, x, y-1 ) + hit( copy, x, y+1 ) ) 
		+ ( hit( copy, x-1, y-1 ) + hit( copy, x+1, y-1 ) + hit( copy, x-1, y+1 ) + hit( copy, x+1, y+1 ) ) 
		) / 20 ;

	CTRL_KERNEL_END(CUDA);
}

/* A.3. CELL UPDATE 9-POINT, NON-COMPACT, NO CORNERS */
CTRL_KERNEL_FUNCTION( updateCell_NC9, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy ) {
	int x = thread_id_x;
	int y = thread_id_y;
	hit( mat, x, y ) = ( 
		( hit( copy, x-2, y ) + hit( copy, x+2, y ) + hit( copy, x, y-2 ) + hit( copy, x, y+2 ) ) 
		+ 4 * ( hit( copy, x-1, y ) + hit( copy, x+1, y ) + hit( copy, x, y-1 ) + hit( copy, x, y+1 ) ) 
		) / 20 ;

	CTRL_KERNEL_END(CUDA);
}

/* A.3. CELL UPDATE 5-POINT, FORWARD RIGHT-DOWN, ONE ELEMENT IN THE CORNER */
CTRL_KERNEL_FUNCTION( updateCell_F5, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy ) {
	int x = thread_id_x;
	int y = thread_id_y;
	hit( mat, x, y ) = ( 
		2.0f * ( hit( copy, x-1, y ) + hit( copy, x, y-1 ) ) 
		+ ( hit( copy, x-2, y ) + hit( copy, x, y-2 ) ) 
		+ .5f * hit( copy, x-1, y-1 ) 
		) / 6.5f;

	CTRL_KERNEL_END(CUDA);
}

/* A.2. CELL UPDATE 3D 27-POINT */
CTRL_KERNEL_FUNCTION( updateCell_3d27, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy ) {
	int x = thread_id_x;
	int y = thread_id_y;
	int z = thread_id_z;

	hit( mat, x, y, z ) = (
		hit( copy, x-1, y-1, z-1 )
		+ hit( copy, x-1, y-1, z )
		+ hit( copy, x-1, y-1, z+1 )
		+ hit( copy, x-1, y, z-1 )
		+ hit( copy, x-1, y, z )
		+ hit( copy, x-1, y, z+1 )
		+ hit( copy, x-1, y+1, z-1 )
		+ hit( copy, x-1, y+1, z )
		+ hit( copy, x-1, y+1, z+1 )
		+ hit( copy, x, y-1, z-1 )
		+ hit( copy, x, y-1, z )
		+ hit( copy, x, y-1, z+1 )
		+ hit( copy, x, y, z-1 )
		+ hit( copy, x, y, z )
		+ hit( copy, x, y, z+1 )
		+ hit( copy, x, y+1, z-1 )
		+ hit( copy, x, y+1, z )
		+ hit( copy, x, y+1, z+1 )
		+ hit( copy, x+1, y-1, z-1 )
		+ hit( copy, x+1, y-1, z )
		+ hit( copy, x+1, y-1, z+1 )
		+ hit( copy, x+1, y, z-1 )
		+ hit( copy, x+1, y, z )
		+ hit( copy, x+1, y, z+1 )
		+ hit( copy, x+1, y+1, z-1 )
		+ hit( copy, x+1, y+1, z )
		+ hit( copy, x+1, y+1, z+1 )
	) / 27;

	CTRL_KERNEL_END(CUDA);
}

/* A.3. CELL UPDATE DEFAULT STENCIL */
CTRL_KERNEL_FUNCTION( updateCell_default, CUDA, DEFAULT, KHitTile_float mat, const KHitTile_float copy, const KHitTile_float weight, const float factor ) {
	int x = thread_id_x;
	int y = thread_id_y;
	int z = thread_id_z;

	float sum = 0;

	int i, j, k;
	switch( weight.shape.info.sig.numDims ) {
		case 1:
			for (i=hit_tileDimBegin(weight,0); i<=hit_tileDimEnd(weight,0); i++) {
				if (!hit( weight, i-hit_tileDimBegin(weight, 0) )) continue;
				sum += hit( copy, x+i ) * hit( weight, i-hit_tileDimBegin(weight,0) );
			}
			break;
		case 2:
			for (i=hit_tileDimBegin(weight,0); i<=hit_tileDimEnd(weight,0); i++)
				for (j=hit_tileDimBegin(weight,1); j<=hit_tileDimEnd(weight,1); j++) {
					if (!hit( weight, i-hit_tileDimBegin(weight, 0), j-hit_tileDimBegin(weight, 1) )) continue;
					sum += hit( copy, x+i, y+j ) * hit( weight, 
										i-hit_tileDimBegin(weight,0),
										j-hit_tileDimBegin(weight,1) );
			}
			break;
		case 3:
			for( i=hit_tileDimBegin(weight,0); i<=hit_tileDimEnd(weight,0); i++)
				for( j=hit_tileDimBegin(weight,1); j<=hit_tileDimEnd(weight,1); j++)
					for( k=hit_tileDimBegin(weight,2); k<=hit_tileDimEnd(weight,2); k++) {
						if (!hit( weight, i-hit_tileDimBegin(weight, 0), j-hit_tileDimBegin(weight, 1), k-hit_tileDimBegin(weight, 2))) continue;
						sum += hit( copy, x+i, y+j, z+k ) * hit( weight, 
											i-hit_tileDimBegin(weight,0),
											j-hit_tileDimBegin(weight,1),
											k-hit_tileDimBegin(weight,2) );
					}
	}
	hit( mat, x, y, z ) = sum / factor;

	CTRL_KERNEL_END(CUDA);
}

