/**
 * Program to test the access performance of the different tiles.
 *
 * @file matrix_sig_vs_bit.c
 * @author Javier Fresno
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
 * Copyright (c) 2007-2019, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#include <stdio.h>
#include <stdlib.h>
#include <hitmap.h>
#include <hit_com.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


// Define the Tile_int
hit_tileNewType(int);

// Layout
HitLayout lay = HIT_LAYOUT_NULL_STATIC;


#define printfR(...) { if( hit_Rank == 0 ) { printf(__VA_ARGS__); fflush(stdout); }}



int main(int argc, char ** vargs){

	int nodes;

	if(argc != 2){
		printf("Usage: %s NUMBER_OF_NODES\n",vargs[0]);
		exit(0);
	}
	sscanf(vargs[1],"%d",&nodes);
	
	hit_comInit(&argc,&vargs);
	
	printfR("nodes: %d\n",nodes);
	
	printfR("Sig Shape\tSig Tile\tSTile NStride\tBitmap Shape\tGBTile iterator\tGBTile direct\n");
	
	int i,j;
	HitClock t;
	
	// A.1 Create Sig Shape
	hit_clockStart(t);
	HitSig sig = hit_sig(0,nodes-1,1);
	HitShape shape = hit_shape(2,sig,sig);
	hit_clockStop(t);
	printfR("%f\t",hit_clockGetSeconds(t));
	
	// A.2 Allocate and set zeros
	HitTile_int tile;
	hit_tileDomainShapeAlloc(&tile,int,shape);
	
	hit_clockStart(t);
	for(i=0;i<nodes;i++)
	for(j=0;j<nodes;j++){
		hit_tileElemAt(tile,2,i,j) = 0;
	}
	hit_clockStop(t);
	printfR("%f\t",hit_clockGetSeconds(t));
	
	// A.3 set zeros no stride
	hit_clockStart(t);
	for(i=0;i<nodes;i++)
	for(j=0;j<nodes;j++){
		hit_tileElemAtNoStride(tile,2,i,j) = 0;
	}
	hit_clockStop(t);
	printfR("%f\t",hit_clockGetSeconds(t));
	
	// A.3 Free
	hit_tileFree(tile);

	
	
	// B.1 Create Bit shape
	hit_clockStart(t);
	HitShape bshape = hit_bitmapShape(nodes);
	for(i=0;i<nodes;i++)
	for(j=0;j<nodes;j++){
		//hit_bShapeAddEdge(&bshape,i,j);
		hit_bShapeSet(bshape,i,j);
	}
	hit_clockStop(t);
	printfR("%f\t",hit_clockGetSeconds(t));
	
	// B.2 Allocate and set zeros
	HitTile_int btile;
	hit_gbTileDomainShapeAlloc(&btile, int, bshape, HIT_EDGES);
	
	hit_clockStart(t);
	int vertex, edge;
	hit_bShapeVertexIterator(vertex,bshape){
		hit_bShapeEdgeIterator(edge,bshape,vertex){
		
			hit_gbTileEdgeIteratorAt(btile,vertex,edge) = 0;
		
		}
	}
	hit_clockStop(t);
	printfR("%f\t",hit_clockGetSeconds(t));
	
	
	// B.3 set zeros without iterator
	hit_clockStart(t);
	for(i=0;i<nodes;i++)
	for(j=0;j<nodes;j++){
		hit_gbTileEdgeAt(btile,i,j) = 0;
	}
	hit_clockStop(t);
	printfR("%f\t",hit_clockGetSeconds(t));
	
	
	// B.4 Free
	hit_tileFree(btile);
	
	printfR("\n");
	
	hit_comFinalize();
	
	return 0;
}














