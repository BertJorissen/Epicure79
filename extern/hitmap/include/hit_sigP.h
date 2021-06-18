/** @cond INTERNAL */
/**
* Signatures for dense or strided domains.
*
* Private and internal definitions.
*
* @file hit_sigP.h
* @ingroup Tiling
* @version 1.0
* @author Arturo Gonzalez-Escribano
* @author Javier Fresno Bausela
* @author Carlos de Blas Carton
* @date Jun 2010
*/
/** @endcond */

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

#ifndef _HitSigP_
#define _HitSigP_

/** @cond INTERNAL */
#define HIT_SIG_NUM_FIELDS	3

/* 2.1. Hit SIGNATURE NULL CONSTANT */
/**
 * Static Null value for domain signatures.
 */
#define	HIT_SIG_NULL_STATIC	{ -1, -2, 1 }

/* 2.2. Hit SIGNATURE WHOLE CONSTANT */
/**
 * Static Whole value for domain signatures.
 */
#define	HIT_SIG_WHOLE_STATIC	{ -2, -3, 1 }


/* 4. Hit SIGNATURE BLENDING */
/**
 * Selection of a subdomain.
 *
 * It computes the result of applying the second signature as a selection of the indexes
 * of the first signature in tile coordinates.
 * The result is equivalent to transform the first signature to tile coordinates. Subselect
 * the indexes indicated by the second signature. And transform back the selected indexes
 * to the array coordinates of the first signature.
 *
 * This functon is internally used in the tile selection functions. See hit_tileSelect().
 *
 * @author Arturo Gonzalez-Escribano
 *
 * @param[in] s1 Reference domain signature.
 * @param[in] s2 Subselection signature.
 * @return The signature of the subselection in array coordinates.
 */
static inline HitSig hit_sigBlend(HitSig s1, HitSig s2) {
	HitSig newSig;
	newSig.begin = s1.begin + s2.begin * s1.stride;
	newSig.end = s1.begin + s2.end * s1.stride;
	newSig.stride = s1.stride * s2.stride;
	return newSig;
}

/** @endcond */

/** @cond OLD */
/**
 * To expand the tile cardinalities cardinalities through two different signatures
 * @author Yuri
 */
static inline HitSig hit_sigExpand(HitSig s1, HitSig s2) {
	HitSig newSig;
	newSig.begin = s1.begin + s2.begin;
	newSig.end = s1.end + s2.end;
	newSig.stride = s1.stride * s2.stride;
	return newSig;
}
/** @endcond */


/* END OF HEADER FILE _HitSigP_ */
#endif
