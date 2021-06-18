/**
* Definition of the HitShape types for multidimensional
* domain shapes. Constructors and functions to manipulate them.
*
* @file hit_shape.c
* @version 1.1
* @author Arturo Gonzalez-Escribano
* @author Javier Fresno Bausela
* @author Carlos de Blas Carton
* @date Mar 2011
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
#include <limits.h>
#include <hit_allocP.h>
#include <hit_sshape.h>
#include <hit_cshape.h>
#include <hit_bshape.h>
#include <unistd.h>
#include <hit_funcop.h>
#include <hit_error.h>



/* 2. Hit SHAPE NULL CONSTANTs */
HitShape	HIT_SHAPE_NULL = HIT_SHAPE_NULL_STATIC;
// @author arturo
HitShape	HIT_SIG_SHAPE_NULL = HIT_SIG_SHAPE_NULL_STATIC;
// @author arturo
#ifdef __cplusplus
HitShape	HIT_CSR_SHAPE_NULL.type = HIT_CSR_SHAPE;
			HIT_CSR_SHAPE_NULL.info.csr = HIT_CSR_SHAPE_INTERNAL_NULL_STATIC;
HitShape	HIT_BITMAP_SHAPE_NULL.type = HIT_BITMAP_SHAPE;
			HIT_BITMAP_SHAPE_NULL.info.csr = HIT_BITMAP_SHAPE_INTERNAL_NULL_STATIC;
#else
HitShape	HIT_CSR_SHAPE_NULL = HIT_CSR_SHAPE_NULL_STATIC;
HitShape	HIT_BITMAP_SHAPE_NULL = HIT_BITMAP_SHAPE_NULL_STATIC;
#endif

/* 3. Hit SHAPE WHOLE CONSTANT */
HitShape	HIT_SHAPE_WHOLE = HIT_SHAPE_WHOLE_STATIC;
// @author arturo
HitSigShape	HIT_SIG_SHAPE_WHOLE = HIT_SIG_SHAPE_WHOLE_STATIC;

/* 4. Hit Name List constant */
HitNameList HIT_NAMELIST_NULL = HIT_NAMELIST_NULL_STATIC;


/* 6. HIT SHAPE FREE FUNCTION */
void hit_shapeFree(HitShape shape){

	switch (hit_shapeType(shape)){
		// Do nothing if it is a Signature Shape.
		case HIT_SIG_SHAPE:
			return;
		// CSR Shape.
		case HIT_CSR_SHAPE:
			hit_cShapeFree(shape);
			return;
		// Bitmap Shape.
		case HIT_BITMAP_SHAPE:
			hit_bShapeFree(shape);
			return;
		default:
			hit_errInternal(__func__, "Unsupported shape type", "", __FILE__, __LINE__);
			break;
	}



}


/* 7. Hit SHAPE: COMPARISON OPERATOR */
int hit_shapeCmp(HitShape sh1, HitShape sh2) {

	// Only works for Signature Shape.
	if(	hit_shapeType(sh1) == HIT_CSR_SHAPE ||
		hit_shapeType(sh2) == HIT_CSR_SHAPE	) return 0;

	if (hit_shapeDims(sh1) != hit_shapeDims(sh2) ) return 0;
	int i;
	for (i=0; i<hit_shapeDims(sh1); i++)
		if ( ! hit_sigCmp( hit_shapeSig(sh1,i), hit_shapeSig(sh2,i) ) ) return 0;
	return 1;
}

/* 8. Hit INTERSECT SHAPES */
HitShape hit_shapeIntersect(HitShape sh1, HitShape sh2) {

	/* 0. ONLY FOR SIGNATURE SHAPE */
	if(	hit_shapeType(sh1) != HIT_SIG_SHAPE ||
		hit_shapeType(sh2) != HIT_SIG_SHAPE	) return HIT_SHAPE_NULL;

	HitShape res = HIT_SHAPE_NULL;

	/* 1. DIFFERENT DIMENSIONS, RETURN EMPTY SHAPE */
	if (hit_shapeDims(sh1) != hit_shapeDims(sh2)) return res;

	/* 2. FOR EACH DIMENSION IN BOTH SHAPES. OBSOLETE: DIMENSIONS ARE EQUAL */
	int minDims = ( hit_shapeDims(sh1) < hit_shapeDims(sh2) ) ? hit_shapeDims(sh1) : hit_shapeDims(sh2);

	int i;
	int null = 0; // FLAG: CHECK IF ALL DIMENSIONS HAVE NON-EMPTY INTERSECTION
	for (i=0; i<minDims; i++) {
		/* 2.1. COMPUTE INTERSECTION */
		hit_shapeSig(res,i) = hit_sigIntersect( hit_shapeSig(sh1,i), hit_shapeSig(sh2,i) );

		/* 2.2. CHECK IF THERE IS A NON-NULL SIGNATURE */
		if ( hit_sigCmp( hit_shapeSig(res,i), HIT_SIG_NULL ) ) null = 1;
	}

	/* 3. IF NO DIMENSIONAL INTERSECTION, RETURN NULL SHAPE */
	if ( null ) return HIT_SHAPE_NULL;

	/* 4. RETURN NON-EMPTY INTERSECTION */
	hit_shapeDimsSet( res, minDims );
	return res;
}


/* 8.2. Hit SUBSELECTION OF SHAPES */
HitShape hit_shapeSubset(HitShape sh1, HitShape sh2) {

	/* 0. ONLY FOR SIGNATURE SHAPE */
	if(	hit_shapeType(sh1) != HIT_SIG_SHAPE ||
		hit_shapeType(sh2) != HIT_SIG_SHAPE	) return HIT_SHAPE_NULL;

	/* 1. DIFFERENT DIMENSIONS, RETURN EMPTY SHAPE */
	if (hit_shapeDims(sh1) != hit_shapeDims(sh2)) return HIT_SHAPE_NULL;

	/* 2. FOR EACH DIMENSION IN BOTH SHAPES: OBSOLETE IF DIMENSIONS ARE EQUAL */
	HitShape res = HIT_SHAPE_NULL;
	int minDims = ( hit_shapeDims(sh1) < hit_shapeDims(sh2) ) ? hit_shapeDims(sh1) : hit_shapeDims(sh2);
	int i;
	for (i=0; i<minDims; i++) {
		/* 2.1. COMPUTE SELECTION */
		hit_shapeSig(res,i) = hit_sigBlend( hit_shapeSig(sh1,i), hit_shapeSig(sh2,i) );
	}

	/* 3. RETURN */
	hit_shapeDimsSet( res, minDims );
	return res;
}

/* 8.3. Hit TRANSFORM COORDINATES */
HitShape hit_shapeTileToArray(HitShape sh1, HitShape sh2) {

	/* 0. ONLY FOR SIGNATURE SHAPE */
	if(	hit_shapeType(sh1) != HIT_SIG_SHAPE ||
		hit_shapeType(sh2) != HIT_SIG_SHAPE	) return HIT_SHAPE_NULL;

	/* 1. DIFFERENT DIMENSIONS, RETURN EMPTY SHAPE */
	if (hit_shapeDims(sh1) != hit_shapeDims(sh2)) return HIT_SHAPE_NULL;

	/* 2. FOR EACH DIMENSION IN BOTH SHAPES: OBSOLETE IF DIMENSIONS ARE EQUAL */
	HitShape res = HIT_SHAPE_NULL;
	int minDims = ( hit_shapeDims(sh1) < hit_shapeDims(sh2) ) ? hit_shapeDims(sh1) : hit_shapeDims(sh2);
	int i;
	for (i=0; i<minDims; i++) {
		/* 2.1. COMPUTE TRANSFORMATION */
		hit_shapeSig(res,i).begin =
				hit_sigTileToArray( hit_shapeSig(sh1,i), hit_shapeSig(sh2,i).begin );
		hit_shapeSig(res,i).end =
				hit_sigTileToArray( hit_shapeSig(sh1,i), hit_shapeSig(sh2,i).end );
		hit_shapeSig(res,i).stride = hit_shapeSig(sh1,i).stride * hit_shapeSig(sh2,i).stride;
	}

	/* 3. RETURN */
	hit_shapeDimsSet( res, minDims );
	return res;
}

HitShape hit_shapeArrayToTile(HitShape sh1, HitShape sh2) {

	/* 0. ONLY FOR SIGNATURE SHAPE */
	if(	hit_shapeType(sh1) != HIT_SIG_SHAPE ||
		hit_shapeType(sh2) != HIT_SIG_SHAPE	) return HIT_SHAPE_NULL;

	/* 1. DIFFERENT DIMENSIONS, RETURN EMPTY SHAPE */
	if (hit_shapeDims(sh1) != hit_shapeDims(sh2)) return HIT_SHAPE_NULL;


	/* 2. FOR EACH DIMENSION IN BOTH SHAPES: OBSOLETE IF DIMENSIONS ARE EQUAL */
	HitShape res = HIT_SHAPE_NULL;
	//int minDims = ( hit_shapeDims(sh1) < hit_shapeDims(sh2) ) ? hit_shapeDims(sh1) : hit_shapeDims(sh2);
	int minDims = hit_shapeDims(sh1);
	int i, minStride, maxStride;
	for (i=0; i<minDims; i++) {
		if (hit_shapeSig(sh1,i).stride > hit_shapeSig(sh2,i).stride){
			minStride = hit_shapeSig(sh2,i).stride;
			maxStride = hit_shapeSig(sh1,i).stride;
		}
		else {
			minStride = hit_shapeSig(sh1,i).stride;
			maxStride = hit_shapeSig(sh2,i).stride;
		}
		/* 2.1 IF SIGS AREN'T COMPATIBLE, RETURN EMPTY SHAPE */
		if (maxStride % minStride != 0) return HIT_SHAPE_NULL;

		/* 2.2. COMPUTE TRANSFORMATION */
		hit_shapeSig(res,i).begin =
				hit_sigArrayToTile( hit_shapeSig(sh1,i), hit_shapeSig(sh2,i).begin );
		hit_shapeSig(res,i).end =
				hit_sigArrayToTile( hit_shapeSig(sh1,i), hit_shapeSig(sh2,i).end );
		//hit_shapeSig(res,i).stride = hit_shapeSig(sh1,i).stride * hit_shapeSig(sh2,i).stride;
		hit_shapeSig(res,i).stride = maxStride/minStride;

		//if(hit_shapeSig(res,i).stride < 1) hit_shapeSig(res,i).stride = 1;
	}

	/* 3. RETURN */
	hit_shapeDimsSet( res, minDims );
	return res;
}

/* 9.1 Hit SHAPE EXPAND MANY DIMS IN EQUAL OFFSET */
HitShape hit_shapeExpand(HitShape shape,int dims,int offset){

	// Only works for Signature Shape.
	if(	hit_shapeType(shape) != HIT_SIG_SHAPE ) return HIT_SHAPE_NULL;

	HitShape res = shape;

	int dim;
	for(dim=0;dim<dims;dim++){
		hit_shapeSig(res,dim).begin-=offset;
		hit_shapeSig(res,dim).end+=offset;
	}
	return res;
}

/* 9.2 Hit SHAPE EXPAND A DIM IN A GIVEN DIRECTION */
HitShape hit_shapeDimExpand(HitShape shape,int dim, int position, int offset ){

	// Only works for Signature Shape.
	if(	hit_shapeType(shape) != HIT_SIG_SHAPE ) return HIT_SHAPE_NULL;

	HitShape res = shape;

	if ( position == HIT_SHAPE_BEGIN ) hit_shapeSig(res,dim).begin -= offset;
	else hit_shapeSig(res,dim).end += offset;

	return res;
}


/* 9.3 Hit SHAPE GET BORDER */
HitShape hit_shapeBorder(HitShape shape, int dim, int position, int offset){

	// Only works for Signature Shape.
	if(	hit_shapeType(shape) == HIT_CSR_SHAPE ) return HIT_SHAPE_NULL;

	HitShape res = shape;

	if(position == HIT_SHAPE_BEGIN){
		hit_shapeSig(res,dim).begin -= offset;
		hit_shapeSig(res,dim).end = hit_shapeSig(res,dim).begin;
	} else {
		hit_shapeSig(res,dim).end += offset;
		hit_shapeSig(res,dim).begin = hit_shapeSig(res,dim).end;
	}
	return res;
}

// @arturo Oct, 31st 2018
/* 9.4 Hit SHAPE TRANSFORM (MOVE,EXPAND,CUT,ETC...) */
HitShape hit_shapeTransform( HitShape shape, int dim, int action, int offset ){

	// Only works for Signature Shape.
	if( hit_shapeType(shape) == HIT_CSR_SHAPE ) return HIT_SHAPE_NULL;

	HitShape res = shape;

	// Raw vs. shape coordinates (take into account the strides by default)
	int strideFlag = 1;
	if ( action >= HIT_SHAPE_RAW ) {
		strideFlag = 0;
		action = action - HIT_SHAPE_RAW;
	}

	// Dimensions to transform
	int dimStart, dimEnd;
	if ( dim == HIT_SHAPE_ALLDIMS ) {
		dimStart = 0;
		dimEnd = hit_shapeDims( res )-1;
	}
	else dimStart = dimEnd = dim;

	int index;
	for( index=dimStart; index<=dimEnd; index++ ){
		// Offset in saw vs. strided coordinates for this dimension
		int dimOffset = offset;
		if ( strideFlag ) dimOffset *= hit_shapeSig(res,index).stride;

		// Apply action
		if ( action == HIT_SHAPE_BEGIN ) {
			hit_shapeSig(res,index).begin += dimOffset;
		}
		else if ( action == HIT_SHAPE_END ) {
			hit_shapeSig(res,index).end += dimOffset;
		}
		else if ( action == HIT_SHAPE_MOVE ) {
			hit_shapeSig(res,index).begin += dimOffset;
			hit_shapeSig(res,index).end += dimOffset;
		}
		else if ( action == HIT_SHAPE_STRETCH ) {
			hit_shapeSig(res,index).begin -= dimOffset;
			hit_shapeSig(res,index).end += dimOffset;
		}
		else if ( action == HIT_SHAPE_FIRST ) {
			// offset means here a number of selected elements
			hit_shapeSig(res,index).end = hit_shapeSig(res,index).begin + dimOffset -1;
		}
		else if ( action == HIT_SHAPE_LAST ) {
			// offset means here a number of selected elements
			hit_shapeSig(res,index).begin = hit_shapeSig(res,index).end - dimOffset +1;
		}
	}

	// Return resulting shape
	return res;
}




/* 12 */
int hit_nameListName2Index(HitNameList list, int name){

	// A. There is a inv array.
	if( list.flagNames == HIT_SHAPE_NAMES_ARRAY ){
		if(name >= list.nInvNames) return -1;
		return list.invNames[name];
	}

	// B. There is no inv array, but the names are ordered from 0 to N.
	if( list.flagNames == HIT_SHAPE_NAMES_ORDERED ){
		if( name < 0 || name >= list.nNames ) return -1;
		return name;
	}

	// C. There is no inv array, we have to loop through all the vertices.
	if( list.flagNames == HIT_SHAPE_NAMES_NOARRAY ){
		// Search the local name.
		int i;
		for(i=0; i<list.nNames; i++){
			if(list.names[i] == name) return i;
		}
		return -1;
	}

	return -1;
}



void hit_nameListFree(HitNameList list){

	hit_free(list.names);

	if(list.flagNames == HIT_SHAPE_NAMES_ARRAY){
		hit_free(list.invNames);
	}
}


void hit_nameListCreate(HitNameList * list, int nelems){

	*list = HIT_NAMELIST_NULL;

	list->nNames = nelems;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(list->names,(size_t) nelems * sizeof(int),int*);
	hit_malloc(list->names, int, nelems );

	// Fill the global names array
	int i;
	for(i=0;i<nelems;i++){
		list->names[i] = i;
	}
	list->flagNames = HIT_SHAPE_NAMES_ORDERED;
}

void hit_nameListClone(HitNameList * dst, HitNameList * src){

	*dst = HIT_NAMELIST_NULL;

	dst->nNames = src->nNames;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(dst->names,(size_t) dst->nNames * sizeof(int),int*);
	hit_malloc(dst->names, int, dst->nNames );

	memcpy(dst->names, src->names, (size_t) dst->nNames * sizeof(int) );

	dst->flagNames = HIT_SHAPE_NAMES_ORDERED;


}




void hit_nameListAdd(HitNameList * list, int x){

	if(list->nNames == 0){

		list->nNames = 1;
		// @arturo Ago 2015: New allocP interface
		// hit_malloc(list->names,sizeof(int),int*);
		hit_malloc(list->names, int, 1);
		list->names[0] = x;

	} else {

		list->nNames++;
		// @arturo Ago 2015: New allocP interface
		// hit_realloc(list->names,(size_t) list->nNames * sizeof(int),int*);
		hit_realloc(list->names, int, list->nNames );
		list->names[list->nNames-1] = x;

	}

	list->flagNames = HIT_SHAPE_NAMES_NOARRAY;
}


void hit_nameListCreateInvNames(HitNameList * list){

	// 1. Set the flag.
	list->flagNames = HIT_SHAPE_NAMES_ARRAY;

	// 2. Get the maximum name that will determine the size of the array.
	int name_max = 0;
	int i;
	for(i=0; i<list->nNames; i++){
		name_max = hit_max(name_max,list->names[i]);
	}
	name_max++;
	list->nInvNames = name_max;

	// 3. Allocate the array.
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(list->invNames, (size_t) (name_max+1) * sizeof(int),int*);
	hit_malloc(list->invNames, int, name_max+1 );

	// 4. Set the array values.
	for(i=0; i<name_max; i++){
		list->invNames[i] = -1;
	}
	for(i=0; i<list->nNames; i++){
		list->invNames[ list->names[i] ] = i;
	}
}

/* PRINT A SHAPE*/
void
dumpShape (HitShape sh)
{

  int i;
  printf ("[%d:%d:%d", hit_shapeSig (sh, 0).begin, hit_shapeSig (sh, 0).end,
          hit_shapeSig (sh, 0).stride);
  for (i = 1; i < hit_shapeDims (sh); i++)
    printf (",%d:%d:%d", hit_shapeSig (sh, i).begin, hit_shapeSig (sh, i).end,
            hit_shapeSig (sh, i).stride);
  printf ("] \t cards: [%d", hit_sigCard (hit_shapeSig (sh, 0)));
  for (i = 1; i < hit_shapeDims (sh); i++)
    printf (",%d", hit_sigCard (hit_shapeSig (sh, i)));
  printf ("]\n");
}
