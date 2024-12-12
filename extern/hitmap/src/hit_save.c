/**
 * Hit deprecated functions.
 * @file hit_save.c
 * @deprecated This file contains deprecated functions.
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

#include <hit_save.h>
#include <hit_com.h>
#include <hit_topology.h>

#ifdef notcompilethisfile

/* Hit TOPOLOGY: COMMIT */
void hit_topCommitInternal(HitTopology *topo, MPI_Comm oldComm) {
	MPI_Comm *newCom = (MPI_Comm *)malloc( sizeof( MPI_Comm ) );
	
	int ok = MPI_Comm_split( oldComm, topo->active, topo->linearRank, newCom );
	hit_mpiTestError(ok,"Failed split communicator for topology commit");
	
	topo->pTopology.lowLevel = (void *)newCom;
}

/* Hit TOPOLOGY: FREE */
void hit_topFreeInternal(HitTopology *topo, MPI_Comm *oldComm) {
	int ok = MPI_Comm_free( (MPI_Comm *)topo->pTopology.lowLevel );
	hit_mpiTestError(ok,"Failed to free communicator for topology");
	
	topo->pTopology.lowLevel = (void *)MPI_COMM_NULL;
}



/* HitCom: CREATE STRUCT MPI TYPE */
void hit_comTypeStructInternal(HitType * newType, int count, ...){

	int * lengths;
	HitType  * types;

	// @arturo Ago 2015: New allocP interface
	/* 
	hit_malloc(lengths,sizeof(int) * (size_t) count);
	hit_malloc(types,sizeof(HitType) * (size_t) count);
	*/
	hit_malloc(lengths, int, count);
	hit_malloc(types, HitType, count);
	
	
	va_list ap;
	va_start(ap, count);

	int i;
	for(i=0;i<count;i++){

		lengths[i] = va_arg(ap,int);
		types[i]   = va_arg(ap,HitType);
	}
    va_end(ap);
		
	
	HitAint * displacements;
	// @arturo Ago 2015: New allocP interface
	// hit_malloc(displacements,sizeof(HitAint) * (size_t) count);
	hit_malloc(displacements, HitAint, count);
	
	displacements[0] = (HitAint) 0;
	
	for(i=1; i<count; i++){
		int size;
		MPI_Type_size(types[i-1],&size);
		displacements[i] = displacements[i-1] + (HitAint) lengths[i-1] * size ;
	} 
	
	MPI_Type_create_struct(count, lengths, displacements, types, newType);
	MPI_Type_commit(newType);
	
	hit_free(displacements);
	hit_free(lengths);
	hit_free(types);

}

#endif
