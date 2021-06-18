/*
 * This file is intended as a demonstation of weighted layouts and layout groups.
 * @author Eduardo Rodriguez Gutiez
 * @version 0.01, 04/23/2016
 */

#include <stdio.h>
#include "hitmap.h"

hit_tileNewType(int);


void printHitTile(HitLayout globalLayout, HitTile_int aTile)
{
	int i;

	if (hit_layImActive(globalLayout))
		{	
		printf("--------- Data for process %i ---------\n", hit_Rank);
		printf("[%i:%i:%i (%i)]", hit_tileDimBegin(aTile, 0), hit_tileDimEnd(aTile, 0), 
					  hit_tileDimStride(aTile, 0), hit_tileDimCard(aTile, 0));
		for(i = 0; i < hit_tileDimCard(aTile, 0); i++)
			printf("%i ", hit_tileElemAt(aTile, 1, i));
		printf("\n");
		fflush(stdout);
                hit_comBarrier(globalLayout);
		}
}

char hit_layCmp(HitLayout lay1, HitLayout lay2){

	return (lay1.type == lay2.type) && (lay1.active == lay2.active) && (lay1.group == lay2.group) && 
		(lay1.leader == lay2.leader) && (hit_ranksCmp(lay1.leaderRanks,lay2.leaderRanks));
	
}

void hit_layPrint (HitLayout lay){

}

int main(int argc, char* argv[])
{

    //https://www.infor.uva.es/~eduardo/blog/index.php/2016/05/09/debugging-mpi-programs-with-gdb/
    /*int iax = 0;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("PID %d on %s ready for attach\n", getpid(), hostname);
    fflush(stdout);
    while (0 == iax)
          sleep(5);*/

	hit_comInit( &argc, &argv );

	HitTile_int globalTile, myTile;
	HitShape globalShape, myShape;
	HitShape myDimWeightedShape, myDimDummyWeightedShape, myWeightedShape;
	HitLayout globalLayout, dimWeightedLayout, dimDummyWeightedLayout, weightedLayout;
	HitTopology globalTopology, dummyTopology;
	int i;
	//float weights[20] = {1,5,8,4,2,9,7,2,7,0,8,4,5,8,7,5,1,8,3,7};
	float weights[4] = {2,2,4,2};//Normalizar con hitNProcs
	//float weights[hit_NProcs] = {2,2,4,2};//Normalizar con hitNProcs

	if (argc < 2)
		{
		printf("Usage: %s <cells>\n^[[31mCells argument mising.^[[0m\n", argv[0]);
		return -1;
		}
	int cells = atoi(argv[1]);

	hit_tileDomain(&globalTile, int, 1, cells);
	globalTopology = hit_topology(plug_topPlain);
	int dummyProcElems[1] = {4};
	dummyTopology = hit_topology(plug_topDummyDims, 1, dummyProcElems);
	globalShape = hit_tileShape(globalTile);
	//globalLayout = hit_layout(plug_layBlocks, globalTopology, globalShape);

	//activesTopology = hit_layActivesTopology(globalLayout);
	//globalLayout = hit_layout(plug_layContiguous, globalTopology, globalShape, weights);
	//dimWeightedLayout = hit_layout(plug_layDimWeighted, activesTopology, globalShape, restrictDim, weights);
	int restrictDim = 0;
	dimWeightedLayout = hit_layout(plug_layDimWeighted, globalTopology, globalShape, restrictDim, weights);
	dimDummyWeightedLayout = hit_layout(plug_layDimWeighted, dummyTopology, globalShape, restrictDim, weights);
	if (hit_layCmp(dimDummyWeightedLayout, HIT_LAYOUT_NULL))
		printf("[31mEl resultado de aplicar layDimWeighted a topDummyDims es null[0m");
	
	//dimWeightedLayout = hit_layout(plug_layBlocks, globalTopology, globalShape);
	//weightedLayout = hit_layout(plug_layWeighted, activesTopology, globalShape, weights);
	//dimWeightedLayout = hit_layout(plug_layWeighted, globalTopology, globalShape, weights);

	printf("[%i:%i:%i (%i)]", hit_tileDimBegin(globalTile, 0), hit_tileDimEnd(globalTile, 0), 
				  hit_tileDimStride(globalTile, 0), hit_tileDimCard(globalTile, 0));

	myDimWeightedShape = hit_layShape(dimWeightedLayout);

	HitSig aSig;
	int j;
	for (j = 0; j < 4; j++){
		//myDimDummyWeightedShape = hit_layShape(dimDummyWeightedLayout);
		myDimDummyWeightedShape = hit_layShapeNeighbor(dimDummyWeightedLayout, 0, j); //j = shift
		printf("\n%i dimension, virtual_rank %i",hit_shapeDims(myDimDummyWeightedShape), j);
		for (i = 0; i < hit_shapeDims(myDimDummyWeightedShape); i++){
			aSig = hit_shapeSig(myDimDummyWeightedShape, i);
			printf("[32m[%i:%i:%i (%i)][0m", aSig.begin, aSig.end, aSig.stride, hit_sigCard(aSig));
			}
		}
	printf("\n");
		
	//myWeightedShape = hit_layShape(weightedLayout);
	hit_tileSelect(&myTile, &globalTile, myDimWeightedShape);
	//hit_tileSelect(&myTile, &globalTile, myweightedShape);
	hit_tileAlloc(&myTile);

	/*myShape = hit_layShape(groupLayouts[hit_Rank]);
	hit_tileSelect(&myTile, &globalTile, myShape);
	hit_tileAlloc(&myTile);*/

	//Fill the array with some values (e.g. its local indexes)
	for(i = 0; i < hit_tileDimCard(myTile, 0); i++)
		hit_tileElemAt(myTile, 1, i) = i;
	printHitTile(dimWeightedLayout, myTile);
	//printHitTile(weightedLayout, myTile);
	if(hit_layImActive(dimWeightedLayout))
		hit_comBarrier(dimWeightedLayout);
	/*if(hit_layImActive(weightedLayout))
		hit_comBarrier(weightedLayout);*/

	//printf("Hola"); fflush(stdout);
	hit_tileFree(myTile);
	hit_shapeFree(myDimWeightedShape);
	//hit_shapeFree(myWeightedShape);
	hit_layFree(dimWeightedLayout);
	//hit_layFree(weightedLayout);
	//hit_layFree(globalLayout);
	//XXX: ESTA TOPOLOGIA NO SE PUEDE LIBERAR PORQUE ES NULL hit_topFree(dummyTopology);
	hit_topFree(globalTopology);
	hit_comFinalize();
	return 0;
}
