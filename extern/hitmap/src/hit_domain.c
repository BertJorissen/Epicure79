/**
 * Functionalities to manipulate Domains of multiple Signature-based shapes.
 *
 * Manipulation of HitSigDomain: Non-overlapping collections of multidimensional 
 * shapes for unions of dense domains of indexes.
 *
 * @file hit_domain.c
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

#include<hit_domain.h>



/*
 * Add a shape at the end of the domain list, realloc the list
 */
void hit_domainAddSigShape( HitSigDomain *dom, HitSigShape sh ) {
	dom->num++;
	dom->list = (HitSigShape *)realloc( dom->list, (size_t)dom->num * sizeof(HitSigShape) );
	if ( dom->list == NULL ) {
		fprintf(stderr, "HitSigDomain: Internal error re-allocating memory to add shape\n");
		exit( EXIT_FAILURE );
	}
	dom->list[ dom->num-1 ] = sh;
}

/*
 * Add many shapes at the end of the domain list, realloc the list
 */
void hit_domainAddManySigShapes( HitSigDomain *dom, HitSigDomain shps ) {
	if ( shps.num < 1 ) return;
	int oldNum = dom->num;
	dom->num += shps.num;
	dom->list = (HitSigShape *)realloc( dom->list, (size_t)dom->num * sizeof(HitSigShape) );
	if ( dom->list == NULL ) {
		fprintf(stderr, "HitSigDomain: Internal error re-allocating memory to add many shapes\n");
		exit( EXIT_FAILURE );
	}
	int ind;
	for ( ind=0; ind<shps.num; ind++ ) 
		dom->list[ ind+oldNum ] = shps.list[ ind ];
}


/*
 * Remove a shape identified by its position in th list
 */
void hit_domainRemoveShape( HitSigDomain *dom, int num ) {
	int ind;
	for( ind=num; ind < dom->num-1; ind++ ) {
		dom->list[ind] = dom->list[ind+1];
	}
	dom->num--;
/* Skip reallocating until the next add operation
	dom->list = (HitSigShape *)realloc( dom->list, dom->num * sizeof(HitSigShape) );
	if ( dom->list == NULL ) {
		fprintf(stderr, "HitSigDomain: Internal error re-allocating memory to remove shape\n");
		exit( EXIT_FAILURE );
	}
*/
}

/*
 * Mark a shape as deleted
 */
void hit_domainNullifyShape( HitSigDomain *dom, int num ) {
	dom->list[num].numDims = -1;
}

/*
 * Clean deleted shapes
 */
void hit_domainRemoveNullShapes( HitSigDomain *dom ) {
	int adv_ind = 0;
	int loc_ind = 0;
	int origNum = dom->num;
	while ( adv_ind < origNum ) {
		if ( dom->list[adv_ind].numDims == -1 ) { 
			adv_ind++;
			dom->num--;
		}
		else {
			if ( adv_ind != loc_ind ) dom->list[loc_ind] = dom->list[adv_ind];
			adv_ind++;
			loc_ind++;
		}
	}
}

/* 
 * printDomain: For test and debug purposes
 */
void hit_domainPrint( HitSigDomain dom_old ) {
	int ind;
	for ( ind=0; ind<dom_old.num; ind++ ) {
		printf("\tShape %d:\t", ind );
		int dim;
		for ( dim=0; dim<dom_old.list[ind].numDims; dim++ ) {
			printf("[%d:%d]", 
				dom_old.list[ind].sig[dim].begin,
				dom_old.list[ind].sig[dim].end );
		}
		printf("\n");
	}
}


/*
 * Classes of unions during dimensions exploration
 */
typedef enum hit_sig_union_t { ret_none, ret_equal, ret_new, ret_old, ret_overlap, ret_touching } hit_sig_union_t;

/*
 * Check signatures to identify the union class for a given dimension
 */
hit_sig_union_t hit_sigUnionType( HitSig s_new, HitSig s_old ) {
#ifdef DEBUG
printf("DEBUG: new [%d:%d], old [%d:%d]\n", s_new.begin, s_new.end, s_old.begin, s_old.end );
#endif
	if ( s_new.begin == s_old.end+1 || s_new.end == s_old.begin-1 )
		return ret_touching;
	else if ( s_new.begin > s_old.end || s_new.end < s_old.begin )
		return ret_none;
	else if ( s_new.begin == s_old.begin && s_new.end == s_old.end )
		return ret_equal;
	else if( s_new.begin <= s_old.begin && s_new.end >= s_old.end )
		return ret_new;
	else if( s_old.begin <= s_new.begin && s_old.end >= s_new.end )
		return ret_old;
	else return ret_overlap;
}

/*
 * Explore dimensions to identify the union class
 */
hit_sig_union_t exploreDimension( int dim, HitSigShape in_new, HitSigShape in_old, HitSigShape *intersection, HitSigDomain *out_news, HitSigDomain *out_olds ) {
	HitSig s_new = in_new.sig[dim];
	HitSig s_old = in_old.sig[dim];
	HitSig s_intersect;

	/* 1. Check type of overlapping/union */
	hit_sig_union_t type = hit_sigUnionType( s_new, s_old );
#ifdef DEBUG
printf("DEBUG: Inner: %d\n", type );
#endif

	/* 2. No intersection, skip */
	if ( type == ret_none ) return ret_none;

#ifdef DEBUG
printf("DEBUG: s_new [%d,%d]\n", s_new.begin, s_new.end );
printf("DEBUG: s_old [%d,%d]\n", s_old.begin, s_old.end );
#endif

	/* 3. Intersect dimension */
	s_intersect.begin = ( s_new.begin < s_old.begin ) ? s_old.begin : s_new.begin;
	s_intersect.end = ( s_new.end < s_old.end ) ? s_new.end : s_old.end;
	s_intersect.stride = 1;
	intersection->sig[dim] = s_intersect;

#ifdef DEBUG
printf("DEBUG: intersection dim %d, [%d,%d]\n", dim, intersection->sig[dim].begin, intersection->sig[dim].end );
#endif

	/* 4. Last dimension: End recursion */
	if ( dim == in_new.numDims-1 ) {
		/* If fusion is needed, add a new fused shape to out_news */
		// Adding touching overlap
		// if ( type == ret_overlap ) {
		if ( type == ret_overlap || type == ret_touching ) {
			HitSigShape add = *intersection;
			add.sig[dim].begin = ( s_new.begin < s_old.begin ) ? s_new.begin : s_old.begin;
			add.sig[dim].end = ( s_new.end > s_old.end ) ? s_new.end : s_old.end;
			hit_domainAddSigShape( out_news, add );
			// Touching is transformed in overlap
			return ret_overlap;
		}
		return type;
	}

	/* 5. Other dimensions */
	/* 5.0. Recursion */
	hit_sig_union_t type_lower = exploreDimension( dim+1, in_new, in_old, intersection, out_news, out_olds );

	/* 5.1. If there is no intersection, nothing to do, return none */
	if ( type_lower == ret_none ) return ret_none;

	/* 5.2. Shape are equal on this dimension, noting to do, return the lower */
	if ( type == ret_equal ) return type_lower;

	/* 5.3. Lower types are all equal */
	if ( type_lower == ret_equal ) {
		/* If fusion is needed, add a new fused shape to out_news */
		// Adding touching overlap
		// if ( type == ret_overlap ) {
		if ( type == ret_overlap || type == ret_touching ) {
			HitSigShape add = *intersection;
			add.sig[dim].begin = ( s_new.begin < s_old.begin ) ? s_new.begin : s_old.begin;
			add.sig[dim].end = ( s_new.end > s_old.end ) ? s_new.end : s_old.end;
			hit_domainAddSigShape( out_news, add );
			// Touching is transformed in overlap
			return ret_overlap;
		}
		return type;
	}

	// Touching shapes is equivalent to none if the lower dimensions are not equals
	if ( type == ret_touching ) return ret_none;

	/* 5.4. Lower are non overlapped and they are the same as current, nothing to do */
	if ( type_lower == type && ( type == ret_new || type == ret_old ) ) return type_lower;

	/* 5.5. Lower is overlapped, cut both shapes in this dimension */
	if ( type_lower == ret_overlap ) {
#ifdef DEBUG
printf("DEBUG: Post-overlapped \n");
#endif
		int pre_dim;
		HitSigShape add;

		/* New: Restrict previous dimensions to the intersection */
		add = in_new;
		for ( pre_dim=0; pre_dim<dim && pre_dim < HIT_MAXDIMS; pre_dim++ ) {
			add.sig[pre_dim] = intersection->sig[pre_dim];
		}
		/* New: Substract the intersection at the current dimension */
		/* New: Add left part if exists */
		if ( intersection->sig[dim].begin > s_new.begin ) {
#ifdef DEBUG
printf("DEBUG: Add new left\n");
#endif
			add.sig[dim].begin = s_new.begin;
			add.sig[dim].end = intersection->sig[dim].begin - 1;
			hit_domainAddSigShape( out_news, add );
		}
#ifdef DEBUG
printf("DEBUG: Test new right: %d, %d\n", intersection->sig[dim].end, s_new.end );
#endif
		/* New: Add right part if exists */
		if ( intersection->sig[dim].end < s_new.end ) {
#ifdef DEBUG
printf("DEBUG: Add new right\n");
#endif
			add.sig[dim].begin = intersection->sig[dim].end + 1;
			add.sig[dim].end = s_new.end;
			hit_domainAddSigShape( out_news, add );
		}

		/* Old: Restrict previous dimensions to the intersection */
		add = in_old;
		for ( pre_dim=0; pre_dim<dim && pre_dim < HIT_MAXDIMS; pre_dim++ ) {
			add.sig[pre_dim] = intersection->sig[pre_dim];
		}
		/* Old: Substract the intersection at the current dimension */
		/* Old: Add left part if exists */
		if ( intersection->sig[dim].begin > s_old.begin ) {
			add.sig[dim].begin = s_old.begin;
			add.sig[dim].end = intersection->sig[dim].begin - 1;
			hit_domainAddSigShape( out_olds, add );
		}
		/* Old: Add right part if exists */
		if ( intersection->sig[dim].end < s_old.end ) {
			add.sig[dim].begin = intersection->sig[dim].end + 1;
			add.sig[dim].end = s_old.end;
			hit_domainAddSigShape( out_olds, add );
		}

		/* Return overlap */
		return type_lower;
	}

	/* 5.6. Lower is new/old, cut the opposite on this dimension (overlapped or not) */
	if ( type_lower == ret_new ) {
#ifdef DEBUG
printf("DEBUG: Post-new, cut old \n");
#endif
		int pre_dim;
		HitSigShape add;

		/* Old: Restrict previous dimensions to the intersection */
		add = in_old;
		for ( pre_dim=0; pre_dim<dim && pre_dim < HIT_MAXDIMS; pre_dim++ ) {
			add.sig[pre_dim] = intersection->sig[pre_dim];
		}
		/* Old: Substract the intersection at the current dimension */
		/* Old: Add left part if exists */
		if ( intersection->sig[dim].begin > s_old.begin ) {
			add.sig[dim].begin = s_old.begin;
			add.sig[dim].end = intersection->sig[dim].begin - 1;
			hit_domainAddSigShape( out_olds, add );
		}
		/* Old: Add right part if exists */
		if ( intersection->sig[dim].end < s_old.end ) {
			add.sig[dim].begin = intersection->sig[dim].end + 1;
			add.sig[dim].end = s_old.end;
			hit_domainAddSigShape( out_olds, add );
		}

		/* Return new */
		return type_lower;
	}
	else if ( type_lower == ret_old ) {
#ifdef DEBUG
printf("DEBUG: Post-old, cut new \n");
#endif
		int pre_dim;
		HitSigShape add;

		/* New: Restrict previous dimensions to the intersection */
		add = in_new;
		for ( pre_dim=0; pre_dim<dim && pre_dim < HIT_MAXDIMS; pre_dim++ ) {
			add.sig[pre_dim] = intersection->sig[pre_dim];
		}
		/* New: Substract the intersection at the current dimension */
		/* New: Add left part if exists */
		if ( intersection->sig[dim].begin > s_new.begin ) {
#ifdef DEBUG
printf("DEBUG: Add new left\n");
#endif
			add.sig[dim].begin = s_new.begin;
			add.sig[dim].end = intersection->sig[dim].begin - 1;
			hit_domainAddSigShape( out_news, add );
		}
#ifdef DEBUG
printf("DEBUG: Test new right: %d, %d\n", intersection->sig[dim].end, s_new.end );
#endif
		/* New: Add right part if exists */
		if ( intersection->sig[dim].end < s_new.end ) {
#ifdef DEBUG
printf("DEBUG: Add new right\n");
#endif
			add.sig[dim].begin = intersection->sig[dim].end + 1;
			add.sig[dim].end = s_new.end;
			hit_domainAddSigShape( out_news, add );
		}

		/* Return old */
		return type_lower;
	}

	/* Error: It arrives here only if not all cases are considered */
	fprintf(stderr, "HitSigDomain: Internal error, non possible state during dimension exploration\n");
	exit( EXIT_FAILURE );
}

/*
 * Union of a new shape with a given domain
 */
void hit_domainSigShapeUnion( HitSigDomain *old, HitSigShape shp_new ) {
	/* Start the new search with the new input */
	HitSigDomain alt_news = HIT_SIG_DOMAIN_NULL_STATIC;
	hit_domainAddSigShape( &alt_news, shp_new );

	/* Storage for new/old pieces */
	HitSigDomain tmp_news = HIT_SIG_DOMAIN_NULL_STATIC;
	HitSigDomain tmp_olds = HIT_SIG_DOMAIN_NULL_STATIC;

	/* Compare the new shape with each one of the old shapes */
	int currentOldNum = old->num;
	int ind;
	for( ind = 0; ind < currentOldNum; ind++ ) {
#ifdef DEBUG
{
printf("DEBUG: Nuevo old shape %d\n", ind );
	HitSigShape s_new = old->list[ind];
	printf("DEBUG: Old Shape:\t[%d:%d][%d:%d][%d:%d]\n",
		s_new.sig[0].begin,
		s_new.sig[0].end,
		s_new.sig[1].begin,
		s_new.sig[1].end,
		s_new.sig[2].begin,
		s_new.sig[2].end );

printf("DEBUG: AltNews:");
printDomain( alt_news );
}
#endif
		/* Start with next old in the cheking queue: It can be splitted in several pieces 
			when checking each piece of the new shape */
		HitSigDomain alt_olds = HIT_SIG_DOMAIN_NULL_STATIC;
		hit_domainAddSigShape( &alt_olds, old->list[ind] );
		hit_domainNullifyShape( old, ind );

		/* If a previous comparison has split the new shape, there are several ones to compare */
		int indNews;
		for ( indNews = 0; indNews<alt_news.num; indNews++ ) {
			/* Keep track of generated pieces of the new shape */
			HitSigDomain out_news = HIT_SIG_DOMAIN_NULL_STATIC;

			/* Next piece of the new shape to compare */
			HitSigShape test_new = alt_news.list[indNews];

			/* If a check with a previous new piece has split the old shape, there are several ones to compare */
			int indOlds;
			for ( indOlds = 0; indOlds<alt_olds.num; indOlds++ ) {
				/* Keep track of generated pieces of the old shape */
				HitSigDomain out_olds = HIT_SIG_DOMAIN_NULL_STATIC;

				/* Next piece of the old shape to compare */
				HitSigShape test_old = alt_olds.list[indOlds];

				/* Use a temporal variable to track the intersection */
				HitSigShape intersection = test_new;

				/* Recursive exploration of dimensions */
				hit_sig_union_t result = exploreDimension( 0, test_new, test_old, &intersection, &out_news, &out_olds );
#ifdef DEBUG
	printf("DEBUG: Action %d\n", result );
#endif

				/* Action Continue: No overlap */
				if ( result == ret_none ) {
#ifdef DEBUG
	printf("DEBUG: None %d\n", result );
#endif
					continue;
				}

				/* Action Skip new: Equal shapes */
				if ( result == ret_equal ) {
#ifdef DEBUG
	printf("DEBUG: Skip new %d\n", result );
#endif
					hit_domainNullifyShape( &alt_news, indNews );
					continue;
				}

				/* Action Delete Old/New when they are totally or partially inside the dominant */
				if ( result == ret_old ) {
#ifdef DEBUG
	printf("DEBUG: Delete new %d\n", result );
#endif
					hit_domainNullifyShape( &alt_news, indNews );
				}
				else if ( result == ret_new ) {
#ifdef DEBUG
	printf("DEBUG: Delete old %d\n", result );
#endif
					hit_domainNullifyShape( &alt_olds, indOlds );
				}
				else { /* Overlap */
#ifdef DEBUG
	printf("DEBUG: Delete both %d\n", result );
#endif
					hit_domainNullifyShape( &alt_news, indNews );
					hit_domainNullifyShape( &alt_olds, indOlds );
				}

				/* Change new and/or old: Change deleted shapes by fused or cut pieces */
				/* Actions: Change old by its surviving pieces */
				if ( out_olds.num > 0 ) {
#ifdef DEBUG
	printf("DEBUG: Old pieces %d\n", out_olds.num );
#endif
					hit_domainAddManySigShapes( &tmp_olds, out_olds );
				}
				hit_domainClear( out_olds );

				/* Actions: Change new by its surviving pieces */
				if ( out_news.num > 0 ) {
#ifdef DEBUG
	printf("DEBUG: New pieces %d\n", out_news.num );
#endif
					hit_domainAddManySigShapes( &tmp_news, out_news );
				}
				hit_domainClear( out_news );
			}
			hit_domainRemoveNullShapes( &alt_olds );
			hit_domainAddManySigShapes( &alt_olds, tmp_olds );
			hit_domainClear( tmp_olds );
		}
		hit_domainRemoveNullShapes( &alt_news );
		hit_domainAddManySigShapes( &alt_news, tmp_news );
		hit_domainClear( tmp_news );

		/* Add the old pieces to the original domain */
		hit_domainAddManySigShapes( old, alt_olds );
		hit_domainClear( alt_olds );
	}

	/* Add the new pieces to the original domain */
	hit_domainAddManySigShapes( old, alt_news );
	hit_domainClear( alt_news );

	/* Clean original old null shapes */
	hit_domainRemoveNullShapes( old );

	/* Sort the resulting shapes: Direct insertion in an ancillary list */
	HitSigDomain sorted = HIT_SIG_DOMAIN_NULL_STATIC;
	for ( ind=0; ind<old->num; ind++ ) {
		/* Locate the next shape to insert */
		HitSigShape insert = HIT_SIG_SHAPE_INTERNAL_NULL_STATIC;
		int indSearch, indLocated = -1;
		for ( indSearch=0; indSearch<old->num; indSearch++ ) {
			if ( old->list[indSearch].numDims == -1 ) continue;
			if ( insert.numDims == -1 ) {
#ifdef DEBUG
//printf("DEBUG: Sorting: First candidate %d\n", indSearch );
#endif
				insert = old->list[indSearch];
				indLocated = indSearch;
				continue;
			}

			int dim;
			for ( dim=0; dim<old->list[indSearch].numDims; dim++ ) {
				if ( insert.sig[dim].begin < old->list[indSearch].sig[dim].begin ) break;
				if ( insert.sig[dim].begin > old->list[indSearch].sig[dim].begin ) {
#ifdef DEBUG
//printf("DEBUG: Sorting: Lower candidate %d\n", indSearch );
#endif
					insert = old->list[indSearch];
					indLocated = indSearch;
					break;
				}
				if ( insert.sig[dim].end < old->list[indSearch].sig[dim].end ) break;
				if ( insert.sig[dim].end > old->list[indSearch].sig[dim].end ) {
#ifdef DEBUG
//printf("DEBUG: Sorting: Lower candidate %d\n", indSearch );
#endif
					insert = old->list[indSearch];
					indLocated = indSearch;
					break;
				}
			}
		}
		if ( indLocated == -1 ) {
			fprintf(stderr, "HitSigDomain: Internal error, no insert candidate when sorting the resulting domain\n");
			exit( EXIT_FAILURE );
		}
		hit_domainNullifyShape( old, indLocated );
		hit_domainAddSigShape( &sorted, insert );
	}
	/* Move sorted list to old list */
	hit_domainClear( *old );
	*old = sorted;
}

/*
 * Intersection of two domains
 */
HitSigDomain hit_domainSigIntersection( HitSigDomain *dom1, HitSigDomain *dom2) {
	HitSigDomain res = HIT_SIG_DOMAIN_NULL_STATIC;

	/* For all pairs of shapes */
	int ind1, ind2;
	for( ind1=0; ind1<dom1->num; ind1++ )
		for( ind2=0; ind2<dom2->num; ind2++ ) {
			/* Get shapes to intersect */
			HitSigShape shp1 = dom1->list[ind1];
			HitSigShape shp2 = dom2->list[ind2];
			
			/* Build intersection */
			HitSigShape shpRes = HIT_SIG_SHAPE_INTERNAL_NULL_STATIC;
			shpRes.numDims = shp1.numDims;

			int dim;
			for( dim=0; dim<shp1.numDims; dim++ ) {
				HitSig s1 = shp1.sig[dim];
				HitSig s2 = shp2.sig[dim];

				/* Check if they do not overlap to stop */
				if ( s1.begin > s2.end || s1.end < s2.begin ) {
					shpRes.numDims = -1;
					break;
				}

				/* Intersect dimension */
				shpRes.sig[dim].begin = ( s1.begin < s2.begin ) ? s2.begin : s1.begin;
				shpRes.sig[dim].end = ( s1.end < s2.end ) ? s1.end : s2.end;
				shpRes.sig[dim].stride = 1;
			}

			/* Check if there is an intersection: Add it to the result domain */
			if ( shpRes.numDims != -1 ) 
				hit_domainAddSigShape( &res, shpRes );
		}

	/* Return resulting domain */
	return res;
}

