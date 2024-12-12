/**
 * Functionalities to manipulate Domains of multiple Signature-based shapes.
 *
 * Manipulation of HitSigDomain: Non-overlapping collections of multidimensional 
 * shapes for unions of dense domains of indexes.
 *
 * @file hit_domain.h
 * @ingroup Tiling
 * @version 1.0
 * @author Arturo Gonzalez-Escribano
 * @date Dec 2018
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

#ifndef _HitSigDomain_
#define _HitSigDomain_

#include <stdio.h>
#include <stdlib.h>
#include "hit_sig.h"
#include "hit_sshape.h"

/**
 * @struct HitSigDomain
 * ADT for collections of multidimensional Signature domains.
 *
 * A signature domain is a list of signature shapes.
 * The function to add new shapes to the union ensures that all the domain indexes 
 * represented in the added shapes are represented in the collection, and that no pair of 
 * shapes in the collection overlaps. The shapes in the collection are built to keep the 
 * maximum possible continuity of elements in the last dimension (for easy memory copy
 * or access in row-major order), and to reduce the number of shapes.
 *
 * These domains allow to define unions of dense array shapes, in a form that can be 
 * directly used to efficiently marshall/unmarshall data on communication buffers.
 *
 * Member functions are documented in \ref hit_domain.h.
 *
 * @ingroup Tiling
 * @implements HitSigDomain
 */

/* 1. STRUCTURE AND SPECIAL VALUES */
// @cond INTERNAL
/**
 * Data structure
 */
typedef struct {
	int		num;
	HitSigShape	*list;
} HitSigDomain;

/**
 * Null internal static value for Signature Domains
 */
#define HIT_SIG_DOMAIN_NULL_STATIC	{ 0, NULL }

/**
 * Adding shapes to a list without doing union or normalization
 */
#define hit_domainAddShape( dom, shp )	hit_domainAddSigShape( &(dom), shp.info.sig )
void hit_domainAddSigShape( HitSigDomain *dom, HitSigShape sh );

/**
 * Print domain info in stdout for debug purposes
 */
void hit_domainPrint( HitSigDomain dom );

// @endcond


/**
 * @name Initializers, constructors
 */
/**@{*/

/* 2. HitSigDomain generating functions */
/**
 * Constructor (1) of Signature-Domain collections.
 *
 * @hideinitializer
 *
 * @retval	\e HitSigDomain  A empty domain.
 */
static inline HitSigDomain hit_domainEmpty() {
	HitSigDomain res = HIT_SIG_DOMAIN_NULL_STATIC;
	return res;
}

/**
 * Constructor (2) of Signature-Domain collections.
 *
 * It builds a new domain with a single shape in the collection.
 *
 * @hideinitializer
 *
 * @param[in] shp 	\e HitShape The innitial shape.
 * @retval	\e HitSigDomain  A new domain with a single shape.
 */
static inline HitSigDomain hit_domainNew( HitShape shp ) {
	HitSigDomain res = HIT_SIG_DOMAIN_NULL_STATIC;
	hit_domainAddShape( res, shp );
	return res;
}

/**@}*/

/**
 * @name Retrieving/Setting shape information
 */
/**@{*/

/**
 * Number of shapes in a domain.
 *
 * @hideinitializer
 *
 * @param[in] domain \e HitSigDomain A domain of signature shapes.
 * @retval	\e int  Number of shapes in the domain.
 */
#define hit_domainSize( dom )	(dom).num

/**
 * Retrieve the shape with a particular index in the domain
 *
 * @hideinitializer
 *
 * @param[in] dom \e HitSigDomain A domain of signature shapes.
 * @param[in] idx \e int A valid index of signature in the domain.
 * @retval	\e HitShape  The shape with the given index.
 */
#define hit_domainElem( dom, idx ) hit_shapeFromSigShape( (dom).list[idx] )

/**@}*/

/**
 * @name Basic operations
 */
/**@{*/

/**
 * Clear the information in a domain, make it an empty set
 *
 * @hideinitializer
 *
 * @param[in] dom \e HitSigDomain A domain of signature shapes.
 */
#define hit_domainClear( dom )  { free( (dom).list ); (dom).num=0; (dom).list=NULL; }

/**
 * Upgrade a domain with the union of a new shape
 *
 * @hideinitializer
 *
 * @param[in] dom \e HitSigDomain A domain of signature shapes.
 * @param[in] shp \e HitShape A signature shape.
 */
#define hit_domainShapeUnion( dom, shp )	hit_domainSigShapeUnion( &(dom), shp.info.sig )
void hit_domainSigShapeUnion( HitSigDomain *old, HitSigShape shp_new );


/**
 * Compute the intersection of two domains
 *
 * @hideinitializer
 *
 * @param[in] dom1 \e HitSigDomain A domain of signature shapes.
 * @param[in] dom2 \e HitSigDomain A domain of signature shapes.
 * @retval	\e HitSigDomain The domain representing the intersection of the input domains.
 */
#define hit_domainIntersection( dom1, dom2 )	hit_domainSigIntersection( &(dom1), &(dom2) )
HitSigDomain hit_domainSigIntersection( HitSigDomain *dom1, HitSigDomain *dom2 );

/**@}*/

/* END OF HEADER FILE _HitSigDomain_ */
#endif
