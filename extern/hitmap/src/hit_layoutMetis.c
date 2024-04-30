/**
 * Metis layout plug-in. This layout uses the Metis partition library to
 * map the elements of a sparse shape domain into the processors of a 
 * virtual topology.
 * 
 * @file hit_layoutMetis.c
 * @version 1.0
 * @author Javier Fresno Bausela
 * @date Jun 2011
 *
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

#ifndef INCLUDE_METIS__H
	#define INCLUDE_METIS__H
	#include "metis.h"
#endif

#include <stdio.h>
#include <math.h>

#include <hit_topology.h>
#include <hit_layout.h>
#include <hit_layoutP.h>
#include <hit_funcop.h>
#include <hit_allocP.h>
#include <hit_com.h>

#include <hit_cshape.h>
#include <hit_bshape.h>


#ifndef INCLUDE_METIS__H
	#define INCLUDE_METIS__H
	#include "metis.h"
#endif



/**
 * Performs a Broadcast communication to share a sparse shape.
 */
void hit_sparseShapeBcastInternal(HitShape * shape, HitTopology topo){

	int ok;
	int sizes[2];
	// @arturo Mar 2013
	//MPI_Comm comm = *((MPI_Comm *)topo.pTopology.lowLevel);
	MPI_Comm comm = topo.pTopology->comm;
	
	if(topo.pTopology->selfRank == 0){

		int n = hit_cShapeNvertices(*shape);
		int m2 = hit_cShapeNedges(*shape);

		sizes[0] = n;
		sizes[1] = m2;

		ok = MPI_Bcast(sizes, 2, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		
		ok = MPI_Bcast(&(hit_cShapeNameList(*shape,0).flagNames),1,MPI_INT,0,comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		ok = MPI_Bcast(hit_cShapeXadj(*shape), n+1, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		ok = MPI_Bcast(hit_cShapeAdjncy(*shape), m2, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		ok = MPI_Bcast(hit_cShapeNameList(*shape,0).names, n, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");

	} else {

		ok = MPI_Bcast(sizes, 2, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		int  n = sizes[0];
		int m2 = sizes[1];

		HitShape newShp = hit_csrShape(n,m2);

		ok = MPI_Bcast(&(hit_cShapeNameList(newShp,0).flagNames),1,MPI_INT,0,comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		if(hit_cShapeNameList(newShp,0).flagNames == HIT_SHAPE_NAMES_ARRAY)
			hit_cShapeNameList(newShp,0).flagNames = HIT_SHAPE_NAMES_NOARRAY;
		
		ok = MPI_Bcast(hit_cShapeXadj(newShp), n+1, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		ok = MPI_Bcast(hit_cShapeAdjncy(newShp), m2, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");
		ok = MPI_Bcast(hit_cShapeNameList(newShp,0).names, n, MPI_INT, 0, comm);
		hit_mpiTestError(ok,"Error in sparseShapeBcast");

		hit_cShapeNameList(newShp,0).nNames = n;
		hit_cShapeNameList(newShp,1) = hit_cShapeNameList(newShp,0);

		*shape = newShp;
	}

}

//#define DEBUG_LAY_METIS

#ifdef DEBUG_LAY_METIS
#define debug(...) { if( hit_Rank == 0 ) { printf(__VA_ARGS__); fflush(stdout); }}
#define debugall(...) { printf(__VA_ARGS__);}
#endif



// @javfres 2015-10-05 I have been refactoring this function dropping out part
// of the group support so it could work when there are more processes than
// nodes. Now some processes will be inactive.
HitLayout hit_layout_plug_layMetis(int freeTopo, HitTopology topo, HitShape * shapeP){

	int i;

	// 0. Broadcast the sparse shape
	hit_sparseShapeBcastInternal(shapeP,topo);

	// Shape pointer to struct
	HitShape shape = *shapeP;
	
	// 1. Obtain the number of vertices and processes
	int numVertices = hit_cShapeNvertices(shape);
	int numProcessors = hit_topCard(topo);

	// 2. Create the layout
	HitLayout lay = HIT_LAYOUT_NULL;
	hit_layout_list_initGroups(&lay,hit_cShapeNvertices(shape));
	lay.topo = topo;
	lay.origShape = shape;
	int numParts = hit_min(numVertices,numProcessors);
	lay.numActives[0] = numParts;
	lay.type = HIT_LAYOUT_METIS;
	hit_layShape(lay) = HIT_CSR_SHAPE_NULL;

	// Metis doesn't work if there is more processes than nodes,
	// we must split the communicator and set non active processros.
	if(numProcessors <= numVertices){
		lay.active = 1;
		lay.pTopology[0] = hit_ptopDup(topo.pTopology);
	} else {
		int linearRank = hit_topRankInternal(topo,hit_topRanks(topo));
	
		if(linearRank < numVertices){
			lay.active = 1;
		} else {
			lay.active = 0;
		}
		
		// Split the communicator
		lay.pTopology[0] = hit_ptopSplit(topo.pTopology, lay.active);
	}

	// Exit if the layout is not active
	if(lay.active == 0) {
		if( freeTopo ) hit_topFree( topo );
		return lay;
	}

	// Create the groups
	for(i=0;i<numParts;i++){
		// @javier 2015-10-05 Only one process per group so the rest are disabled
		// hit_layout_list_addGroup(&lay,-1,numProcessors / numParts + ((numProcessors % numParts > i) ? 1 : 0 ));
		hit_layout_list_addGroup(&lay,-1,1);

	}

	// @arturo 2015/01/22
	//int group = hit_lay_procGroup(lay,topo.linearRank);
	int group = hit_lay_procGroup(lay, hit_topSelfRankInternal( topo ) );
	lay.group = group;

	if( freeTopo ) hit_topFree( topo );

	// 3. Call the partition function
	int wgtflag = 0;
	int numflag = 0;
	int options[5] = {0,0,0,0,0};
	int edgecut;

	// We can use the assignedGroups generated in hit_layout_list_initGroups as the part array.
	int * part = lay.info.layoutList.assignedGroups;

	if(numProcessors > 1){

		// Call METIS function.
		METIS_PartGraphKway (&hit_cShapeNvertices(shape), hit_cShapeXadj(shape),
			hit_cShapeAdjncy(shape), NULL, NULL,
			&wgtflag, &numflag, &numParts, options, &edgecut, part);

	} else {

		bzero(part, (size_t) numVertices * sizeof(int));
	}


#ifdef DEBUG_LAY_METIS
	{int i; sleep(2*hit_Rank);
	debug("part: ");
	for(i=0;i<hit_cShapeNvertices(shape);i++){
		debug("%d",part[i]);
	}
	debug("\n");}
#endif

	// 4.a Get the number of vertices (n) from the partition.
	int n = 0;

	for(i=0;i<hit_cShapeNvertices(shape);i++){
		if(part[i] == group){
			n++;
		}
	}
	
	// 4.b Get the number of edges (m).
	int vertex;
	int m=0;

	for(vertex=0; vertex<hit_cShapeNvertices(shape); vertex++){

		// Get first and last links.
		int first = hit_cShapeXadj(shape)[vertex];
		int last = hit_cShapeXadj(shape)[vertex+1];
		int link;

		for(link=first; link<last; link++){

			//Get the neighbor.
			int neighbor = hit_cShapeAdjncy(shape)[link];
			if((part[vertex] == group && part[neighbor] == group) ){
					m++;
			}
		}
	}


#ifdef DEBUG_LAY_METIS
	{int i; 
	debugall("p(%d): ",group);
	debugall(" n=%d, m=%d \n",n,m);}
#endif

	
	// 5. Create the local shape.
	HitShape lshape = hit_csrShape(n,m);

	// 5.1 Calculate the global-local name translation.
	int current = 0;
	for(i=0; i<hit_cShapeNvertices(shape); i++){
		if( part[i] == group ){
			hit_cShapeNameList(lshape,0).names[current++] = hit_cShapeNameList(shape,0).names[i];
		}
	}

	// Create the inverse translation list to speedup the algorithm.
	hit_cShapeCreateInvNames(&lshape);

#ifdef DEBUG_LAY_METIS
	{int i; 
	debugall("globalName(%d): ",group);
	for(i=0;i<hit_cShapeNvertices(lshape);i++){
		debugall("%d",hit_cShapeNameList(lshape,0).names[i]);
	}
	debugall("\n");}
#endif

	// 5.2 Init xadj and adjncy
	hit_cShapeXadj(lshape)[0] = 0;
	int lvertex;

	
	// Create the local graph.
	for(lvertex=0; lvertex<hit_cShapeNvertices(lshape); lvertex++){

		hit_cShapeXadj(lshape)[lvertex+1] = hit_cShapeXadj(lshape)[lvertex];
		int gvertex = hit_cShapeNameList(lshape,0).names[lvertex];
		
		// Locate the vertex in the global shape.
		int g;
		for(g=0;g<hit_cShapeNvertices(shape); g++){
			if( gvertex == hit_cShapeNameList(shape,0).names[g]  ){
				vertex = g;
				break;
			}
		}

		// Get first and last links.
		int first = hit_cShapeXadj(shape)[vertex];
		int last = hit_cShapeXadj(shape)[vertex+1];
		int link;

		for(link=first; link<last; link++){

			//Get the neighbor.
			int neighbor = hit_cShapeAdjncy(shape)[link];
			int gneighbor = hit_cShapeNameList(shape,0).names[neighbor];
			int lneighbor = hit_cShapeVertexToLocal(lshape,gneighbor);
			
			if(lneighbor != -1){

				hit_cShapeAdjncy(lshape)[hit_cShapeXadj(lshape)[lvertex+1]] = lneighbor;
				hit_cShapeXadj(lshape)[lvertex+1] ++;
			}
		}
	}

#ifdef DEBUG_LAY_METIS
	{int i; 
	debugall("xadj(%d): ",group);
	for(i=0;i<hit_cShapeNvertices(lshape)+1;i++){
		debugall("%d",hit_cShapeXadj(lshape)[i]);
	}
	debugall("\n");}


	{int i;
	debugall("adjncy(%d): ",group);
	for(i=0;i<m;i++){
		debugall("%d",hit_cShapeAdjncy(lshape)[i]);
	}
	debugall("\n");}
#endif

	// The namelist is the same for both dimensions
	hit_cShapeNameList(lshape,1) = hit_cShapeNameList(lshape,0);

	// 7. Fill the layout structure.
	hit_layShape(lay) = lshape;

	// 8. Create the assignedGroups array
	lay.info.layoutList.numElementsTotal = hit_cShapeNvertices(shape);

	// 10. Return the layout
	return lay;
}







