
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

#include <hit_mbtile.h>
#include <hit_bshape.h>
#include <hit_allocP.h>


void hit_mbTileDomainShapeInternal(void * newVarP, size_t baseExtent, HitShape shape) {

	HitPTile newVar = (HitPTile)newVarP;
	*newVar = HIT_TILE_NULL;

	// 1. Num of dimensions, Mark no memory status, and Null pointers.
	newVar->shape = shape;
	newVar->baseExtent = baseExtent;
	newVar->memStatus = HIT_MS_NOMEM;
	newVar->hierDepth = (char) HIT_NONHIERARCHICAL;
	newVar->ref = NULL;
	newVar->data = NULL;
	newVar->memPtr = NULL;
	newVar->type = HIT_MB_TILE;
	
	// 2. Process Vertices and Edges cardinalities.
	newVar->card[1] = hit_bShapeCard(shape,0);
	newVar->card[2] = hit_bShapeCard(shape,1);


}


void hit_mbTileAllocInternal(void *newVarP, const char *name, const char *file, int numLine) {
	
	HitPTile newVar = (HitPTile)newVarP;

	// 0. Skip alloc when Null.
	if ( newVar->memStatus == HIT_MS_NULL) return;
	
	// 1. Check variable type, cannot have already its own memory.
	if ( newVar->memStatus == HIT_MS_OWNER ){
		hit_errInternal(__FUNCTION__,"Trying to reallocate a Tile: ",name,file,numLine);
	}

	// 3. Allocate memory.
	if(newVar->card[1] != 0 && newVar->card[2] != 0){
		// @arturo Ago 2015: New allocP interface
		// hit_malloc(newVar->memPtr,(size_t)(newVar->card[1]*newVar->card[2]) * newVar->baseExtent,void*);
		hit_vmalloc(newVar->memPtr, (size_t)(newVar->card[1] * newVar->card[2]) * newVar->baseExtent);
		newVar->data = newVar->memPtr;
	}
	
	// 4. Change memory status.
	newVar->memStatus = HIT_MS_OWNER;
}


int hit_mbTileElemAtIndex(void * varP, int row, int column){
	
	/* 1. Get the shape of the tile */
	HitTile *var = (HitTile *)varP;
	HitShape shape = hit_tileShape(*var);

	//int card1 = hit_bShapeCard(shape,0);
	int card2 = hit_bShapeCard(shape,1);

	return row * card2 + column;
}


int hit_mbTileGraphElemAtIndex(void * varP, int pos1, int pos2){

	/* 1. Get the shape of the tile */
	HitTile *var = (HitTile *)varP;
	HitShape shape = hit_tileShape(*var);

	int row = hit_bShapeCoordToLocal(shape,0,pos1);
	int column = hit_bShapeCoordToLocal(shape,1,pos2);

	//int card1 = hit_bShapeCard(shape,0);
	int card2 = hit_bShapeCard(shape,1);
	
	return row * card2 + column;
}



/* @arturo to @javier: TODO: Check this function, potential bugs */
void hit_mbTileClear(void * tileP){

	/* 1. Get the shape of the tile */
	HitTile *tile = (HitTile *)tileP;
	//HitShape shape = hit_tileShape(*tile);

	int card1 = tile->card[1];
	int card2 = tile->card[1];

	bzero(tile->data, tile->baseExtent  * (size_t) (card1 * card2) );
}





