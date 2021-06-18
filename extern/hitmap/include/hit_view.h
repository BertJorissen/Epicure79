/**
* Views: Transformed shapes
*
* These functionalities allows to create a single data structure representing an
* ordered collection of actions to modify, displace, strech, etc. a shape.
*
* @file hit_view.h
* @ingroup Tiling
* @version 1.0
* @author Arturo Gonzalez-Escribano
* @date Mar 2019
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

#ifndef _HitView_
#define _HitView_

#include<stdio.h>
#include<stdlib.h>

/* Hit View */
/**
 * @struct HitView
 *
 * ADT for collections of shape transformation actions
 */
typedef struct {
	char dimension;	/**< The dimension to alter, or HIT_VIEW_ALLDIM */
	char action;	/**< The action to apply to the shape */
	int  offset;	/**< An offset or absolute index number, depending on the action */
} HitShpViewRecord;

typedef struct {
	int num_transforms;		/**< Number of transformations in the view */
	HitShpViewRecord *transforms;	/**< List of tuples with the actions specifications */
} HitShpView;

/* Hit VIEW STATIC NULL CONSTANT */
/**
 * Null value for views, with no transformation.
 */
#define HIT_SHPVIEW_NULL_STATIC	{ 0, NULL }
//static HitShpView HIT_SHPVIEW_NULL = HIT_SHPVIEW_NULL_STATIC;


/* Internal helpers for View Constructor: */
/* Initializer of View */
static inline HitShpView hit_shpViewInternalInit( int num ) {
	HitShpView result = HIT_SHPVIEW_NULL_STATIC;
	if ( num == 0 ) return result;
	result.num_transforms = num;
	result.transforms = (HitShpViewRecord *)malloc( sizeof(HitShpViewRecord) * (size_t)num );
	return result;
}
/* Update of a trnasformation in an initializaed view */
static inline HitShpView hit_shpViewInternalUpdate( int num, char dim, char action, int offset, HitShpView var ) {
	var.transforms[ num ].dimension = dim;
	var.transforms[ num ].action = action;
	var.transforms[ num ].offset = offset;
	return var;
}
/* Transform view transformation tuple into a parameter list */
#define HIT2_SHPVIEWRECORD_LIST( a,b,c )	a, b, c

/**
 * Constructor of view values.
 *
 * @hideinitializer
 *
 * @param[in] ... List of up to 10 transformation tuples with the format: ( dim, action, offset )
 * @return A view variable.
 */
#define hitShpView( ... ) HIT2_SHPVIEW_VARIADIC( HIT2_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
#define HIT2_SHPVIEW_VARIADIC( num, ... )	HIT2_SHPVIEW_VARIADIC_2( num, __VA_ARGS__ )
#define HIT2_SHPVIEW_VARIADIC_2( num, ... )	HIT2_SHPVIEW_##num( num, __VA_ARGS__ )

#define HIT2_SHPVIEW_1( num, tuple )	hit_shpViewInternalUpdate( num-1, HIT2_SHPVIEWRECORD_LIST tuple, hit_shpViewInternalInit( num ) )
#define HIT2_SHPVIEW_2( num, tuple, ... ) hit_shpViewInternalUpdate( num-2, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_1( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_3( num, tuple, ... ) hit_shpViewInternalUpdate( num-3, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_2( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_4( num, tuple, ... ) hit_shpViewInternalUpdate( num-4, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_3( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_5( num, tuple, ... ) hit_shpViewInternalUpdate( num-5, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_4( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_6( num, tuple, ... ) hit_shpViewInternalUpdate( num-6, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_5( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_7( num, tuple, ... ) hit_shpViewInternalUpdate( num-7, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_6( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_8( num, tuple, ... ) hit_shpViewInternalUpdate( num-8, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_7( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_9( num, tuple, ... ) hit_shpViewInternalUpdate( num-9, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_8( num, __VA_ARGS__ ) )
#define HIT2_SHPVIEW_10( num, tuple, ... ) hit_shpViewInternalUpdate( num-10, HIT2_SHPVIEWRECORD_LIST tuple, HIT2_SHPVIEW_9( num, __VA_ARGS__ ) )

/**
 * Free a View variable
 *
 * @hideinitializer
 *
 * @param[in,out] view A view to be reset to null value
 */
#define hit_shpViewFree( trans ) { (trans).num_transforms = 0; free( (trans).transforms ); (trans).transforms = NULL; }

/* END OF HEADER FILE _HitView_ */
#endif
