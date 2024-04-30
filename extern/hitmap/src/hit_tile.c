/**
 * Hierarchical tiling of multi-dimensional dense array.
 * Definitions and functions to manipulate HitTile types. 
 * This type defines a handler for dense arrays and sparse structures
 * with meta-information to create and manipulate hierarchical tiles. 
 *
 * @file hit_tile.c
 * @version 1.1.1
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Nov 2012
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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <hit_allocP.h>
#include <hit_sshape.h>
#include <hit_cshape.h>
#include <hit_bshape.h>
#include <hit_tile.h>

/* Hit NULL VARIABLE */
/*
unsigned char HitNullData[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
#define	HIT_TILE_NULL_STATIC	{ 0, HIT_SHAPE_NULL_STATIC, { 0,0,0,0 }, 0, { 0,0,0,0,0 }, { 0,0,0,0 }, &HitNullData, NULL, NULL, HIT_MS_NULL, HIT_NONHIERARCHICAL, HIT_HIERARCHY_UNDEFINED_CHILDREN }
*/

HitTile	HIT_TILE_NULL = HIT_TILE_NULL_STATIC;
HitPTile HIT_TILE_NULL_POINTER = &HIT_TILE_NULL;

/* DEBUG: DUMP VARIABLE STRUCTURE INFORMATION */									//yuri
/*void hit_tileDumpTileInternal(const void *var, const char* name, FILE *file) {
	int i;
	const HitTile *v = (const HitTile *)var;

	fprintf(file,"Dump tile %s, address %p\n",name,var);
	if (var == NULL) return;

	fprintf(file,"\tmemStatus: %d\n", v->memStatus);
	fprintf(file,"\tnumDims:   %d\n", hit_shapeDims(v->shape));
	fprintf(file,"\tbaseExtent:   %d\n", (unsigned int) v->baseExtent);
	for (i=0; i<hit_shapeDims(v->shape); i++) fprintf(file,"\tcard[%d]: %d\n", i, v->card[i]);
	fprintf(file,"\tacumCard:     %d\n", v->acumCard);
	for (i=0; i<hit_shapeDims(v->shape)+1; i++) fprintf(file,"\torigAcumCard[%d]: %d\n", i, v->origAcumCard[i]);
	for (i=0; i<hit_shapeDims(v->shape); i++) {
		fprintf(file,"\tqstride[%d]: %d\n", i, v->qstride[i]);
	}
	for (i=0; i<hit_shapeDims(v->shape); i++) {
		fprintf(file,"\tSig[%d]: %d, %d, %d\n", i, hit_shapeSig(v->shape,i).begin, hit_shapeSig(v->shape,i).end, hit_shapeSig(v->shape,i).stride);
	}
	fprintf(file,"\thierDepth:   %d\n", v->hierDepth);
	fprintf(file,"\tchildBegin: { ");
	for (i=0; i<HIT_MAXDIMS; i++) fprintf(file,"%d ", v->childBegin[i]);
	fprintf(file,"}\n");
	fprintf(file,"\tchildSize: { ");
	for (i=0; i<HIT_MAXDIMS; i++) fprintf(file,"%d ", v->childSize[i]);
	fprintf(file,"}\n");
	fprintf(file,"\tdata: %p\n", v->data);
	fprintf(file,"\tmemPtr: %p\n", v->memPtr);
	fprintf(file,"\tref: %p\n", (void*) v->ref);
	fprintf(file,"\n");
}*/



/* Hit UPDATE MEMORY CARDINALITIES: INTERNAL */
static inline void hit_tileUpdateAcumCards(void *newVarP) {
	HitTile *newVar = (HitTile *)newVarP;
	int i;
	int cardinality;

	newVar->origAcumCard[hit_shapeDims(newVar->shape)] = 1;
	cardinality=1;
	for(i=hit_shapeDims(newVar->shape)-1; i>=0; i--) {
		cardinality = cardinality * newVar->card[i];
		newVar->origAcumCard[i] = cardinality;
	}
	newVar->acumCard = newVar->origAcumCard[0];
}


/* Hit OFFSET TO THE START OF THE REAL DATA FROM THE FIRST MEMORY POSITION */
/* Several uses:
* 		offset to compute the data pointer from the memory start
* 		-offset to compute the data pointer of the global coordinates of a selection
*/
static inline size_t hit_tileOffset(const void *varP) {
	const HitTile *v = (const HitTile *)varP;
	int i;
	size_t offset=0;

	for (i=0; i<hit_shapeDims(v->shape); i++)
		offset = offset + (size_t)hit_shapeSig(v->shape,i).begin * (size_t)v->origAcumCard[i+1];
	return offset;
}


/* Hit BUILD A TILE ON A SINGLE VARIABLE OF ANY TYPE */
/* @arturo Nov 2012 */
void hit_tileSingleInternal( void *tileP, void *varP, size_t size ) {
	HitTile	*result = (HitTile *)tileP;
	*result = HIT_TILE_NULL;
	
	/* 1. ONE DIMENSION, ONE ELEMENT */
	result->shape = hit_shape( 1, hit_sigStd(1) );
	result->card[0] = 1;
	result->card[1] = 1;
	result->acumCard = 1;
	result->origAcumCard[0] = 1;
	result->origAcumCard[1] = 1;
	result->qstride[0] = 1;

	/* 2. NOT_OWNER MEMORY STATUS, THE FREE OPERATION SHOULD IGNORE IT, IT WORKS LIKE A SELECTION */
	result->memStatus = HIT_MS_NOT_OWNER;
	result->data = varP;

	/* 3. OTHER FIELDS */
	result->baseExtent = size;
	result->hierDepth = 0;

	/* 4. RETURN NEW TILE */
	return;
}


/* Hit VARIABLE DECLARATION: ARRAY */
void hit_tileDomainInternal(void *newVarP, size_t baseExtent, int hierDepth, int numDims, ...){
	int i;
	va_list ap;
	HitPTile newVar = (HitPTile)newVarP;
	*newVar = HIT_TILE_NULL;

#ifdef DEBUG
fprintf(stderr,"CTRL DomainInternal, baseExtent: %d, hierDepth: %d, numDims: %d\n", (unsigned int)baseExtent, hierDepth, numDims);
#endif

	/* 1. NUM OF DIMENSIONS, MARK NO MEMORY STATUS, AND NULL POINTERS */
	hit_shapeDimsSet(newVar->shape,numDims);
	newVar->baseExtent = baseExtent;
	newVar->memStatus = HIT_MS_NOMEM;
	newVar->hierDepth = (char)hierDepth;
	newVar->ref = NULL;
	newVar->data = NULL;
	newVar->memPtr = NULL;

	/* 2. PROCESS PARAMETERS AS CARDINALITIES */
	va_start(ap, numDims);
	for(i=0; i<numDims; i++) {
		newVar->card[i] = va_arg(ap, int);
		// @arturo Oct 2016: RETURN NULL FOR INVALID SHAPES
		if ( newVar->card[i] < 1 ) { *newVar = HIT_TILE_NULL; return; }
		hit_shapeSig(newVar->shape,i).begin = 0;
		hit_shapeSig(newVar->shape,i).end = newVar->card[i]-1;
		hit_shapeSig(newVar->shape,i).stride = 1;
		newVar->qstride[i] = 1;
	}
	va_end(ap);

	/* 3. COMPUTE ACCUMULATED CARDINALITIES */
	hit_tileUpdateAcumCards(newVar);
}


/* Hit VARIABLE DECLARATION WITH SHAPE: ARRAY */
void hit_tileDomainShapeInternal(void * newVarP, size_t baseExtent, int hierDepth, HitShape shape) {
	int i;
	HitPTile newVar = (HitPTile)newVarP;
	*newVar = HIT_TILE_NULL;

#ifdef DEBUG
fprintf(stderr,"CTRL DomainShapeInternal, baseExtent: %d, hierDepth: %d\n", (unsigned int)baseExtent, hierDepth);
#endif

	/* 1. NUM OF DIMENSIONS, MARK NO MEMORY STATUS, AND NULL POINTERS */
	newVar->shape = shape;
	newVar->baseExtent = baseExtent;
	newVar->memStatus = HIT_MS_NOMEM;
	newVar->hierDepth = (char)hierDepth;
	newVar->ref = NULL;
	newVar->data = NULL;
	newVar->memPtr = NULL;

	/* 2. PROCESS PARAMETERS AS CARDINALITIES */
	if( hit_shapeType(shape) == HIT_SIG_SHAPE ){
		for(i=0; i<hit_shapeDims(newVar->shape); i++) {
			/* if stride is not greater than 0, the domain is erroneous */
			newVar->card[i] = (hit_shapeSig(shape,i).stride>0) ? hit_sigCard(hit_shapeSig(shape,i)) : 0;
			newVar->qstride[i] = 1;
		}
	} else {
		newVar->card[0] = hit_cShapeNvertices(shape);
		newVar->qstride[0] = 1;
	}

	/* 3. COMPUTE ACCUMULATED CARDINALITIES */
	hit_tileUpdateAcumCards(newVar);
}









/* Hit VARIABLE DECLARATION AND INITIALIZATION: ARRAY */
void hit_tileAllocInternal(void *newVarP, const char *name, const char *file, int numLine) {
	HitPTile newVar = (HitPTile)newVarP;


	/* 0. SKIP ALLOC WHEN NULL OR WHEN CARDINALITY=0 */
	if ( newVar->memStatus == HIT_MS_NULL || newVar->acumCard==0  ) return;
	
	
	/* 1. CHECK VARIABLE TYPE, CANNOT ALREADY HAVE ITS OWN MEMORY */
	if ( newVar->memStatus == HIT_MS_OWNER ){
		hit_errInternal(__FUNCTION__,"Trying to reallocate a Tile: ",name,file,numLine);
	}

	/* 2. (v0.9.2 Change) MEMORY FOR THE DATA */
#ifdef DEBUG
fprintf(stderr,"CTRL Alloc Internal: Mem size: %d x %d = %d\n", newVar->acumCard, 
																(unsigned int)newVar->baseExtent,
		(unsigned int)newVar->acumCard * (unsigned int)newVar->baseExtent);
#endif

	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newVar->memPtr,(size_t)newVar->acumCard * newVar->baseExtent,void*);
	hit_vmalloc(newVar->memPtr, (size_t)newVar->acumCard * newVar->baseExtent);
	newVar->data = newVar->memPtr;

	/* 3. UPDATES FOR SHADOW COPIES, OR ALLOCATION OF SELECTIONS OF NO-MEMORY VARIABLES */
	if ( newVar->memStatus == HIT_MS_NOT_OWNER 
	  	|| newVar->memStatus == HIT_MS_NOMEM ) {
		/* 3.1. NEW STRIDES TO ACCESS ARE ALWAYS 1 */
		int i;
		for (i=0; i<hit_shapeDims(newVar->shape); i++) newVar->qstride[i] = 1;

		/* 3.2. UPDATE ORIGINAL ACUMULATED CARDINALITIES, NOW IT HAS ITS OWN MEMORY */
		hit_tileUpdateAcumCards(newVar);
	}

	/* 4. CHANGE MEMORY STATUS */
	newVar->memStatus = HIT_MS_OWNER;

	/* 5. END */
}



/* Hit SIGNATURE HELPER: ONLY USED IN THE hit_tileFill FUNCTIONALITY */
static inline int hit_sigBlockSizeInternal( HitSig a ) {
	/* 1. TRIVIAL CASE: STRIDE = 1 */
	if ( a.stride == 1 ) return (a.end - a.begin + 1);
	/* 2. NORMALIZE THE END COMPONENT */
	int jumps = (a.end - a.begin) / a.stride;
	a.end = a.begin + jumps * a.stride;
	/* 3. COMPUTE SIZE OF THE BLOCK */
	return (a.end - a.begin + a.stride);
}


/* Hit FILL UTILITY: RECURSIVELY UPDATE THE ARRAY COORDINATES OF A REGULAR MULTILEVEL HIERARCHY */
void hit_tileFillUpdateArrayCoordinatesRec( HitTile *root, int coords[HIT_MAXDIMS] ) {
	int dim;
	int displacement[HIT_MAXDIMS] = { 0, 0, 0, 0 };

	/* 1. COMPUTE DISPLACEMENTS AND UPDATE ARRAY COORDINATES */
	for(dim=0; dim < hit_tileDims(*root); dim++) {
		displacement[dim] = coords[dim] * hit_sigBlockSizeInternal( hit_tileDimSig(*root,dim) );

	   	hit_tileDimBegin(*root,dim) = displacement[dim] + hit_tileDimBegin(*root,dim);
	   	hit_tileDimEnd(*root,dim) = displacement[dim] + hit_tileDimEnd(*root,dim);
	}

	/* 2. RECURSION END */
	if ( root->hierDepth == HIT_NONHIERARCHICAL ) return;

	/* 3. TRAVERSE ALL POSITIONS OF ROOT TILE TO RECURSIVELY UPDATE THEIR COORDINATES */
	int ind[HIT_MAXDIMS];
	memset(ind, 0, (size_t)hit_tileDims(*root)*sizeof(int));

	int i,j;
	for(i=0; i < root->acumCard; i++) {
		/* 3.1. COMPUTE THE offset IN PARENT MEMORY AND GET CHILD POINTER */
		size_t offset=0;
		for(j=0; j < hit_tileDims(*root); j++) 
			offset += (size_t)ind[j] * (size_t)root->qstride[j] * (size_t)root->origAcumCard[j+1];
		HitTile *child = (HitTile *)( (char *)root->data + offset * root->baseExtent );

		/* 3.2. RECURSION */
		hit_tileFillUpdateArrayCoordinatesRec( child, displacement );

		/* 3.3. NEXT COORDINATES */
		j = hit_shapeDims( root->shape )-1;
		do{
			ind[j] = (ind[j]+1) % (root->card[j]);					
			if ( ind[j] != 0 ) break;
			j--;
		} while( j >= 0 );
	}
}


/* Hit FILL: FILL A TILE WITH A GIVEN VALUE */
void hit_tileFillInternal(void * varP, void * value, const char *name, const char *file, int numLine) {
	HitTile *var = (HitTile *)varP;
	int i;
	int ind[HIT_MAXDIMS], j;
	size_t offset;
	char *ptr;
	HitTile reducedDomainVar[2];

	/* NOTE: CHECKING IF THE TARGET VARIABLE IS A MULTILEVEL-HIERARCHY ONLY ONCE IS 
	 * MORE EFFICIENT, BUT IT HAS IMPLIED THE DUPLICATING OF SOME CODE */

	/* 0.1. SKIP FILLING NULL OR TILES WITH NO MEMORY */
	if ( var->memStatus == HIT_MS_NULL || var->memStatus == HIT_MS_NOMEM ) return;

/* @arturo: Jul 2014. 
 * 	BUG CORRECTED: Fill of selection variables with no boundary that extend beyond limits
 * 			The function accessed elements that were not in the original allocated variable */
	/* 0.2. SELECTION VARIABLES THAT EXPAND BEYOND LIMITS OF THE FIRST ALLOCATED ANCESTOR
	 * 			SUBSELECT THE INTERSECTION OF SHAPES */
	/* 0.2.1. LOCATE THE FIRST ALLOCATED ANCESTOR */
	HitTile *ancestor = var;
	while ( ancestor->memStatus != HIT_MS_OWNER ) ancestor = ancestor->ref;
	/* 0.2.2. INTERSECT SHAPES */
	if ( ancestor != var ) {
		HitShape intersection = hit_shapeIntersect( hit_tileShape(*var),hit_tileShape(*ancestor) );

		/* 0.2.3. SUBSTITUTE THE VARIABLE BY A SELECTION OF THE INTERSECTION */
		hit_tileSelectArrayCoords( &reducedDomainVar[0], var, intersection );
		var = &reducedDomainVar[0];
	}

	/* 0.4. INTERSECTIONS MAY DERIVE IN AN EMPTY VARIABLE */
	if ( var->memStatus == HIT_MS_NULL ) return;

	/* 1. SHALLOW TILES (NATIVE OR PROGRAMMER-DEFINED BASE TYPE) */
	if( var->hierDepth == HIT_NONHIERARCHICAL ) { 
		switch( var->memStatus ) {
			/* 1.1. REAL OWNER VARIABLE, SET ALL THE CONTIGUOS DATA */
			case HIT_MS_OWNER: 
				ptr=(char *)var->data;
				for(i=0; i<var->acumCard; i++) {
					memcpy(ptr, value, var->baseExtent);
					ptr += var->baseExtent;
				}
				break;

			/* 1.2. SELECTION, COPY DATA LIKE AN UPDATE, LOCATING EACH POSITION ON PARENT */
			case HIT_MS_NOT_OWNER:
				/* 1.2.1. INITIALISE ind WITH 0 */
				memset(ind,0, (size_t)hit_shapeDims(var->shape)*sizeof(int));
				/* 1.2.2. TRAVERSE ALL POSITIONS */
				for(i=0;i<var->acumCard;i++) {
					/* 1.2.2.1. COMPUTE THE offset IN PARENT MEMORY */
					offset=0;
					for(j=0;j<hit_shapeDims(var->shape);j++) {
						offset += (size_t)ind[j] * (size_t)var->qstride[j] * (size_t)var->origAcumCard[j+1];
					}
					/* 1.2.2.2. COPY DATA ON THE POSITION */
					memcpy( (char *)var->data+offset*var->baseExtent, value, var->baseExtent );

					/* 1.2.2.3. NEXT POSITION */
					j=hit_shapeDims(var->shape)-1;
					do{
						ind[j]=(ind[j]+1)% (var->card[j]);					
						if (ind[j]!=0) break;
						j--;
					} while(j>=0);
				}
				break;

			/* 1.3. PARANOID CHECK: IMPOSIBLE STATE */
			default: hit_errInternal("tileFill", "Unknown memory state in variable to fill:", name, file, numLine);
			break;
		}
	}
	/* 2. MULTILEVEL HIERARCHY OF TILES */
	else if (var->hierDepth > 0) { // PARANOID CHECK
		HitTile *child = (HitTile *)value;

		/* 2.1. UPDATE THE LEVEL OF THE HIERARCHY */
		var->hierDepth = (char)((int)(child -> hierDepth) + 1);

		/* 2.2. UPDATE THE childSize/childBegin FIELDS WITH CHILDREN INFORMATION */
		for ( i=0; i < hit_tileDims(*child); i++ ) {
			/* 2.3.1. GET THE SIZE OF THE NORMALIZED SIGNATURE */
			var->childSize[i] = hit_sigBlockSizeInternal( hit_tileDimSig( *child, i ) ); 

			/* 2.3.2. IF CHILDREN TILES ARE ALSO MULTILEVEL, MULTIPLY BY THEIR CHILDREN SIZE */
			/* NOTE: IF CHILDREN DOMAINS ARE NOT INITIALIZED, childSize IS MULTIPLIED BY ZERO!!! */
			if ( child->hierDepth ) var->childSize[i] *= child->childSize[i];

			/* 2.3.3. GET THE STARTING ARRAY COORDINATE OF CHILDREN */
			if ( child->hierDepth ) 
				var->childBegin[i] = child->childSize[i] * hit_tileDimBegin( *child, i )
													+ child->childBegin[i];
			else
				var->childBegin[i] = hit_tileDimBegin( *child, i ); 
		}

		/* 2.3. CLONE ELEMENTS AND UPDATE THEIR ARRAY COORDINATES */
		switch( var->memStatus ) {
			/* 2.3.1. REAL OWNER VARIABLE, CLONE THE SUB-TILE ON EACH POSITION */
			case HIT_MS_OWNER: 
				/* WE NEED THE ELEMENT ARRAY INDEXES TO UPDATE THE LOWER LEVEL COORDINATES
				 * THUS, USE THE CODE OF THE NOT_OWNER MEMORY STATUS
				ptr=var->data;
				for(i=0;i<var->acumCard;i++) {
					hit_tileCloneInternal(ptr, value, name, file, numLine);
					ptr = (char *)ptr + var->baseExtent;
				}
				break;
				*/

			/* 2.3.2. SELECTION, COPY DATA LIKE AN UPDATE, LOCATING EACH POSITION ON PARENT */
			case HIT_MS_NOT_OWNER:
				/* 2.3.2.1. INITIALISE ind WITH 0 */
				memset(ind,0, (size_t)hit_shapeDims(var->shape)*sizeof(int));
				/* 2.3.2.2. TRAVERSE ALL POSITIONS */
				for(i=0;i<var->acumCard;i++) {
					/* 2.3.2.2.1. COMPUTE THE offset IN PARENT MEMORY */
					offset=0;
					for(j=0;j<hit_shapeDims(var->shape);j++) {
						offset += (size_t)ind[j] * (size_t)var->qstride[j] * (size_t)var->origAcumCard[j+1];
					}
					/* 2.3.2.2.2. CLONE THE INPUT SUB-TILE ON THE POSITION */
					HitTile *pos = (HitTile *)((char *)var->data+offset*var->baseExtent);
					hit_tileCloneInternal( (char *)pos, value, name, file, numLine );

					/* TODO: Patch (TO BE DEPRECATED IN v1.2, see PERSONAL NOTES Nov-2011-02.1) */
					pos->ancestor = (HitTile*) varP;


					pos->ref=NULL;

					/* 2.3.2.2.3. RECURSIVELY UPDATE THE ARRAY COORDINATES */
					int arrayCoords[HIT_MAXDIMS] = { 0, 0, 0, 0 };
					for(j=0;j<hit_shapeDims(var->shape);j++) 
						arrayCoords[j] = ind[j] * hit_tileDimStride(*var,j) + hit_tileDimBegin(*var,j);
					hit_tileFillUpdateArrayCoordinatesRec( pos, arrayCoords );

					/* 2.3.2.2.4. NEXT POSITION */
					j=hit_shapeDims(var->shape)-1;
					do{
						ind[j]=(ind[j]+1)% (var->card[j]);					
						if (ind[j]!=0) break;
						j--;
					} while(j>=0);
				}
				break;

			/* 2.3.3. PARANOID CHECK: IMPOSIBLE STATE */
			default: hit_errInternal("tileFill", "Unknown memory state in variable to fill:", name, file, numLine); 
			break;
		}
	}
	/* 3. PARANOID CHECK: NEGATIVE VALUE IN HIERARCHY-LEVEL FIELD  */
	else hit_errInternal("tileFill", "Negative value in hierarchy-level of variable:", name, file, numLine); 
}



/* Hit MULTILEVEL TILES: LOCATE BLOCK WHICH CONTAINS THE GIVEN ARRAY COORDINATES  */
void *hit_mtileBlockArrayCoord( HitTile *root, int coords[HIT_MAXDIMS] ) {
	int thisLevelTileCoords[HIT_MAXDIMS];

	/* 0. CHECK: SHOULD NOT BE NULL AND MEMORY SHOULD BE INITIALIZED */
	if ( root->memStatus == HIT_MS_NULL || root->memStatus == HIT_MS_NOMEM ) return NULL;

	/* 1. TRANSFORM TO ROOT TILE COORDINATES USING THE CHILDREN BLOCK INFORMATION */
	int dim;
	for ( dim=0; dim < hit_tileDims(*root); dim++ ) {
		/* 1.1. TRANSFORM TO LOCAL BLOCK COORDINATES */
		int arrayCoord = (int)floor(( coords[dim] - root->childBegin[dim] ) / (double)root->childSize[dim]);
		/* 1.2. CHECK IF THE BLOCK IS IN THE ROOT TILE DOMAIN */
		if ( ! hit_sigIn( hit_tileDimSig(*root,dim), arrayCoord ) ) return NULL;
		/* 1.3. STORE THE TILE COORDINATE */
		thisLevelTileCoords[dim] = (int)floor(( arrayCoord - hit_tileDimBegin(*root,dim) ) / (double)hit_tileDimStride(*root,dim) );
	}

	/* 2. IF IT IS A LEAF, RETURN THE TILE */
	if ( root->hierDepth == HIT_NONHIERARCHICAL ) return root;

	/* 3. NOT A LEAF, RECURSION WITH THE CHILD ELEMENT */
	size_t offset;
	int j;
	offset=0;
	for(j=0; j < hit_tileDims(*root); j++) {
		offset += (size_t)thisLevelTileCoords[j] * (size_t)root->qstride[j] * (size_t)root->origAcumCard[j+1];
	}
	HitTile *child = (HitTile *) root->data + offset;
	return hit_mtileBlockArrayCoord( child, coords );
}

/* Hit MULTILEVEL TILES: LOCATE THE ELEMENT WITH THE GIVEN ARRAY COORDINATES  */
void *hit_mtileElemAtArrayCoord( HitTile *root, int coords[HIT_MAXDIMS] ) {
	int thisLevelTileCoords[HIT_MAXDIMS];

	/* 0. CHECK: SHOULD NOT BE NULL AND MEMORY SHOULD BE INITIALIZED */
	if ( root->memStatus == HIT_MS_NULL || root->memStatus == HIT_MS_NOMEM ) return NULL;

	/* 1. TRANSFORM TO ROOT TILE COORDINATES USING THE CHILDREN BLOCK INFORMATION */
	int dim;
	for ( dim=0; dim < hit_tileDims(*root); dim++ ) {
		/* 1.1. TRANSFORM TO LOCAL BLOCK COORDINATES */
		int arrayCoord = (int)floor(( coords[dim] - root->childBegin[dim] ) / (double)root->childSize[dim]);
		/* 1.2. CHECK IF THE BLOCK IS IN THE ROOT TILE DOMAIN */
		if ( ! hit_sigIn( hit_tileDimSig(*root,dim), arrayCoord ) ) return NULL;
		/* 1.3. STORE THE TILE COORDINATE */
		thisLevelTileCoords[dim] = (int)floor(( arrayCoord - hit_tileDimBegin(*root,dim) ) / (double)hit_tileDimStride(*root,dim) );
	}

	/* 2. LOCATE THE TILE ELEMENT */
	size_t offset;
	int j;
	offset=0;
	for(j=0; j < hit_tileDims(*root); j++) {
		offset += (size_t)thisLevelTileCoords[j] * (size_t)root->qstride[j] * (size_t)root->origAcumCard[j+1];
	}

	/* 3. IF IT IS A LEAF, RETURN THE ELEMENT POINTER */
	if ( root->hierDepth == HIT_NONHIERARCHICAL ) return (char *)root->data + offset * root->baseExtent;

	/* 4. NOT A LEAF, RECURSION WITH THE CHILD ELEMENT */
	return hit_mtileElemAtArrayCoord( (HitTile *)root->data + offset, coords );
}


/* Hit CLONE: DUPLICATE VARIABLE, MEMORY AND DATA */
void hit_tileCloneInternal(void *newVarP, const void *oldVarP, const char *name, const char *file, int numLine) {
	HitTile *newVar = (HitTile *)newVarP;
	const HitTile *oldVar = (const HitTile *)oldVarP;
	int i;

	/* 1. DUPLICATE HANDLER */
	*newVar = *oldVar;

	/* 2. NULL VARIABLE: RETURN */
	if ( oldVar->memStatus == HIT_MS_NULL ) return;

	/* 3. ALLOCATE NEW MEMORY */
	/* 3.1. ALLOCATE */
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(newVar->memPtr,(size_t)newVar->acumCard * newVar->baseExtent,void*);
	hit_vmalloc(newVar->memPtr, (size_t)newVar->acumCard * newVar->baseExtent );
	
	newVar->data = newVar->memPtr;
	newVar->memStatus = HIT_MS_OWNER;

	/* 3.2. NEW STRIDES TO ACCESS ARE ALWAYS 1 */
	for (i=0; i<hit_shapeDims(newVar->shape); i++) newVar->qstride[i] = 1;

	/* 3.3. UPDATE ORIGINAL ACUMULATED CARDINALITIES, NOW IT HAS ITS OWN MEMORY */
	hit_tileUpdateAcumCards(newVar);

	/* NO MEMORY: RETURN */
	if(oldVar->memStatus==HIT_MS_NOMEM) return;

	/* PARENT TILE IS THE ORIGINAL TILE: CLONE IS LIKE A SELECTION + ALLOCATION + UPDATE */
#ifdef __cplusplus
	newVar->ref = const_cast<HitTile*>(oldVar);
#else
	newVar->ref = oldVar;
#endif

	/* HIERARCHICAL STRUCTURE: PROCESS CHILDREN */
	if( oldVar->hierDepth > 0 ) {
		switch( oldVar->memStatus ) {
			/* 4.1. NO MEMORY, RETURN*/
			case HIT_MS_NOMEM: return;

			/* 4.2. REAL OWNER VARIABLE, CLONE ALL THE CHILDS, MEMORY IS CONTIGUOUS */
			case HIT_MS_OWNER:
				for(i=0;i<oldVar->acumCard;i++) {
					hit_tileCloneInternal( (char *)newVar->data+(size_t)i*newVar->baseExtent, (char *)oldVar->data+(size_t)i*oldVar->baseExtent, name, file, numLine); 
				}
			   break;

			/* 4.3. SELECTION, CLONE ALL THE CHILDS, MEMORY IS NOT CONTIGUOUS  */
			case HIT_MS_NOT_OWNER:
				// Fake as a shadow to call Update
				//newVar->ref = oldVar;
				hit_tileUpdateFromToAncestorInternal(newVar, HIT_UPDATE_FROM, name, file, numLine);
				//newVar->ref = NULL;
				break;

			/* 4.4. IMPOSIBLE STATE */
			default: hit_errInternal("tileClone", "Unknown memory state in source variable:", name, file, numLine); 
			break;
		}
	}
	/* LEAF TILE */
	else if( oldVar->hierDepth == HIT_NONHIERARCHICAL ) { // PARANOID CHECK
		switch( oldVar->memStatus ) {
			/* 4.1. NO MEMORY, RETURN*/
			case HIT_MS_NOMEM: return;

			/* 4.2. REAL OWNER VARIABLE, COPY ALL THE CONTIGUOS DATA */
			case HIT_MS_OWNER: memcpy(newVar->memPtr, oldVar->memPtr,
									   newVar->baseExtent * (size_t)newVar->acumCard);
				break;

			/* 4.3. SELECTION, COPY DATA LIKE AN UPDATE */
			case HIT_MS_NOT_OWNER:
				/* Fake as a shadow to call Update */
				//newVar->ref = oldVar;
				hit_tileUpdateFromToAncestorInternal(newVar, HIT_UPDATE_FROM, name, file, numLine);
				//newVar->ref=NULL;
				break;

			/* 4.4. IMPOSIBLE STATE */
			default: hit_errInternal("tileClone", "Unknown memory state in source variable:", name, file, numLine); 
			break;
		}
	}
	/* PARANOID CHECK: IMPOSIBLE VALUE IN HIERARCHY-LEVEL FIELD */
	else hit_errInternal("tileClone", "Negative value in hierarchy-level field of variable:", name, file, numLine); 

	/* 5. END */
	return;
}

/* Hit INTERNAL: SELECTION FUNCTIONS: UPDATE ANCESTOR REFERENCE POINTER */
void hit_tileSelectRefPointer( HitTile *newVar, const HitTile *oldVar ){
	switch( oldVar->memStatus ) {
		case HIT_MS_NULL:
			newVar->ref = NULL;
			break;

		case HIT_MS_NOMEM:
				/* IF PARENT HAS NO MEMORY, COPY ITS REFERENCE TO ALLOW UPDATES IF IT IS
				 * 	ALLOCATED IN THE FUTURE */
		case HIT_MS_OWNER:
#ifdef __cplusplus
			newVar->ref = const_cast<HitTile*>(oldVar);
#else
			newVar->ref = oldVar;
#endif
			break;

		case HIT_MS_NOT_OWNER:
			newVar->ref = oldVar->ref;
			break;

		default: /* PARANOID CHECK */
			hit_errInternal("tileSelectInternal", "Unknown memory state in source tile.", "", __FILE__, __LINE__); 
			break;
	}
}


/* Hit SELECTING FROM A VARIABLE (LOCAL COORDINATES) */
int hit_tileSelectInternal(void *newVarP, const void *oldVarP, HitShape sh, int out) {
	int i;
	// (v1.1) Use size_t types to ensure correctness for 64 bits
	//int offset = 0;
	size_t offset = 0;
	HitTile *newVar = (HitTile *)newVarP;
	const HitTile *oldVar = (const HitTile *)oldVarP;

	int numDims;
	*newVar = HIT_TILE_NULL;

	/* 0.1. OPTIMIZE WHEN SELECTING A NULL SHAPE */
	if ( hit_shapeDims(sh) == -1 ) return 1;

	/* 0.2 OPTIMIZE WHEN SELECTING HitNull */
	if (oldVar->memStatus == HIT_MS_NULL) return 1;

	/* 0.3 OPTIMIZE WHEN SELECTING THE WHOLE VARIABLE */
	//selecting with HIT_SHAPE_WHOLE
	if (hit_shapeDims(sh) == 0) {
		memcpy(newVarP, oldVarP, sizeof(HitTile));
		/* MEMORY STATUS MAY CHANGE ON THE SELECTION */
		if ( oldVar->memStatus != HIT_MS_NOMEM ) newVar->memStatus = HIT_MS_NOT_OWNER;
		/* UPDATE REFERENCE POINTER */
		hit_tileSelectRefPointer( newVar, oldVar );
		return 1;
	}

	/* 0.4. COPY HIERARCHICAL DEPTH PROPERTIES */
	newVar->hierDepth=oldVar->hierDepth;
	memcpy(newVar->childSize, oldVar->childSize, HIT_MAXDIMS*sizeof(int));
	memcpy(newVar->childBegin, oldVar->childBegin, HIT_MAXDIMS*sizeof(int));

	/* COMPUTE SIGNATURES, DETECT AND STOP OUT-OF-BOUNDS SELECTIONS */
	/* 1. GET NUMBER OF DIMENSIONS */
	numDims = hit_shapeDims(oldVar->shape);

	/* 2. BLEND SIGNATURES FROM PARAMETERS */
	for(i=0; i<hit_shapeDims(sh); i++) {
		HitSig newSig = hit_shapeSig(sh,i);
		// (v1.1 @arturo) ACCEPT HIT_SIG_WHOLE TO INDICATE THAT THE WHOLE DIMENSION IS SELECTED
		if ( hit_sigCmp( newSig, HIT_SIG_WHOLE )) newSig = hit_sigStd( oldVar->card[i] );
		else
			/* CHECK OUT OF BOUNDS */
			if ( out == HIT_OUTOFBOUNDS_CHECK
					&& (
					newSig.begin < 0
					|| newSig.end < newSig.begin
					|| newSig.stride < 1
					|| newSig.end > oldVar->card[i]
					)
				)
			{
#ifdef DEBUG
fprintf(stderr,"CTRL tileSelect: Out of bounds in signature %d: (%d,%d,%d) -> (%d,%d,%d)\n", i, newSig.begin, newSig.end, newSig.stride, hit_shapeSig(oldVar->shape,i).begin, hit_shapeSig(oldVar->shape,i).end, hit_shapeSig(oldVar->shape,i).stride);
#endif
					/* RETURN NULL */
					*newVar = HIT_TILE_NULL;
					return 0;
			}

		/* COMPUTE OFFSET */
		// @arturo Mar 13, BUG CORRECTED: Selection of several levels of depth with stride
		//offset = offset + (size_t)(newSig.begin) * (size_t)(oldVar->origAcumCard[i+1]); 
		offset = offset + (size_t)(newSig.begin * oldVar->qstride[i]) * (size_t)(oldVar->origAcumCard[i+1]); 

		/* ALIGN END TO STRIDE */
		newSig.end = newSig.end - (newSig.end - newSig.begin) % newSig.stride;

		/* STRIDE NORMALIZATION FOR BEGIN=END */
		if ( newSig.begin == newSig.end ) newSig.stride=1;

		/* BLEND SIGNATURES */
		hit_shapeSig(newVar->shape,i) = hit_sigBlend( hit_shapeSig(oldVar->shape,i), newSig );
		newVar->card[i] = hit_sigCard( hit_shapeSig(newVar->shape,i));
		newVar->qstride[i] = newSig.stride*oldVar->qstride[i];			
	}

	/* 3. IF NEEDED COPY OTHER SIGNATURES FROM ORIGINAL VARIABLE */
	for(i=hit_shapeDims(sh); i<hit_shapeDims(oldVar->shape); i++) {
		hit_shapeSig(newVar->shape,i) = hit_shapeSig(oldVar->shape,i);
		newVar->card[i] = oldVar->card[i];
		newVar->qstride[i] = oldVar->qstride[i];	
	}

	/* 4. SET SELECTION, NUMBER OF DIMENSIONS, SIGs AND CARDINALITIES */
	hit_shapeDimsSet(newVar->shape,numDims);
	newVar->baseExtent = oldVar->baseExtent;

	/* 3. COMPUTE NEW ACCUMULATED CARDINALITIES */
	hit_tileUpdateAcumCards(newVar);

	/* 4. COPY ORIGINAL ACUMMULATED CARDS */
	memcpy(newVar->origAcumCard, 
			oldVar->origAcumCard, 
			(size_t)(hit_shapeDims(newVar->shape)+1)*sizeof(int)
			);

	/* 5. SET THE DATA POINTER */
	/*		START AT OLD DATA POINTER
	 *		ADD THE OFFSET OF REDUCED DIMENSIONS
	 *		ADD THE OFFSET OF THE NON-REDUCED DIMENSIONS (BEGINS)
	 *
	 * NOTE: SELECTION OF NO-MEMORY VARIABLES INHERIT THE NO-MEMORY STATUS
	 */
	if ( oldVar->memStatus == HIT_MS_NOMEM ) {
		newVar->data = NULL;
		newVar->memStatus = HIT_MS_NOMEM;
	}
	else {
		newVar->data = (char*)(oldVar->data) + offset * oldVar->baseExtent;
		newVar->memStatus = HIT_MS_NOT_OWNER;
	}

	/* 6. SET THE REFERENCE FIELD */
	/* NO-MEMORY SELECTION SHOULD BECOME A SHADOW WHEN ALLOCATED: PREPARE THE REFERENCE
	 *  a) SUBSELECTION OF SELECTION: COPY OLD REFERENCE
	 *  b) SUBSELECTION OF A VARIABLE: REFERENCE TO THAT VARIABLE 
	 */
	/* v1.1: ALLOW UPDATES TO PARENT MEMORY TILES OF SECOND GENERATION OR MORE
	newVar->ref = ( oldVar->ref == NULL ) ? oldVar : oldVar->ref;
	*/
	hit_tileSelectRefPointer( newVar, oldVar );

	/* 7. END */
	return 1;
}


/** Checks if the shape is inside the boundary of the tile
 * using array coords */
int hit_tileCheckBoundaryArrayCoords(const void *tileP, HitShape sh) {
	int i;

	const HitTile *tile = (const HitTile*) tileP;

	for (i = 0; i < hit_shapeDims(sh); i++) {
		HitSig newSig = hit_shapeSig(sh,i);
		HitSig tileSig = hit_shapeSig(hit_tileShape(*tile),i);

		/* CHECK OUT OF BOUNDS */
		if (newSig.begin < tileSig.begin || newSig.end > tileSig.end
			|| newSig.end < newSig.begin || newSig.stride < 1){

			return HIT_OUTOFBOUNDS_CHECK;
		}
	}
	return HIT_NO_OUTOFBOUNDS_CHECK;
}


/** Checks if the shape is inside the boundary of the tile
 * using tile coords */
int hit_tileCheckBoundaryTileCoords(const void *tileP, HitShape sh) {
	int i;

	const HitTile *tile = (const HitTile*) tileP;

	for (i = 0; i < hit_shapeDims(sh); i++) {
		HitSig newSig = hit_shapeSig(sh,i);

		/* CHECK OUT OF BOUNDS */
		if (newSig.begin < 0 || newSig.end < newSig.begin || newSig.stride < 1
				|| newSig.end >= tile->card[i]){

			return HIT_OUTOFBOUNDS_CHECK;
		}
	}
	return HIT_NO_OUTOFBOUNDS_CHECK;
}



/* Hit SELECTING FROM A VARIABLE (GLOBAL COORDINATES) */
int hit_tileSelectArrayCoordsInternal(void *newVarP, const void *oldVarP, HitShape sh, int out) {
	int i;
	// (v1.1) Use size_t types to ensure correctness for 64 bits
	//int	offset = 0;
	size_t	offset = 0;
	HitTile *newVar = (HitTile *)newVarP;
	const HitTile *oldVar = (const HitTile *)oldVarP;

	int numDims;
	*newVar = HIT_TILE_NULL;

	/* 0.1. OPTIMIZE WHEN SELECTING A NULL SHAPE */
	if ( hit_shapeDims(sh) == -1 ) return 1;

	/* 0.2 OPTIMIZE WHEN SELECTING HitNull */
	if (oldVar->memStatus == HIT_MS_NULL) return 1;

	/* 0.3 OPTIMIZE WHEN SELECTING THE WHOLE VARIABLE */
	if (hit_shapeDims(sh) == 0) {
		memcpy(newVarP, oldVarP, sizeof(HitTile));
		/* MEMORY STATUS MAY CHANGE ON THE SELECTION */
		if ( oldVar->memStatus != HIT_MS_NOMEM ) newVar->memStatus = HIT_MS_NOT_OWNER;
		/* UPDATE REFERENCE POINTER */
		hit_tileSelectRefPointer( newVar, oldVar );
		return 1;
	}
	
	/* 0.4 RETURN NULL TILE IF SHAPE NOT IN TILE DOMAIN */
	for(i=0; i<hit_shapeDims(sh); i++) {
		if( out == HIT_OUTOFBOUNDS_CHECK 
					&& ! hit_sigIn(hit_shapeSig(oldVar->shape,i),hit_shapeSig(sh,i).begin) ) {
			*newVar = HIT_TILE_NULL;
#ifdef DEBUG
	printf("---- TileSelect, Out of Bounds Check! CTRL 1\n");
#endif
			return 1;
		}
		if( out == HIT_OUTOFBOUNDS_CHECK 
					&& ! hit_sigIn(hit_shapeSig(oldVar->shape,i),hit_shapeSig(sh,i).end) ) {
			*newVar = HIT_TILE_NULL;
#ifdef DEBUG
	printf("---- TileSelect, Out of Bounds Check! CTRL 2: %d in [%d:%d:%d] is false\n",
				hit_shapeSig(sh,i).end,
				hit_shapeSig(oldVar->shape,i).begin,
				hit_shapeSig(oldVar->shape,i).end,
				hit_shapeSig(oldVar->shape,i).stride
			);
#endif
			return 1;
		}
		if( hit_shapeSig(sh,i).stride % hit_shapeSig(oldVar->shape,i).stride != 0 ) {
#ifdef DEBUG
	printf("---- TileSelect, Out of Bounds Check! CTRL 3\n");
#endif
			*newVar = HIT_TILE_NULL;
			return 1;
		}
	}

	/* 0.5. COPY THE HIERARCHICAL DEPTH PROPERTIES */
	newVar->hierDepth=oldVar->hierDepth;
	memcpy(newVar->childSize, oldVar->childSize, HIT_MAXDIMS*sizeof(int));
	memcpy(newVar->childBegin, oldVar->childBegin, HIT_MAXDIMS*sizeof(int));

	/* 1. COMPUTE SIGNATURES, DETECT AND STOP OUT-OF-BOUNDS SELECTIONS */
	/* 1.1. GET NUMBER OF DIMENSIONS */
	numDims = hit_shapeDims(oldVar->shape);

	/* 1.2. BLEND FIRST SIGNATURES FROM PARAMETERS */
	for(i=0; i<hit_shapeDims(sh); i++) {
		// CIRCUMVENT A BUG IN GCC 4.6 
#if ( __GNUC__ == 4 && __GNUC_MINOR__ == 6 )
		HitSig newSig;
		memcpy( &newSig, &hit_shapeSig( sh, i ), sizeof(newSig) );
#else
		HitSig newSig = hit_shapeSig(sh,i);
#endif
		// (v1.1 @arturo) ACCEPT HIT_SIG_WHOLE TO INDICATE THAT THE WHOLE DIMENSION IS SELECTED
		if ( hit_sigCmp( newSig, HIT_SIG_WHOLE )) newSig = hit_sigStd( oldVar->card[i] );
		else {
			/* CHECK OUT OF BOUNDS */
			if ( out == HIT_OUTOFBOUNDS_CHECK
					&& (
					newSig.begin < hit_shapeSig(oldVar->shape,i).begin
					|| newSig.end > hit_shapeSig(oldVar->shape,i).end
					|| newSig.stride < 1
					/* WHEN ONLY ONE ELEMENT IS SELECTED (BEGIN=END) ANY STRIDE IS OK
					 * IT WILL BE NORMALIZED TO 1 AFTERWARDS */
					|| ( newSig.begin != newSig.end && 
						 newSig.stride % hit_shapeSig(oldVar->shape,i).stride != 0
						)
					|| ( newSig.begin - hit_shapeSig(oldVar->shape,i).begin )
												% hit_shapeSig(oldVar->shape,i).stride != 0
					|| newSig.end < newSig.begin
					)
				)
			{
#ifdef DEBUG
	fprintf(stderr,"CTRL tileSelect: Out of bounds in global signature %d: (%d,%d,%d)\n", i, newSig.begin, newSig.end, newSig.stride);
	fprintf(stderr,"CTRL tileSelect: Old var signature %d: (%d,%d,%d)\n", i, hit_shapeSig(oldVar->shape,i).begin, hit_shapeSig(oldVar->shape,i).end, hit_shapeSig(oldVar->shape,i).stride);
#endif
				/* RETURN NULL */
				*newVar = HIT_TILE_NULL;
				return 0;
			}

			/* TRANSLATE TO LOCAL COORDINATES: SEVERAL ELEMENTS SELECTED */
			newSig.begin = (newSig.begin - hit_shapeSig(oldVar->shape,i).begin) / hit_shapeSig(oldVar->shape,i).stride;
			newSig.end = (newSig.end - hit_shapeSig(oldVar->shape,i).begin) / hit_shapeSig(oldVar->shape,i).stride;
			newSig.stride = newSig.stride / hit_shapeSig(oldVar->shape,i).stride;
		}

		/* COMPUTE OFFSET */
		// offset = offset + (size_t)(newSig.begin) * (size_t)(oldVar->origAcumCard[i+1]); 
		// @arturo Mar 13, BUG CORRECTED: Selection of several levels of depth with stride
		offset = offset + (size_t)(newSig.begin * oldVar->qstride[i]) * (size_t)(oldVar->origAcumCard[i+1]); 

		/* ALIGN END TO THE NEW STRIDE */
		// @arturo Mar 2013, BUG CORRECTED
		newSig.end = newSig.end - (newSig.end - newSig.begin) % newSig.stride;

		/* STRIDE NORMALIZATION FOR BEGIN=END */
		if ( newSig.begin == newSig.end ) newSig.stride=1;

		/* BLEND SIGNATURES */
		hit_shapeSig(newVar->shape,i) = hit_sigBlend( hit_shapeSig(oldVar->shape,i), newSig );
		newVar->card[i] = hit_sigCard( hit_shapeSig(newVar->shape,i));
		newVar-> qstride[i] = newSig.stride*oldVar->qstride[i];
	}
	/* 1.3. IF NEEDED COPY OTHER SIGNATURES FROM ORIGINAL VARIABLE */
	for(i=hit_shapeDims(sh); i<hit_shapeDims(oldVar->shape); i++) {
		hit_shapeSig(newVar->shape,i) = hit_shapeSig(oldVar->shape,i);
		newVar->card[i] = oldVar->card[i];
		newVar->qstride[i] = oldVar->qstride[i];
	}

	/* 2. SET SELECTION, NUMBER OF DIMENSIONS, SIGs AND CARDINALITIES */
	hit_shapeDimsSet(newVar->shape,numDims);
	newVar->baseExtent = oldVar->baseExtent;

	/* 3. COMPUTE NEW ACCUMULATED CARDINALITIES */
	hit_tileUpdateAcumCards(newVar);

	/* 4. COPY ORIGINAL ACUMMULATED CARDS */
	memcpy(newVar->origAcumCard, 
			oldVar->origAcumCard, 
			(size_t)(hit_shapeDims(newVar->shape)+1)*sizeof(int)
			);

	/*		START AT OLD DATA POINTER
	 *		ADD THE OFFSET OF REDUCED DIMENSIONS
	 *		ADD THE OFFSET OF THE NON-REDUCED DIMENSIONS (BEGINS)
	 *
	 * NOTE: SELECTION OF NO-MEMORY VARIABLES INHERIT THE NO-MEMORY STATUS
	 */
	if ( oldVar->memStatus == HIT_MS_NOMEM ) {
		newVar->memStatus = HIT_MS_NOMEM;
		newVar->data = NULL;
	}
	else {
		newVar->memStatus = HIT_MS_NOT_OWNER;
		newVar->data = (char*)(oldVar->data) + offset * oldVar->baseExtent;
	}

	/* 6. SET THE REFERENCE FIELD */
	/* NO-MEMORY SELECTION SHOULD BECOME A SHADOW WHEN ALLOCATED: PREPARE THE REFERENCE
	 *  a) SUBSELECTION OF SELECTION: COPY OLD REFERENCE
	 *  b) SUBSELECTION OF A VARIABLE: REFERENCE TO THAT VARIABLE 
	 */
	/* v1.1: ALLOW UPDATES TO PARENT MEMORY TILES OF SECOND GENERATION OR MORE
	newVar->ref = ( oldVar->ref == NULL ) ? oldVar : oldVar->ref;
	*/
	hit_tileSelectRefPointer( newVar, oldVar );

	/* 7. END */
	return 1;
}


/* Hit REDUCE DIMENSIONS */
int hit_tileReduceDims(void *newVarP, int numReductions) {
	int i;
	HitTile *newVar = (HitTile *)newVarP;

	/* 1. CHECK NUM DIMENSIONS */
	if (numReductions >= hit_shapeDims(newVar->shape)) {
			*newVar = HIT_TILE_NULL;
			return 1;
	}
	
	/* 2. CHECK DIMENSIONS TO REDUCE: SHOULD HAVE CARDINALITY 1 */
	for (i=0; i<numReductions; i++) {
		if (newVar->card[i] != 1) {
			*newVar = HIT_TILE_NULL;
			return 1;
		}
	}

	/* 3. REDUCE DIMENSIONS */
	for (i=numReductions; i<hit_shapeDims(newVar->shape); i++) {
		hit_shapeSig(newVar->shape,i-numReductions) = hit_shapeSig(newVar->shape,i);
		newVar->card[i-numReductions] = newVar->card[i];
		newVar->origAcumCard[i-numReductions] = newVar->origAcumCard[i];
		newVar->qstride[i-numReductions] = newVar->qstride[i];
	}
	newVar->origAcumCard[i-numReductions] = newVar->origAcumCard[i];
	hit_shapeDimsSet(newVar->shape,hit_shapeDims(newVar->shape) - numReductions);

	/* 4. END */
	return 0;
}

/* Hit EXPAND DIMENSIONS */
int hit_tileExpandDims(void *newVarP, int numExpansions) {
	int i;
	HitTile *newVar = (HitTile *)newVarP;

	/* 1. CHECK NUM DIMENSIONS */
	if (numExpansions + hit_shapeDims(newVar->shape) > HIT_MAXDIMS) {
			*newVar = HIT_TILE_NULL;
			return 0;
	}
	
	/* 2. EXPAND DIMENSIONS */
	newVar->origAcumCard[hit_shapeDims(newVar->shape) + numExpansions] = newVar->origAcumCard[hit_shapeDims(newVar->shape)];
	for (i=hit_shapeDims(newVar->shape)-1; i>=0; i--) {
		hit_shapeSig(newVar->shape,i+numExpansions) = hit_shapeSig(newVar->shape,i);
		newVar->card[i+numExpansions] = newVar->card[i];
		newVar->origAcumCard[i+numExpansions] = newVar->origAcumCard[i];
		newVar->qstride[i+numExpansions] = newVar->qstride[i];
	}
	/* 3. DIMENSIONS EXPANDED: SHOULD HAVE CARDINALITY 1 */
	hit_shapeDimsSet(newVar->shape,hit_shapeDims(newVar->shape) + numExpansions);

	for (i=0; i<numExpansions; i++) {
		newVar->card[i] = 1;
		newVar->origAcumCard[i] = 1;
		newVar->qstride[i] = 1;
		hit_shapeSig(newVar->shape,i).begin = 1;
		hit_shapeSig(newVar->shape,i).end = 1;
		hit_shapeSig(newVar->shape,i).stride = 1;
	}

	/* 4. END */
	return 1;
}



/* Hit UPDATE A SHADOW COPY FROM/TO ITS ANCESTOR 
 * v1.1, Dec 2011
 * v1.2, Apr 2014
 * v1.3, Oct 2016
 * @arturo
 */
void hit_tileUpdateFromToAncestorInternal( void *sh, int fromTo, 
						const char *name, const char *file, int numLine) {
	HitPTile shadow = (HitPTile)sh;
	HitPTile shadowOrig = shadow->ref;
	HitTile reducedDomainShadow;
	int	ind[ hit_tileDims( *shadow ) ];
	int i,j,dim;
	size_t k;
	size_t chunkSize;

#ifdef DEBUG
const char *strFromTo[2] = { "FROM", "TO" };
fprintf(stderr,"CTRL Starting UPDATE %s ANCESTOR\n", strFromTo[ fromTo ]);
#endif
	/* 0.1. SKIP UPDATING NULL */
	if ( shadow->memStatus == HIT_MS_NULL ) return;

	/* 0.2. ONLY SHADOWS: REFERING TO ORIGINAL VARIABLES WITH MEMORY */
	if ( shadow->memStatus != HIT_MS_OWNER
			|| 
			( shadowOrig->memStatus != HIT_MS_OWNER && shadowOrig->memStatus != HIT_MS_NOT_OWNER )
	   )
	{
		hit_errInternal(__FUNCTION__,"Bad variable type",name,file,numLine);
	}

	/* 1. COMPUTE INTERSECTION SHAPE TO DETECT SELECTIONS WHICH EXPANDED THE ORIGINAL SHAPE.
	 * 		IT MEANS TILES CREATED WITH THE NO-BOUNDARY SELECTION FUNCTION.
	 * 		FOR NORMAL SELECTIONS THE INTERSECTION IS THE SHAME SHAPE THAN THE SHAPE IN shadow */

	/* 1.1. COMPUTE INTERSECTION SHAPE */
	HitShape intersection = hit_shapeIntersect( hit_tileShape(*shadow),hit_tileShape(*shadowOrig) );

	/* 1.2. IF DIFFERENT, CHANGE shadow BY A NEW SELECTION TILE WITH THE INTERSECTION SHAPE 
	 *		NO NEED TO UPDATE ref FIELD, WE WILL ONLY USE THE CARDINALITIES AND DATA POINTER */
	if ( ! hit_shapeCmp( intersection, hit_tileShape( *shadow ) ) ) {
#ifdef DEBUG
fprintf(stderr,"CTRL UPDATE %s ANCESTOR: Intersection detects an expanded tile, CREATING SELECTION\n", strFromTo[ fromTo ]);
#endif
		hit_tileSelectArrayCoords( &reducedDomainShadow, shadow, intersection );
		shadow = &reducedDomainShadow;
	}

	//// 
	//// ATENTION: DUE TO THE INTERSECTION (OR THREAD) SUB-SELECTION, 
	////	shadow MAY BE NOW A SELECTION TILE WITH HIT_MS_NOT_OWNER STATUS
	////

	/* 2. COMPUTE THE SIZE OF THE MAXIMAL CONTIGUOS CHUNK OF DATA */

	/* 2.1. CHECK EACH DIMENSION UNTIL SOMETHING PREVENTS MORE DIMENSIONS TO BE	CONTIGUOUS 
	 * 		IN ONE OR BOTH VARIABLES
	 * 		DIMENSIONS' RANGE: (dim in [-1,numDims-1]), -1 INDICATES WHOLE CONTIGUOUS VARIABLES */
	chunkSize = (size_t)1;
	for ( dim = hit_tileDims( *shadow )-1
		;
		dim >= 0 
		// SAME NUMBER OF ELEMENTS IN BOTH VARIABLES
		&& shadow->card[dim] == shadowOrig->card[dim]
		// WITH NO STRIDE
		&& shadowOrig->qstride[dim] == 1 && shadow->qstride[dim] == 1
		// THE SAME MEMORY SPACE IN BOTH VARIABLES TO ALLOW FURTHER DIMENSIONS TO BE CONTIGUOUS.
		&& shadow->origAcumCard[dim+1] == shadowOrig->origAcumCard[dim+1]
		// WITH THE SAME NUMBER OF ELEMENTS AS IN MEMORY SPACE (NO EXTRA BEGINNING/ENDING ELEMENTS)
		&& shadow->origAcumCard[dim] == (int)chunkSize * shadow->card[dim]
		;
		dim-- ) 
		chunkSize = chunkSize * (size_t)shadow->card[ dim ];

	/* 2.2. LAST CONTIGUOUS DIMENSION: CHUNK SIZE IS MULTIPLIED BY THE NUMBER OF ELEMENTS IN 
	 * THIS DIMENSION WHICH ARE CONTIGUOS IN BOTH VARIABLES */
	/* @arturo Mar 2014, BUG CORRECTED: COMPARE REAL STRIDES, AND MEMORY CONTIGUOUS */
	// if ( dim > -1 && shadowOrig->qstride[dim] == 1 && shadow->qstride[dim] == 1 ) {
	if ( dim > -1 
			&& hit_tileDimStride( *shadowOrig, dim ) == hit_tileDimStride( *shadow, dim ) 
			&& shadowOrig->qstride[dim] == 1 ) {
#ifdef DEBUG
fprintf(stderr,"CTRL UPDATE %s ANCESTOR: Contiguous after dimension %d\n", strFromTo[ fromTo ], dim);
#endif
		chunkSize = chunkSize * (size_t)shadow->card[ dim ];
		dim--;
	}
	size_t chunkMemSize  = shadow->baseExtent * chunkSize;

#ifdef DEBUG
fprintf(stderr,"CTRL dim=%d, chunkSize=%d, Shadow: OrigAcumCard=%d\n", dim, (unsigned int)chunkSize, shadow->origAcumCard[0]);
#endif

	/* 4. INIT INDICES */
	for(i=0; i< dim+1; i++) ind[i] = 0;

	/* 5. COPY DATA (AS MANY CHUNKS AS NEEDED) */
	/* 5.1. OPTIMIZATION: AVOID FROM/TO CONDITIONALS INSIDE THE LOOP */
	HitTile *sourceTile, *targetTile;
	/* @arturo Mar 2014, BUG CORRECTED: 
	 * 		COMPUTE DATA START POSITION IN THE ORIGINAL VAR 
	 * 		AND RELATIVE STRIDES FOR THE SHADOW SUBSELECTION */
	int sourceStart, targetStart;
	int relativeBegin = 0;
	int	relativeStride[ hit_tileDims( *shadow ) ];
	int	relativeNoStride[ hit_tileDims( *shadow ) ];
	int *relativeStrideSource, *relativeStrideTarget;

	for (i=0; i<hit_tileDims( *shadow ); i++) {
		relativeBegin = relativeBegin + 
			( hit_tileDimBegin( *shadow, i ) - hit_tileDimBegin( *shadowOrig, i ) )
				* shadowOrig->origAcumCard[i+1]
			/* @arturo Oct 2016, BUG CORRECTED:
			 * 	FOR ORIGINAL (MEMORY OWNER) TILES WITH STRIDED DOMAINS
			 *	DIVIDE THE START-OFFSET BY THE ORIGINAL STRIDE IN THAT DIMENSION */
				/ hit_tileDimStride( *shadowOrig, i );
		// MULTIPLY BY qstride, IN CASE PARENT IS A STRIDED SELECTION OF ANOTHER ANCESTOR VARIABLE 
		relativeStride[ i ] = shadowOrig->qstride[i] *
							hit_tileDimStride( *shadow, i ) / hit_tileDimStride( *shadowOrig, i );
		relativeNoStride[ i ] = 1;
	}
	if ( fromTo == HIT_UPDATE_FROM ) {
		sourceTile = shadowOrig;
		targetTile = shadow;
		sourceStart = relativeBegin;
		targetStart = 0;
		relativeStrideSource = relativeStride;
		relativeStrideTarget = relativeNoStride;
	}
	else {
		sourceTile = shadow;
		targetTile = shadowOrig;
		sourceStart = 0;
		targetStart = relativeBegin;
		relativeStrideSource = relativeNoStride;
		relativeStrideTarget = relativeStride;
	}

	/* OPTIMIZATION: AVOID CONDITIONALS INSIDE THE LOOP
	 * DUPLICATE CODE FOR NATIVE TYPE ELEMENTS AND HIT_TILE TYPE ELEMENTS */
	/* 5.2. NATIVE TYPE ELEMENTS */
	if ( sourceTile->hierDepth == HIT_NONHIERARCHICAL ) {
		if ( dim < 0 ) { // WHOLE CONTIGUOUS CHUNK
			memcpy( (char*)(targetTile->data) + (size_t)targetStart * shadow->baseExtent,
					(char*)(sourceTile->data) + (size_t)sourceStart * shadow->baseExtent, 
					shadow->baseExtent * chunkSize
					);
		}
		else {
		/* @arturo Apr 2014: OPTIMIZATION, INNER LOOP */
		//for(i=0; i< (int)((size_t)shadow->acumCard/chunkSize); i++) 
			for(i=0; i< (int)((size_t)shadow->acumCard/(size_t)shadow->card[dim]/chunkSize); i++) {
				/* @arturo Mar 2014, BUG CORRECTED: POSITIONS FOR SUBSECLECTIONS AND STRIDES */
				int sourcePos = sourceStart;
				int targetPos = targetStart;

			/* 5.2.1. COMPUTE NEXT POSITIONS IN THE shadow AND ORIGINAL VARIABLES */
			/* @arturo Apr 2014: OPTIMIZATION, INNER LOOP */
			//for(j=0; j< dim+1; j++) 
				for(j=0; j< dim; j++) {
				/* @arturo Mar 2014, BUG CORRECTED: qstride[dim] SHOULD BE qstride[j] */
				//sourcePos = sourcePos + ind[j] * sourceTile->qstride[j] * sourceTile->origAcumCard[j+1];
				//targetPos = targetPos + ind[j] * targetTile->qstride[j] * targetTile->origAcumCard[j+1];
				/* @arturo Mar 2014, BUG CORRECTED: MULTIPLY BY THE RATIO BETWEEN STRIDES */
					sourcePos = sourcePos + ind[j] * relativeStrideSource[j] * sourceTile->origAcumCard[j+1];
					targetPos = targetPos + ind[j] * relativeStrideTarget[j] * targetTile->origAcumCard[j+1];
				}
#ifdef DEBUG
fprintf(stderr,"CTRL UPDATE-%s sourcePos=%d\n", strFromTo[ fromTo ], sourcePos);
fprintf(stderr,"CTRL UPDATE-%s targetPos=%d\n", strFromTo[ fromTo ], targetPos);
#endif

				char *sourcePointer = (char *)(sourceTile->data) 
										+ (size_t)sourcePos * shadow->baseExtent;
				char *targetPointer = (char *)(targetTile->data) 
										+ (size_t)targetPos * shadow->baseExtent;

				size_t loopJumpSource = (size_t)( relativeStrideSource[dim] * sourceTile->origAcumCard[dim+1] ) * shadow->baseExtent;
				size_t loopJumpTarget = (size_t)( relativeStrideTarget[dim] * targetTile->origAcumCard[dim+1] ) * shadow->baseExtent;

				/* 5.2.2. COPY DATA AT THE NEXT POSITION */
				/* @arturo Apr 2014: OPTIMIZATION, INNER LOOP */
				int innerLoopIdx;
				for ( innerLoopIdx = 0; innerLoopIdx < shadow->card[dim]; 
						innerLoopIdx++, 
						sourcePointer += loopJumpSource,
						targetPointer += loopJumpTarget
						) {
					memcpy( targetPointer, sourcePointer, chunkMemSize );
				}
				/* 5.2.3. ADVANCE INDECES */
				/* @arturo Apr 2014: OPTIMIZATION, INNER LOOP */
				//for(j=dim; j>=0; j--) 
				for(j=dim-1; j>=0; j--) {
					ind[j]++;
					if (ind[j]==shadow->card[j]) ind[j]=0;
					else break;
				}
			}
		}
	}
	/* 5.3. HIT_TILE TYPE ELEMENTS */
	else if ( shadowOrig->hierDepth > 0 ) {
		for(i=0; i< (int)((size_t)shadow->acumCard/chunkSize); i++) {
			int sourcePos = 0;
			int targetPos = 0;

			/* 5.3.1. COMPUTE NEXT POSITIONS IN THE shadow AND ORIGINAL VARIABLES */
			for(j=0; j< dim+1; j++) {
				sourcePos = sourcePos + ind[j] * sourceTile->qstride[dim] * sourceTile->origAcumCard[j+1];
				targetPos = targetPos + ind[j] * targetTile->qstride[dim] * targetTile->origAcumCard[j+1];
			}
#ifdef DEBUG
fprintf(stderr,"CTRL UPDATE-%s sourcePos=%d\n", strFromTo[ fromTo ], sourcePos);
fprintf(stderr,"CTRL UPDATE-%s targetPos=%d\n", strFromTo[ fromTo ], targetPos);
#endif

			/* 5.3.2. COPY DATA AT THE NEXT POSITION */
			for( k=0; k < chunkSize; k++ ) {
				hit_tileCloneInternal( 
						(char *)targetTile->data + (k+(size_t)targetPos) * shadow->baseExtent, 
						(char *)sourceTile->data + (k+(size_t)sourcePos) * shadow->baseExtent, 
						name, file, numLine);
			}

			/* 5.3.3. ADVANCE INDECES */
			for(j=dim; j>=0; j--) {
				ind[j]++;
				if (ind[j]==shadow->card[j]) ind[j]=0;
				else break;
			}
		}
	}
	// 5.4. PARANOID CHECK: IMPOSIBLE VALUE IN FIELD
	else hit_errInternal(__FUNCTION__, "Negative value in hierarchy-level field of variable:", name, file, numLine);

	/* 6. RETURN */
	return;
}

/* @arturo Feb 2013 */
/* Hit GENERATE THE SHAPE OF A TILE IN LOCAL COORDINATES */
HitShape hit_tileShapeLocal( const void *inTile ) {
	HitTile tile = *(const HitTile *)inTile;
	HitShape newShp = HIT_SHAPE_NULL;

	hit_sshapeDims( newShp ) = hit_tileDims( tile );

	int dim;
	for ( dim=0; dim < hit_tileDims( tile ); dim++ ) 
		hit_shapeSig( newShp, dim ) = hit_sigStd( hit_tileDimCard( tile, dim ) );

	return newShp;
}

/* Hit TRANSFORM TILE COORDINATES TO ARRAY COORDINATES */
HitShape hit_tileShapeTile2Array(void *var, HitShape sh) {
	int i;
	HitTile *v = (HitTile *) var;
	HitShape newShp=HIT_SHAPE_NULL_STATIC;  
	hit_shapeDimsSet(newShp,hit_shapeDims(sh));
	for(i=0;i<hit_shapeDims(sh);i++) {
		// TRANSLATION FOR EACH DIMENSION
		hit_shapeSig(newShp,i).begin = hit_shapeSig(sh,i).begin * hit_shapeSig(v->shape,i).stride + hit_shapeSig(v->shape,i).begin ;
		hit_shapeSig(newShp,i).end = hit_shapeSig(sh,i).end * hit_shapeSig(v->shape,i).stride + hit_shapeSig(v->shape,i).begin ;
		if ( hit_shapeSig(newShp,i).begin == hit_shapeSig(newShp,i).end ) 
			hit_shapeSig(newShp,i).stride=1;
		else 
			hit_shapeSig(newShp,i).stride = hit_shapeSig(sh,i).stride * hit_shapeSig(v->shape,i).stride; 
	}
	return newShp;
}

/* Hit TRANSFORM TILE COORDINATES TO ARRAY COORDINATES */
/* 
 * Modificated to avoid error with divisible strides 
 * @see hit_shapeArrayToTile
 * @author Maria Sanchez
 */
HitShape hit_tileShapeArray2Tile(void *var, HitShape sh) {
	int i;
	HitTile *v = (HitTile *) var;
	HitShape newShp=HIT_SHAPE_NULL_STATIC;
	hit_shapeDimsSet(newShp,hit_shapeDims(sh));
	int minStride, maxStride;
	for(i=0;i<hit_shapeDims(sh);i++) {
		if (hit_shapeSig(sh,i).stride > hit_shapeSig(v->shape,i).stride){
			maxStride = hit_shapeSig(sh,i).stride;
			minStride = hit_shapeSig(v->shape,i).stride;
		}
		else{
			maxStride = hit_shapeSig(v->shape,i).stride;
			minStride = hit_shapeSig(sh,i).stride;
		}

		if (maxStride % minStride != 0) return HIT_SHAPE_NULL;

		// TRANSLATION FOR EACH DIMENSION
		hit_shapeSig(newShp,i).begin = (hit_shapeSig(sh,i).begin - hit_shapeSig(v->shape,i).begin) / hit_shapeSig(v->shape,i).stride;
		hit_shapeSig(newShp,i).end = (hit_shapeSig(sh,i).end - hit_shapeSig(v->shape,i).begin) / hit_shapeSig(v->shape,i).stride;
		if ( hit_shapeSig(newShp,i).begin == hit_shapeSig(newShp,i).end ) 
			hit_shapeSig(newShp,i).stride=1;
		else 
			hit_shapeSig(newShp,i).stride = maxStride / minStride;
	}
	return newShp;
}


/* Hit FREE DATA: WORKS RECURSIVELY IN A HIERARCHICAL TILE */
void hit_tileFreeRecInternal(void * varP) {

	HitTile *var = (HitTile *)varP;
	int i;
	//int j, ind[HIT_MAXDIMS];
	//size_t offset;
	void * ptr;
	if ( var->memStatus == HIT_MS_OWNER) { 
		ptr=var->data;
		for(i=0;i<var->acumCard;i++) {
			hit_tileFree(*((HitTile *)ptr));
			ptr = (char *)ptr + var->baseExtent;
		}
	}
	// May 2015 @arturo BUG CORRECTED: NOT OWNER SHOULD NOT FREE ANYTHING 
	/*
	else if	( var->memStatus == HIT_MS_NOT_OWNER) {
		//initialise ind with 0
		memset(ind,0, (size_t)hit_shapeDims(var->shape)*sizeof(int));
		for(i=0;i<var->acumCard;i++) {
			offset=0;
			for(j=0;j<hit_shapeDims(var->shape);j++) {
				offset+=(size_t)ind[j]*(size_t)var->qstride[j]*(size_t)var->origAcumCard[j+1];
			}
			offset+=(size_t)ind[j]*(size_t)var->qstride[j];
			hit_tileFree(*((HitTile *)( (char *)var->data+offset*var->baseExtent )));
			j=0;
			do {
				ind[j]=(ind[j]+1)% (var->card[j]);
				j++;
			} while(j<hit_shapeDims(var->shape) && ind[j-1]==0 && var->card[j]!=0);
		}
	}
	*/
}


/* @arturo Feb 2013 */
/* HitTile GLUE TILES WITH TOUCHING BOUNDARIES OF THE SAME GEOMETRY */
/* TODO: Extent to more than one dimension */
void hit_tileGlue( void *tileInA, void *tileInB, void *tileOut ) {
	HitTile *inA = (HitTile *)tileInA;
	HitTile *inB = (HitTile *)tileInB;
	HitTile *out = (HitTile *)tileOut;

	/* 1.1. CHECK: BOTH NOT NULL SHAPES, RETURN NULL */
	if ( hit_shapeCmp( hit_tileShape( *inA ), HIT_SHAPE_NULL )
			&&	
			hit_shapeCmp( hit_tileShape( *inB ), HIT_SHAPE_NULL )
		) {
		*out = HIT_TILE_NULL;
		return;
	}
	/* 1.2. CHECK: A SHAPE IS NULL, RETURN B */
	else if ( hit_shapeCmp( hit_tileShape( *inA ), HIT_SHAPE_NULL ) ) {
		*out = *inB;
		return;
	}
	/* 1.3. CHECK: B SHAPE IS NULL, RETURN A */
	else if ( hit_shapeCmp( hit_tileShape( *inB ), HIT_SHAPE_NULL ) ) {
		*out = *inA;
		return;
	}

	/* 0. CHECK THAT INPUT SHAPES HAVE EXACTLY ONE DIMENSION */
	if ( hit_tileDims( *inA ) != 1 || hit_tileDims( *inB ) != 1 ) {
		hit_warnInternal(__FUNCTION__, "Sorry, Glue for tiles with more than one dimension not yet implemented.", "", __FILE__, __LINE__ );
		*out = HIT_TILE_NULL;
		return;
	}

	/* 2. CHECK GLUE CONDITION: NOT TOUCHING SHAPES WITH THE SAME STRIDE, RETURN NULL */
	if ( hit_tileDimStride( *inA, 0 ) != hit_tileDimStride( *inB, 0 )
			||
			hit_tileDimBegin( *inB, 0 ) - hit_tileDimEnd( *inA, 0 ) != hit_tileDimStride( *inA, 0 )
		) {
		*out = HIT_TILE_NULL;
		return;
	}

	/* 3. GLUE: ALLOCATE NEW TILE */
	HitShape newShp = hit_shape( 1, 
			hit_sig( 
				hit_tileDimBegin( *inA, 0 ), hit_tileDimEnd( *inB, 0 ), hit_tileDimStride( *inA, 0 )
			) );
	hit_tileDomainShapeInternal( out, inA->baseExtent, HIT_NONHIERARCHICAL, newShp );
	hit_tileAlloc( out );

	/* 4. COPY DATA */
	memcpy( out->data, inA->data, inA->baseExtent * (size_t)hit_tileCard( *inA ) );
	memcpy( (char *)(out->data) + inA->baseExtent * (size_t)hit_tileCard(*inA), inB->data, inB->baseExtent * (size_t)hit_tileCard( *inB ) );

	/* 5. FREE ORIGINAL TILES */
	hit_tileFree( *inA );
	hit_tileFree( *inB );
}
