/**
 * Hierarchical tiling of multi-dimensional dense array.
 * Definitions and functions to manipulate HitTile types. 
 * This type defines a handler for dense arrays and sparse structures
 * with meta-information to create and manipulate hierarchical tiles. 
 *
 * @file hit_dump.c
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @date Sep 2012
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
#include <hit_dump.h>

//#include <hit_tile.h>
//#include <hit_cshape.h>
//#include <hit_bshape.h>


/* DEBUG: DUMP VARIABLE STRUCTURE INFORMATION */
void hit_dumpTileInternal(const void *var, const char* name, FILE *file) {
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
}

