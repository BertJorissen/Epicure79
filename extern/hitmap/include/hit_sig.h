/**
* Signatures for dense or strided domains.
*
* These functionalities allows to manage Signatures that define a 1-dimensional
* index space which is dense, or has strided indexes with regular intervals.
*
* @file hit_sig.h
* @ingroup Tiling
* @version 1.0
* @author Arturo Gonzalez-Escribano
* @author Javier Fresno Bausela
* @author Carlos de Blas Carton
* @date Jun 2010
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

#ifndef _HitSig_
#define _HitSig_

/* Hit SIGNATURES */
/**
 * @struct HitSig
 *
 * ADT for signatures of an index domain dimension.
 *
 * A signature defines a subset of Z.
 * A 1-dimensional domain signature is a triplet of three integer values [begin:end:stride]. 
 * They identify the first and last valid indexes in that domain (begin, end), 
 * and also a stride value, that indicates indexes at regular intervals, starting at the
 * begin index.
 *
 * The cardinality of a signature is the number of valid indexes in that domain.
 * An end value lesser than begin indicates and empty domain.
 *
 * Hitmap supports two systems of coordinates for domains. 
 * \arg \e Tile coordinates, that represents the indexes of a tile domain numbered starting 
 * at 0 and counted ignoring strides.
 * \arg \e Array or \e Ancestor coordinates, that represents the indexes 
 * of a hierachical subdomain, in the same tile coordinates of the ancestor that originally
 * defined the domain.
 *
 * Thus, in a new declared domain both coordinates systems have the same indexes. But in a 
 * subdomain declared as a selection of another domain, the tile coordinates are a renumbering
 * the original indexes starting at zero and ignoring strides.
 *
 * Member functions are documented in \ref hit_sig.h.
 *
 */
typedef struct {
	int begin;	/**< The begin index of the dimension */
	int end;	/**< The end index of the dimension */
	int stride; /**< The stride for regular sparse domains */
} HitSig;


#include "hit_sigP.h"


/* 2.1. Hit SIGNATURE NULL CONSTANT */
/**
 * Null value for domain signatures.
 *
 * The cardinality of this domain value is 0 elements.
 */
extern HitSig	HIT_SIG_NULL;

/* 2.2. Hit SIGNATURE WHOLE CONSTANT */
/**
 * Special value: Whole domain. 
 *
 * Special value to indicate in tile selections
 * that the whole collection of indexes of
 * another signature should be selected.
 * The cardinality of this domain value is 0 elements.
 *
 * \sa  hit_tileSelect()
 */
extern HitSig	HIT_SIG_WHOLE;


/* 3. Hit SIGNATURE GENERATING FUNCTION */
/**
 * Constructor (1) of domain signature values.
 *
 * @param[in] begin First valid index in the domain
 * @param[in] end Last valid index in the domain
 * @param[in] stride Stride for the selected valid indexes in the domain, starting at begin.
 * @return A new signature variable.
 */
static inline HitSig hit_sig(int begin, int end, int stride) {
	HitSig a = { begin, end, stride };
	return a;
}

/**
 * Constructor (2) of domain signature values.
 *
 * It is a shortcut to create signatures with the standard index domain
 * of a C-language array. Starting at 0, and with no stride.
 *
 * @param[in] numElems Number of elements in the domain
 * @return A new signature value with the range [0,numElems-1:1].
 */
static inline HitSig hit_sigStd(int numElems) {
	HitSig a = { 0, numElems-1, 1 };
	return a;
}

/**
 * Constructor (3) of domain signature values.
 *
 * This constructor builds a signature containing one and only one index.
 *
 * @param[in] ind An integer index.
 * @return A new signature value with a single index in its range [index:index:1].
 */
static inline HitSig hit_sigIndex(int ind) {
	HitSig a = { ind, ind, 1 };
	return a;
}


/* 5. Hit SIGNATURE CARDINALITY */
/**
 * Compute the cardinality of a domain signature.
 *
 * @hideinitializer
 *
 * @param[in] sig \e HitSig Domain signature.
 * @retval	int  Count of valid indexes in the domain.
 */
#define	hit_sigCard(sig)	(((sig).end-(sig).begin)/(sig).stride+1)

/* 8. Hit SIGNATURE: COMPARISON OPERATOR */
/**
 * Compare two signatures.
 *
 * @hideinitializer
 *
 * @param[in] s1 \e HitSig Domain signature.
 * @param[in] s2 \e HitSig Domain signature.
 * @retval	int Logical value. True if the two signatures are equal.
 */
#define	hit_sigCmp(s1,s2)	((s1).begin == (s2).begin && (s1).end == (s2).end && (s1).stride == (s2).stride)

/* 6. Hit CONDITION: NUMBER IN THE DOMAIN OF A SIGNATURE */
/**
 * Determine if an index is a valid index in a signature domain.
 *
 * @hideinitializer
 *
 * @param[in] sig \e HitSig Domain signature.
 * @param[in] ind \e int Index value.
 * @retval	int Logical value. True if ind is a valid index in the domain.
 */
#define hit_sigIn(sig,ind)	((ind)>=(sig).begin && (ind)<=(sig).end && (((ind)-(sig).begin)%(sig).stride == 0))

/* 7. Hit APPLY A ONE-ELEMENT SELECTION THROUGH A SIGNATURE */
/**
 * Translate an index in tile coordinate system to array coordinates.
 *
 * @hideinitializer
 *
 * @param[in] sig \e HitSig Domain signature.
 * @param[in] ind \e int Index value.
 * @retval	int Index value in array coordinate system.
 */
#define	hit_sigTileToArray(sig,ind)	((ind)*(sig).stride+(sig).begin)

/**
 * Translate an index in array coordinate system to tile coordinates.
 *
 * @hideinitializer
 *
 * @param[in] sig \e HitSig Domain signature.
 * @param[in] ind \e int Index value.
 * @retval	int Index value in tile coordinate system.
 */
#define	hit_sigArrayToTile(sig,ind)	(   (int)(((ind)-(sig).begin)/(sig).stride)   ) 


/* 9. Hit SIGNATURE: INTERSECTION OPERATOR */
/**
 * Intersect two signatures.
 *
 * Returns a new signature with the indexes that both domains have in common.
 *
 * @author Arturo Gonzalez-Escribano
 * @date Oct 2011
 *
 * @param[in] s1 Domain signature.
 * @param[in] s2 Domain signature.
 * @return The signature representing the intersection. HIT_SIG_NULL if the intersection is empty.
 */
HitSig hit_sigIntersect( HitSig s1, HitSig s2 );

/* END OF HEADER FILE _HitSig_ */
#endif
