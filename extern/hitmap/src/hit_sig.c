/**
* Definition of the HitSig types for domain signatures.
* Constructors and functions to manipulate them.
*
* @file hit_sig.c
* @version 1.1
* @author Arturo Gonzalez-Escribano
* @author Javier Fresno Bausela
* @author Carlos de Blas Carton
* @date Sep 2011
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
#include <hit_sig.h>

/* Hit SIGNATURE NULL CONSTANT */
HitSig	HIT_SIG_NULL	= HIT_SIG_NULL_STATIC;
HitSig	HIT_SIG_WHOLE	= HIT_SIG_WHOLE_STATIC;



/* Hitmap INTERNAL HELPER: 
 * 			COMPUTE THE G.C.D. OF TWO INTEGERS WITH THE EXTENDED EUCLIDEAN METHOD
 * 			RETURNS THE GCD AND THE TWO REMAINDERS
 */
void hit_utilExtendedEuclideanGCD( int a, int b, int *gcd, int *pLastx, int *pLasty ) {
	int x=0, lastx=1;
	int y=1, lasty=0;

	while ( b != 0 ) {
		int quotient = a / b;

		int temp=b;
		b = a % b;
		a = temp;

		temp = x;
		x = lastx-quotient*x;
		lastx = temp;

		temp = y;
		y = lasty-quotient*y;
		lasty = temp;
	}

	*gcd = a;
	*pLastx = lastx;
	*pLasty = lasty;
}


/**
 * Computes intersection of two signatures
 */
HitSig hit_sigIntersect(HitSig s1, HitSig s2) {
	HitSig res = HIT_SIG_NULL;

	/* 0. REJECT TRIVIAL NON-OVERLAPPING RANGES */
	if ( s1.end < s2.begin || s2.end < s1.begin ) return HIT_SIG_NULL;

	/* 1. SIMPLER PROCEDURE FOR SIGNATURES WITH STRIDE 1 */
	if ( s1.stride == 1 && s2.stride == 1 ) {
		/* 1.1. GET MAXIMUM OF BEGINS AND MINIMUM OF ENDS */
		res.begin = (s1.begin > s2.begin) ? s1.begin : s2.begin;
		res.end = (s1.end < s2.end) ? s1.end : s2.end;
		res.stride = 1;
	}
	/* 2. PROCEDURE FOR ONLY ONE SIGNATURE WITH STRIDE 1 */
	else if ( s1.stride == 1 || s2.stride == 1) {
		/* 2.1. IDENTIFY THE STRIDED SIGNATURE AND THE NO-STRIDED SIGNATURE */
		HitSig stridedSig, noStridedSig;
		if ( s1.stride != 1 ) { stridedSig = s1; noStridedSig = s2; }
		else { stridedSig = s2; noStridedSig = s1; }

		/* 2.2. BEGIN */
		if ( noStridedSig.begin <= stridedSig.begin ) res.begin = stridedSig.begin;
		else {
			int displacement = (noStridedSig.begin - stridedSig.begin) % stridedSig.stride;
			if ( displacement == 0 ) res.begin = noStridedSig.begin;
			else res.begin = noStridedSig.begin - displacement + stridedSig.stride;
		}

		/* 2.3. END */
		if ( noStridedSig.end >= stridedSig.end ) res.end = stridedSig.end;
		else res.end = noStridedSig.end;

		/* 2.4. CHECK NO INTERSECTION CONDITION */
		if ( res.begin > res.end ) return HIT_SIG_NULL;

		/* 2.4. NORMALIZE END AND COPY STRIDE */
		res.end = res.end - ( res.end - res.begin ) % stridedSig.stride;
		res.stride = stridedSig.stride;
	}
	/* 3. GENERAL PROCEDURE FOR STRIDED SIGNATURES */
	else {
		/* 3.2. SORT SIGNATURES BY STRIDE */
		HitSig low, high;
		if ( s1.stride <= s2.stride ) { low = s1; high = s2; }
		else { low = s2; high = s1; }

		/* 3.3. DISPLACE SIGNATURES: HIGH STRIDED SIGNATURE WILL BEGIN AT 0 */
		int displacement = high.begin;
		high.begin = 0;
		high.end = high.end - displacement;
		low.begin = low.begin - displacement;
		low.end = low.end - displacement;

		int lowerEnd = ( low.end < high.end ) ? low.end : high.end;
		int higherBegin = ( low.begin > high.begin ) ? low.begin : high.begin;

		/* 3.4. OBTAIN THE SMALLER POSITIVE POINT OF THE LOW STRIDED SIGNATURE: norm */
		int norm = low.begin % low.stride;
		if ( norm < 0 ) norm = norm + low.stride;

		/* 3.5. COMPUTE G.C.D. AND L.C.M. OF STRIDES */
		int gcd, lastx, lasty;
		hit_utilExtendedEuclideanGCD( high.stride, low.stride, &gcd, &lastx, &lasty );
		int lcm = high.stride / gcd * low.stride;

		/* 3.6. PARTICULAR CASE: norm == 0, TRIVIAL COINCIDENCE */
		if ( norm == 0 ) {
			/* 3.6.1. GET NEW BEGIN */
			if ( low.begin <= 0 ) res.begin = 0;
			else if ( low.begin % lcm == 0 ) res.begin = low.begin;
			else res.begin = low.begin + lcm - low.begin % lcm;
		}
		/* 3.7. GENERAL CASE: norm != 0 */
		else { 
			/* 3.7.1. CHECK BASIC INTERSECTION CONDITION */
			if ( norm % gcd != 0 ) return HIT_SIG_NULL;

			/* 3.7.2. OBTAIN FIRST COINCIDENCE */
			int multLowStride;
			int jumps = norm / gcd;
			int cycle = high.stride / gcd;

			if ( lasty < 0 ) multLowStride = jumps * -lasty % cycle;
			else multLowStride = cycle - ( jumps * lasty % cycle );

			int coincidence = norm + multLowStride * low.stride;

			/* 3.7.3. COMPUTE RESULT BEGIN */
			if ( coincidence >= higherBegin ) res.begin = coincidence;
			else {
				int adjust = ( higherBegin - coincidence ) % lcm;
				if ( adjust == 0 ) res.begin = higherBegin;
				else res.begin = higherBegin + lcm - adjust;
			}
		}

		/* 3.8. CHECK LAST INTERSECTION CONDITION */
		if ( res.begin > lowerEnd ) return HIT_SIG_NULL;

		/* 3.9. FILL UP RESULT WITH STRIDE AND NORMALIZED END */
		res.stride = lcm;
		res.end = lowerEnd - ( lowerEnd - res.begin ) % lcm;

		/* 3.10. RESTORE DISPLACEMENT */
		res.begin += displacement;
		res.end += displacement;
	}

	/* 4. RETURN INTERSECTION */
	return res;
}


