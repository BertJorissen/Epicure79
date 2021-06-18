/**
 * MG Hitmap
 *
 * NAS Multigrid Benchmark implemented using Hitmap library.
 *
 * @version 1.0
 * @date Jun 2010
 * @author Javier Fresno Bausela
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

/* INCLUDES */
#include <hitmap.h>
#include <math.h>
#include <time.h>
#include "c_randi8.h"


/* ACTIVE OPTIMIZATIONS: The same as NAS version */
#define OPT_ZU
#define OPT_OPERATOR_A
#define OPT_OPERATOR_S
#define OPT_RESID_BUFFER
#define OPT_PSINV_BUFFER
#define OPT_RPRJ3_BUFFER
#define OPT_INTERP_BUFFER


/* DEBUG */
//#define DEBUG

/* CLASS NAMES */
#define S 'S'
#define A 'A'
#define B 'B'
#define C 'C'
#define D 'D'
#define E 'E'

/* DEFAULT PROBLEM CLASS */
#ifndef CLASS
#   define CLASS S
#endif

/**
 * PROBLEM DATA
 * - k_mun: Number of levels
 * - iterations: Iterations number
 * - problem_size: Problem size
 * - verify_value: Norm L2 value
 */
#if CLASS == S
#   define k_num 5
#   define iterations 4
    int problem_size[3] = {32,32,32};
    double verify_value = 0.5307707005734e-04;
#endif

#if CLASS == A
#   define k_num 8
#   define iterations   4
    int problem_size[3] = {256,256,256};
    double verify_value = 0.2433365309069e-05;
#endif

#if CLASS == B
#   define k_num 8
#   define iterations 20
    int problem_size[3] = {256,256,256};
    double verify_value = 0.1800564401355e-05;
#endif

#if CLASS == C
#   define k_num 9
#   define iterations 20
    int problem_size[3] = {512,512,512};
    double verify_value = 0.5706732285740e-06;
#endif

#if CLASS == D
#   define k_num 10
#   define iterations 50
    int problem_size[3] = {1024,1024,1024};
    double verify_value = 0.1583275060440e-09;
#endif

#if CLASS == E
#   define k_num 11
#   define iterations 50
    int problem_size[3] = {2048,2048,2048};
    double verify_value = 0.5630442584711e-10;
#endif

/** MAX LEVEL */
#define k_max (k_num-1)

/* MAX VERIFY DIFERENCE */
#define epsilon 1e-8

/* OPERATORS */
/** Laplace operator */
double operatorA[4] = {-8.0/3.0, 0.0, 1.0/6.0, 1.0/12.0};
/** Restrict operator */
double operatorP[4] = {1.0/2.0, 1.0/4.0, 1.0/8.0, 1.0/16.0};
#if CLASS == S || CLASS == A
/** Smoother operator (for class S and A) */
double operatorS[4] = {-3.0/8.0, 1.0/32.0, -1.0/64.0, 0.0};
#else
/** Smoother operator (other classes) */
double operatorS[4] = {-3.0/17.0, 1.0/33.0, -1.0/61.0, 0.0};
#endif
/** Prolongate operator */
double operatorQ[4] = {1.0, 1.0/2.0, 1.0/4.0, 1.0/8.0};

/* Number of +1 or -1 values in grid v */
#define mm 10

/* TOPOLOGY AND LAYOUT FOR EACH LEVEL */
HitTopology topology;
HitLayout layout[k_num];

/* HITMAP DOUBLE TILE */
hit_tileNewType(double);

/* BLOCK SHAPES */
HitShape block[k_num];

/* GLOBAL CUBIC GRID */
HitShape grid[k_num];

/* BLOCK TILES (include border data) */
HitTile_double block_u;
HitTile_double block_v;
HitTile_double block_z[k_num];
HitTile_double block_r[k_num];

/* COMMUNICATION PATTERNS */
HitPattern pattern_v;
HitPattern pattern_r[k_num];
HitPattern pattern_z[k_num];
HitPattern pattern_z_ex[k_num];

/* HITMAP REDUCE OPERATIONS */
HitOp op_max;
HitOp op_min;
HitOp op_sum;

/* HITMAP CLOCKS */
HitClock setupClk;
HitClock initClk;
HitClock processClk;

/** Macro to swap two numbers */
#define swap(a,b,c){    \
    (c) = (a);  \
    (a) = (b);  \
    (b) = (c);  \
}

/** Macro to calculate the tag for the communications */
#define tag(dir) (100 * (dir + 10*dim))


#ifdef DEBUG
/* Macros to sequentialize debug parallel operations */
#define ALL_SEQUENTIAL_BEGIN(var) \
	struct timespec var ## ts = {0,1000000}; \
	fflush(stdout); \
	int var = 0; \
	for(var=-1;var<hit_NProcs+1;var++){ \
		if(hit_Rank == var){
#define ALL_SEQUENTIAL_END(var) \
		} else { \
			nanosleep(&(var ## ts),NULL); \
		} \
		fflush(stdout); \
		MPI_Barrier(*hit_Comm); \
	}
#endif


/* FUNCTION PROTOTYPES */
void setup();
void warming_up();
void zero3(HitTile_double tile);
void zran3(HitTile_double matrix);
double power(double a, int n);
void bubble(double ten[mm][2], int ten_index[mm][2][3],int ind);
void resid(HitTile_double m_u, HitTile_double m_v, HitTile_double m_r, int k);
void apply_correction(HitTile_double u, HitTile_double r, int k);
void v_cycle(HitTile_double r, HitTile_double z, int k);
void rprj3(HitTile_double m_ro, HitTile_double m_r, int k);
void interp(HitTile_double m_zo, HitTile_double m_z, int k);
void psinv(HitTile_double m_z, HitTile_double m_r, int k);
double norm2u3(HitTile_double m_r,int k);
void free_resources();
void add(HitTile_double m_x, HitTile_double m_y);

/* OUTPUT FUNCIONS */
#define printfRootInternal(...) { if( hit_Rank == 0 ) { printf(__VA_ARGS__); fflush(stdout); }}
#define printfRoot(...) printfRootInternal(__VA_ARGS__)

/* DEBUG OUTPUT FUNCIONS */
#ifdef DEBUG
#define printfAllDebug(...)  printfAllInternal(__VA_ARGS__)
#define printfRootDebug(...) printfRootInternal(__VA_ARGS__)
#define printfDebug(...)     printf(__VA_ARGS__)
void printfAllInternal( const char* format, ... );
#else
#define printfAllDebug(...)
#define printfRootDebug(...)
#define printfDebug(...)
#endif


/**
 * Main function
 */
int main(int argc, char ** argv){

	/* Initialize hitmap communication */
	hit_comInit(&argc,&argv);
	printfRootDebug("Initialize Hitmap\n");


	// Check that nprocs = 2^x 
	int j;
	int sum = 0;
	int nprocs = hit_NProcs;
	for( j=0; (size_t)j < sizeof(int)*8 ; j++ ){
		sum += (nprocs >> j) & 1;
	}
	if(sum != 1){
		printfRoot("\nThe number of processors must be a power of two\n\n");
		hit_comFinalize();
		exit(EXIT_FAILURE);
	}



#ifdef DEBUG
	printfRoot("DEBUG\n");
#endif

	hit_clockSynchronizeAll();

	hit_clockStart(initClk);
	hit_clockStart(setupClk);
	/* Setup function */
	setup();
	hit_clockStop(setupClk);


	/* Warming up iteration */
	warming_up();

	/* Init u and v grids*/
	zero3(block_u);
	zran3(block_v);

	hit_clockStop(initClk);
	hit_comBarrier(layout[k_max]);
	hit_clockStart(processClk);

	/* Main loop */
	int i;
	for (i = 0; i < iterations; i++) {
		printfRoot(" * ITERATION %d *\n",i);
		resid(block_u, block_v, block_r[k_max], k_max);
		apply_correction(block_u, block_r[k_max], k_max);
	}
	resid(block_u, block_v, block_r[k_max], k_max);

	/* Calculate L2 Norm */
	double result = norm2u3(block_r[k_max],k_max);

	hit_clockStop(processClk);

	hit_clockReduce( layout[k_max], setupClk );
	hit_clockReduce( layout[k_max], initClk );
	hit_clockReduce( layout[k_max], processClk );

	/* Print results */
	if( fabs( result - verify_value ) <= epsilon ){
		printfRoot("VERIFICATION SUCCESSFUL\n");
		printfRoot("\tL2 Norm is:            %20.12e\n",result);
		printfRoot("\tThe correct L2 Norm is %20.12e\n",verify_value);
		printfRoot("\tError:                 %20.12e\n",result-verify_value);
	} else {
		printfRoot("VERIFICATION FAILED\n");
		printfRoot("\tL2 Norm is:             %20.12e\n",result);
		printfRoot("\tThe correct L2 Norm is: %20.12e\n",verify_value);
		printfRoot("\tError:                  %20.12e\n",result-verify_value);
	}
	printfRoot("Setup time:   %12.3f\n",hit_clockGetMaxSeconds(setupClk));
	printfRoot("Init time:    %12.3f\n",hit_clockGetMaxSeconds(initClk));
	printfRoot("Process time: %12.2f\n",hit_clockGetMaxSeconds(processClk));

	// Free the resources
    free_resources();

	printfRootDebug("Finalize hitmap\n");
	hit_comFinalize();

	return 0;
}



/**
 * Setup function
 */
void setup(){

	printfRootDebug(" = SETUP = \n");

	HitSig sig0, sig1, sig2;
	HitShape blockEx;
    int k, dim;

    /* 3D Array Topology */
	topology = hit_topology(plug_topArray3D);

	/* Problem size at current level */
	int current_size[3];
	current_size[0] = problem_size[0];
	current_size[1] = problem_size[1];
	current_size[2] = problem_size[2];

	/* Loop through all levels */
	for(k=k_num-1;k>=0;k--){

		printfRootDebug("* Allocating grids for level: %d\n",k);

		/* Create shape for global cubic grid */
		sig0 = hit_sig(0,current_size[0]-1,1);
		sig1 = hit_sig(0,current_size[1]-1,1);
		sig2 = hit_sig(0,current_size[2]-1,1);
		grid[k] = hit_shape(3,sig0,sig1,sig2);

		/* Layout for the k level */
		layout[k] = hit_layout(plug_layBlocksL,topology,grid[k]);
		hit_layWrapNeighbors(&(layout[k]));

		/* My block at level k */
		block[k] = hit_layShape(layout[k]);

#ifdef DEBUG
		if(hit_shapeCmp(block[k],HIT_SHAPE_NULL)){
			printfAllDebug(" I have: nothing\n");
		} else{
			printfAllDebug("I have: [%d-%d,%d-%d,%d-%d]\n",
					hit_shapeSig(block[k],0).begin,
					hit_shapeSig(block[k],0).end,
					hit_shapeSig(block[k],1).begin,
					hit_shapeSig(block[k],1).end,
					hit_shapeSig(block[k],2).begin,
					hit_shapeSig(block[k],2).end);
		}
#endif

		/* Calculate the block with the borders */
		blockEx = hit_shapeExpand(block[k],3,1);

		/* Allocates for the top level: u, v, r and z. */
		if(k == k_max){

			printfRootDebug(" -> Allocating blocks u, v, r, z\n");

			hit_tileDomainShapeAlloc(&block_v,double,blockEx);

			hit_tileDomainShapeAlloc(&(block_r[k]),double,blockEx);
			hit_tileDomainShapeAlloc(&(block_z[k]),double,blockEx);
#ifdef OPT_ZU
			// OPT_ZU: Using this optimization, grid u and grid z are the same at max level
			block_u = block_z[k];
#else
			hit_tileDomainShapeAlloc(&block_u,double,blockEx);
#endif

		/* Allocates for other levels: r and z. */
		} else {

			printfRootDebug(" -> Allocating blocks r, z\n");

			hit_tileDomainShapeAlloc(&(block_r[k]),double,blockEx);
			hit_tileDomainShapeAlloc(&(block_z[k]),double,blockEx);
		}


		/* Communication patterns */
		printfRootDebug("* Creating Comm Pattern for level: %d\n",k);
		if(k == k_max)
			pattern_v = hit_pattern( HIT_PAT_ORDERED );
		pattern_r[k] = hit_pattern( HIT_PAT_ORDERED );
		pattern_z[k] = hit_pattern( HIT_PAT_ORDERED );
		pattern_z_ex[k] = hit_pattern( HIT_PAT_ORDERED );

		/* Border exchange */
		for(dim=0;dim<3;dim++){

			/* Neighbor location */
            HitRanks nbr_l = hit_layNeighbor(layout[k],dim,-1);
            HitRanks nbr_r = hit_layNeighbor(layout[k],dim,+1);

            /* Shape for the face to Give and to Take */
			HitShape faceGive_l, faceGive_r, faceTake_l, faceTake_r;

			faceGive_l = hit_shapeBorder(block[k],dim,HIT_SHAPE_BEGIN,0);
			faceGive_r = hit_shapeBorder(block[k],dim,HIT_SHAPE_END,0);

			faceTake_l = hit_shapeBorder(block[k],dim,HIT_SHAPE_BEGIN,1);
			faceTake_r = hit_shapeBorder(block[k],dim,HIT_SHAPE_END,1);

			faceGive_l = hit_shapeExpand(faceGive_l,dim,1);
			faceGive_r = hit_shapeExpand(faceGive_r,dim,1);
			faceTake_l = hit_shapeExpand(faceTake_l,dim,1);
			faceTake_r = hit_shapeExpand(faceTake_r,dim,1);

			printfAllDebug("Face Give l, dim %d: [%d-%d,%d-%d,%d-%d]\n",dim,
					hit_shapeSig(faceGive_l,0).begin,
					hit_shapeSig(faceGive_l,0).end,
					hit_shapeSig(faceGive_l,1).begin,
					hit_shapeSig(faceGive_l,1).end,
					hit_shapeSig(faceGive_l,2).begin,
					hit_shapeSig(faceGive_l,2).end);

			printfAllDebug("Face Give r, dim %d: [%d-%d,%d-%d,%d-%d]\n",dim,
					hit_shapeSig(faceGive_r,0).begin,
					hit_shapeSig(faceGive_r,0).end,
					hit_shapeSig(faceGive_r,1).begin,
					hit_shapeSig(faceGive_r,1).end,
					hit_shapeSig(faceGive_r,2).begin,
					hit_shapeSig(faceGive_r,2).end);

			printfAllDebug("Face Take l, dim %d: [%d-%d,%d-%d,%d-%d]\n",dim,
					hit_shapeSig(faceTake_l,0).begin,
					hit_shapeSig(faceTake_l,0).end,
					hit_shapeSig(faceTake_l,1).begin,
					hit_shapeSig(faceTake_l,1).end,
					hit_shapeSig(faceTake_l,2).begin,
					hit_shapeSig(faceTake_l,2).end);

			printfAllDebug("Face Take r, dim %d: [%d-%d,%d-%d,%d-%d]\n",dim,
					hit_shapeSig(faceTake_r,0).begin,
					hit_shapeSig(faceTake_r,0).end,
					hit_shapeSig(faceTake_r,1).begin,
					hit_shapeSig(faceTake_r,1).end,
					hit_shapeSig(faceTake_r,2).begin,
					hit_shapeSig(faceTake_r,2).end);

			/* Default Communications Patterns */
			hit_patternAdd( &pattern_r[k],
					hit_comSendRecvSelectTag(layout[k], nbr_l, &block_r[k], faceGive_l, HIT_COM_ARRAYCOORDS, nbr_r, &block_r[k], faceTake_r, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(0)));
			hit_patternAdd( &pattern_r[k],
					hit_comSendRecvSelectTag(layout[k], nbr_r, &block_r[k], faceGive_r, HIT_COM_ARRAYCOORDS, nbr_l, &block_r[k], faceTake_l, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(1)));

			hit_patternAdd( &pattern_z[k],
					hit_comSendRecvSelectTag(layout[k], nbr_l, &block_z[k], faceGive_l, HIT_COM_ARRAYCOORDS, nbr_r, &block_z[k], faceTake_r, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(0)));
			hit_patternAdd( &pattern_z[k],
					hit_comSendRecvSelectTag(layout[k], nbr_r, &block_z[k], faceGive_r, HIT_COM_ARRAYCOORDS, nbr_l, &block_z[k], faceTake_l, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(1)));

			if(k == k_max){
				hit_patternAdd( &pattern_v,
						hit_comSendRecvSelectTag(layout[k], nbr_l, &block_v, faceGive_l, HIT_COM_ARRAYCOORDS, nbr_r, &block_v, faceTake_r, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(0)));
				hit_patternAdd( &pattern_v,
						hit_comSendRecvSelectTag(layout[k], nbr_r, &block_v, faceGive_r, HIT_COM_ARRAYCOORDS, nbr_l, &block_v, faceTake_l, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(1)));

			} else {

				/* Neighbor when there are inactive processors */
	            HitRanks nbr_l_ex = hit_layNeighbor(layout[k+1],dim,-1);
	            HitRanks nbr_r_ex = hit_layNeighbor(layout[k+1],dim,+1);

	            /* Neighbor shapes at this and next level */
				HitShape shape = hit_layShapeOther(layout[k], nbr_l_ex);
				HitShape shape2 = hit_layShapeOther(layout[k+1], nbr_l_ex);


#ifdef DEBUG
				ALL_SEQUENTIAL_BEGIN(seq)
#endif




				/* This processor has to GIVE data to rebuild a inactive processor */
				if (hit_sigCard(hit_shapeSig(block[k+1],dim)) == 1 && hit_sigCard(hit_shapeSig(shape,dim)) == 0 && hit_sigCard(hit_shapeSig(shape2,dim)) == 1) {
					printfDebug("p%d: GIVE, level %d, dim %d\n",HIT_TOPOLOGY_INFO.selfRank,k+1,dim);

					HitShape faceGive_l_ex = hit_shapeExpand(block[k+1],3,1);
					HitShape faceGive_r_ex = hit_shapeExpand(block[k+1],3,1);

					hit_shapeSig(faceGive_l_ex,dim).end -= 1;
					hit_shapeSig(faceGive_r_ex,dim).begin += 1;
					hit_shapeSig(faceGive_r_ex,dim).end -= 1;

					printfDebug("Face Give l ex, dim %d: [%d-%d,%d-%d,%d-%d] Cards[%d,%d,%d]\n",dim,
							hit_shapeSig(faceGive_l_ex,0).begin,
							hit_shapeSig(faceGive_l_ex,0).end,
							hit_shapeSig(faceGive_l_ex,1).begin,
							hit_shapeSig(faceGive_l_ex,1).end,
							hit_shapeSig(faceGive_l_ex,2).begin,
							hit_shapeSig(faceGive_l_ex,2).end,
							hit_sigCard(hit_shapeSig(faceGive_l_ex,0)),
							hit_sigCard(hit_shapeSig(faceGive_l_ex,1)),
							hit_sigCard(hit_shapeSig(faceGive_l_ex,2)));

					printfDebug("Face Give r ex, dim %d: [%d-%d,%d-%d,%d-%d] Cards[%d,%d,%d]\n",dim,
							hit_shapeSig(faceGive_r_ex,0).begin,
							hit_shapeSig(faceGive_r_ex,0).end,
							hit_shapeSig(faceGive_r_ex,1).begin,
							hit_shapeSig(faceGive_r_ex,1).end,
							hit_shapeSig(faceGive_r_ex,2).begin,
							hit_shapeSig(faceGive_r_ex,2).end,
							hit_sigCard(hit_shapeSig(faceGive_r_ex,0)),
							hit_sigCard(hit_shapeSig(faceGive_r_ex,1)),
							hit_sigCard(hit_shapeSig(faceGive_r_ex,2)));

					hit_patternAdd( &pattern_z_ex[k+1],
							hit_comSendSelectTag(layout[k+1], nbr_l_ex, &block_z[k+1], faceGive_l_ex, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(0)));
					hit_patternAdd( &pattern_z_ex[k+1],
							hit_comSendSelectTag(layout[k+1], nbr_r_ex, &block_z[k+1], faceGive_r_ex, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(1)));


				}
				/* This processor has to TAKE data to rebuild himself because it is inactive  */
				if (hit_sigCard(hit_shapeSig(block[k],dim)) == 0 && hit_sigCard(hit_shapeSig(shape,dim)) == 1 && hit_sigCard(hit_shapeSig(shape2,dim)) == 1) {
					printfDebug("p%d: TAKE, level %d, dim %d\n",HIT_TOPOLOGY_INFO.selfRank,k+1,dim);

					HitShape faceTake_l_ex = hit_shapeExpand(block[k+1],3,1);
					HitShape faceTake_r_ex = hit_shapeExpand(block[k+1],3,1);

					hit_shapeSig(faceTake_l_ex,dim).end -= 2;
					hit_shapeSig(faceTake_r_ex,dim).begin += 1;

					printfDebug("Face Take l ex, dim %d: [%d-%d,%d-%d,%d-%d] Cards[%d,%d,%d]\n",dim,
							hit_shapeSig(faceTake_l_ex,0).begin,
							hit_shapeSig(faceTake_l_ex,0).end,
							hit_shapeSig(faceTake_l_ex,1).begin,
							hit_shapeSig(faceTake_l_ex,1).end,
							hit_shapeSig(faceTake_l_ex,2).begin,
							hit_shapeSig(faceTake_l_ex,2).end,
							hit_sigCard(hit_shapeSig(faceTake_l_ex,0)),
							hit_sigCard(hit_shapeSig(faceTake_l_ex,1)),
							hit_sigCard(hit_shapeSig(faceTake_l_ex,2)));

					printfDebug("Face Take r ex, dim %d: [%d-%d,%d-%d,%d-%d] Cards[%d,%d,%d]\n",dim,
							hit_shapeSig(faceTake_r_ex,0).begin,
							hit_shapeSig(faceTake_r_ex,0).end,
							hit_shapeSig(faceTake_r_ex,1).begin,
							hit_shapeSig(faceTake_r_ex,1).end,
							hit_shapeSig(faceTake_r_ex,2).begin,
							hit_shapeSig(faceTake_r_ex,2).end,
							hit_sigCard(hit_shapeSig(faceTake_r_ex,0)),
							hit_sigCard(hit_shapeSig(faceTake_r_ex,1)),
							hit_sigCard(hit_shapeSig(faceTake_r_ex,2)));

					hit_patternAdd( &pattern_z_ex[k+1],
							hit_comRecvSelectTag(layout[k+1], nbr_l_ex, &block_z[k+1], faceTake_l_ex, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(1)));
					hit_patternAdd( &pattern_z_ex[k+1],
							hit_comRecvSelectTag(layout[k+1], nbr_r_ex, &block_z[k+1], faceTake_r_ex, HIT_COM_ARRAYCOORDS, HIT_DOUBLE,tag(0)));
				}
#ifdef DEBUG
				ALL_SEQUENTIAL_END(seq)
#endif

			}
		}

		/* Problem size of the next level */
		current_size[0] /= 2;
		current_size[1] /= 2;
		current_size[2] /= 2;
	}

	/* Hit collective functions */
	hit_comOp(hit_comOpMaxDouble,op_max);
	hit_comOp(hit_comOpMinDouble,op_min);
	hit_comOp(hit_comOpSumDouble,op_sum);
}



/**
 * Cleans all the given Tile to zero.
 * @param tile The tile to clean.
 */
void zero3(HitTile_double tile){

    double aux=0.0;
	hit_tileFill( &tile, &aux );

}


/**
 *
 */
void zran3(HitTile_double matrix){

	printfRootDebug(" = ZRAN3 = \n");


	int b1 = hit_tileArray2Tile(matrix,0,hit_shapeSig(block[k_max],0).begin);
	int b2 = hit_tileArray2Tile(matrix,1,hit_shapeSig(block[k_max],1).begin);
	int b3 = hit_tileArray2Tile(matrix,2,hit_shapeSig(block[k_max],2).begin);

    int n1 = hit_sigCard(hit_shapeSig(block[k_max],0));
    int n2 = hit_sigCard(hit_shapeSig(block[k_max],1));
    int n3 = hit_sigCard(hit_shapeSig(block[k_max],2));

    int e1 = b1 + n1;
    int e2 = b2 + n2;
    int e3 = b3 + n3;

#undef m
#define m(a,b,c) hit_tileElemAtNoStride3(matrix,a,b,c)

#define A0    pow(5.0,13)
#define x    314159265.e0


    /*
     * a1 = A0^{ dim grid z }
     * a2 = A0^{ dim griz y * dim grid z }
     */
    double a1 = power( A0, hit_sigCard(hit_shapeSig(grid[k_max],2)) );
    double a2 = power( A0, hit_sigCard(hit_shapeSig(grid[k_max],1)) * hit_sigCard(hit_shapeSig(grid[k_max],2)) );

    /* Clean the block */
    zero3(matrix);

    /* First item of the block in a linear mode */
    int i = hit_shapeSig(block[k_max],2).begin + hit_sigCard(hit_shapeSig(grid[k_max],2)) *
        (hit_shapeSig(block[k_max],1).begin + hit_sigCard(hit_shapeSig(grid[k_max],1)) * (hit_shapeSig(block[k_max],0).begin ));

    printfAllDebug("My first item is: %d\n",i);

    // ai = a^i
    double ai = power( A0, i );

    double x0 = x;
    int rdummy = (int) randlc( &x0, ai );

    int i1,i2,i3;
    for(i1=b1;i1<e1;i1++){

        double x1 = x0;
        for(i2=b2;i2<e2;i2++){

            double xx = x1;
            /* Fill the vector (i1,i2,b3) with n3 pseudo-random numbers */
            vranlc(n3,&xx,A0,&m(i1,i2,b3));
            rdummy = (int) randlc(&x1,a1);
        }
        rdummy = (int) randlc(&x0,a2);
    }
	// @arturo Ago 2015: Avoid a warning of variable initialized but not used
	(void)rdummy;

    /* Arrays to keep the +1 and -1 values */
    double ten[mm][2];
    int ten_index[mm][2][3];

    /* Start ten and ten_index with 0 */
    for(i=0;i<mm;i++){

        ten[i][1] = 0;
        ten_index[i][1][0] = 0;
        ten_index[i][1][1] = 0;
        ten_index[i][1][2] = 0;

        ten[i][0] = 1;
        ten_index[i][0][0] = 0;
        ten_index[i][0][1] = 0;
        ten_index[i][0][2] = 0;
    }

    /* Find +1 and -1 values */
	for (i1 = b1; i1 < e1; i1++) {
		for (i2 = b2; i2 < e2; i2++) {
			for (i3 = b3; i3 < e3; i3++) {

				if (m(i1,i2,i3) > ten[0][1]) {

					ten[0][1] = m(i1,i2,i3);
					ten_index[0][1][0] = i1;
					ten_index[0][1][1] = i2;
					ten_index[0][1][2] = i3;

					bubble(ten, ten_index, 1);
				}

				if (m(i1,i2,i3) < ten[0][0]) {

					ten[0][0] = m(i1,i2,i3);
					ten_index[0][0][0] = i1;
					ten_index[0][0][1] = i2;
					ten_index[0][0][2] = i3;

					bubble(ten, ten_index, 0);
				}
			}
		}
	}


#ifdef DEBUG
    char a_aux[1024];
    char * aux;
    aux = a_aux;
    int j;
   	for(j=0;j<mm;j++){
    	aux += sprintf(aux,"[%d,%d,%d](%f) ",
    			hit_tileTile2Array(matrix,0,ten_index[j][1][0]),
    			hit_tileTile2Array(matrix,0,ten_index[j][1][1]),
    			hit_tileTile2Array(matrix,0,ten_index[j][1][2]),
    			m(ten_index[j][0][0],ten_index[j][0][1],ten_index[j][0][2]));
    }
    printfAllDebug("My -1 are: %s\n",a_aux);
    aux = a_aux;
   	for(j=0;j<mm;j++){
    	aux += sprintf(aux,"[%d,%d,%d](%f) ",
    			hit_tileTile2Array(matrix,0,ten_index[j][1][0]),
    			hit_tileTile2Array(matrix,0,ten_index[j][1][1]),
    			hit_tileTile2Array(matrix,0,ten_index[j][1][2]),
    			m(ten_index[j][1][0],ten_index[j][1][1],ten_index[j][1][2]));
    }
    printfAllDebug("My +1 are: %s\n",a_aux);
#endif


    i1 = mm-1;
    int i0 = mm-1;
    HitTile_double temp, best;

	hit_tileDomainAlloc(&temp, double, 1,1);
	hit_tileDomainAlloc(&best, double, 1,1);
	HitCom com_max = hit_comReduce(layout[k_max], HIT_RANKS_NULL, &best, &temp, HIT_DOUBLE, op_max);
	HitCom com_min = hit_comReduce(layout[k_max], HIT_RANKS_NULL, &best, &temp, HIT_DOUBLE, op_min);


	 for (i = mm - 1; i >= 0; i--) {

		hit_tileElemAt(best,1,0) = m(ten_index[i1][1][0],ten_index[i1][1][1],ten_index[i1][1][2]);
		hit_comDo(&com_max);
		if (hit_tileElemAt(best,1,0) == hit_tileElemAt(temp,1,0)) {
			i1--;
		}
		ten[i][1] = hit_tileElemAt(best,1,0);

		hit_tileElemAt(best,1,0) = m(ten_index[i0][0][0],ten_index[i0][0][1],ten_index[i0][0][2]);
		hit_comDo(&com_min);
		if (hit_tileElemAt(best,1,0) == hit_tileElemAt(temp,1,0)) {
			i0--;
		}
		ten[i][0] = hit_tileElemAt(best,1,0);
	}

	int m1 = i1 + 1;
	int m0 = i0 + 1;

	zero3(matrix);

	for (i = mm - 1; i >= m1; i--)
		m(ten_index[i][1][0],ten_index[i][1][1],ten_index[i][1][2]) = +1.0;
	for (i = mm - 1; i >= m0; i--)
		m(ten_index[i][0][0],ten_index[i][0][1],ten_index[i][0][2]) = -1.0;


	printfAllDebug("I have %d +1s\n",mm-m1);
	printfAllDebug("I have %d -1s\n",mm-m0);

	hit_tileFree(best);
	hit_tileFree(temp);
	hit_comFree(com_max);
	hit_comFree(com_min);

	hit_patternDo(pattern_v);

}



/**
 * raises an integer, disguised as a double precision real, to an integer power
 *
 * Used by zran3 to speed up pseudo-random block generator
 */
double power(double a, int n){

    double aj;
    int nj;
    double rdummy;

    double powerv = 1.0;

    nj = n;
    aj = a;

    while (nj != 0) {
        if( (nj%2) == 1 ) rdummy =  randlc( &powerv, aj );
        rdummy = randlc( &aj, aj );
        nj = nj/2;
    }
	// @arturo Ago 2015: Avoid a warning of variable initialized but not used
	(void)rdummy;

    return powerv;
}




void bubble(double ten[mm][2], int ten_index[mm][2][3],int ind){

    double temp;
    int i, temp_index;

    if(ind == 1){

        for(i=0;i<mm-1;i++){
            if( ten[i][ind] > ten[i+1][ind] ){
                swap(ten[i+1][ind],ten[i][ind],temp);
                swap(ten_index[i+1][ind][0],ten_index[i][ind][0],temp_index);
                swap(ten_index[i+1][ind][1],ten_index[i][ind][1],temp_index);
                swap(ten_index[i+1][ind][2],ten_index[i][ind][2],temp_index);
            } else {
                return;
            }
        }

    } else {

        for(i=0;i<mm-1;i++){
            if( ten[i][ind] < ten[i+1][ind] ) {
                swap(ten[i+1][ind],ten[i][ind],temp);
                swap(ten_index[i+1][ind][0],ten_index[i][ind][0],temp_index);
                swap(ten_index[i+1][ind][1],ten_index[i][ind][1],temp_index);
                swap(ten_index[i+1][ind][2],ten_index[i][ind][2],temp_index);

            } else {
                return;
            }
        }

    }

}


/**
 * Residual r = v - A u
 */
void resid(HitTile_double m_u, HitTile_double m_v, HitTile_double m_r, int k){

	printfRootDebug(" = RESID =, level %d \n",k);

	/* Note: I suppose that all the HitTile have the same shape */
	int b1 = hit_tileArray2Tile(m_u,0,hit_shapeSig(block[k],0).begin);
	int b2 = hit_tileArray2Tile(m_u,1,hit_shapeSig(block[k],1).begin);
	int b3 = hit_tileArray2Tile(m_u,2,hit_shapeSig(block[k],2).begin);

    int n1 = hit_sigCard(hit_shapeSig(block[k],0));
    int n2 = hit_sigCard(hit_shapeSig(block[k],1));
    int n3 = hit_sigCard(hit_shapeSig(block[k],2));

    int e1 = b1 + n1;
    int e2 = b2 + n2;
    int e3 = b3 + n3;

#undef u
#undef v
#undef r
#define u(a,b,c) hit_tileElemAtNoStride3(m_u,a,b,c)
#define v(a,b,c) hit_tileElemAtNoStride3(m_v,a,b,c)
#define r(a,b,c) hit_tileElemAtNoStride3(m_r,a,b,c)

#ifdef OPT_RESID_BUFFER


    double temp1[n3+2];
    double temp2[n3+2];


    int i1,i2,i3;

    for(i1=b1;i1<e1;i1++){
        for(i2=b2;i2<e2;i2++){
            for(i3=b3-1;i3<e3+1;i3++){
                temp1[i3] = u(i1-1,  i2,  i3) + u(i1+1,  i2,  i3)
                          + u(  i1,i2-1,  i3) + u(  i1,i2+1,  i3);
                temp2[i3] = u(i1-1,i2-1,  i3) + u(i1+1,i2-1,  i3)
                          + u(i1-1,i2+1,  i3) + u(i1+1,i2+1,  i3);
            }
            for(i3=b3;i3<e3;i3++){

                r(i1,i2,i3) = v(i1,i2,i3) - (
                      operatorA[0] * u(i1,i2,i3)
#ifndef OPT_OPERATOR_A
                    + operatorA[1] * (temp1[i3] + u(i1,i2,i3-1) + u(i1,i2,i3+1))
#endif
                    + operatorA[2] * (temp2[i3] + temp1[i3-1] + temp1[i3+1])
                    + operatorA[3] * (temp2[i3-1] + temp2[i3+1])
                    );

            }
        }
    }

#else

    int i1,i2,i3;

    for(i1=1;i1<e1;i1++){
        for(i2=1;i2<e2;i2++){
            for(i3=1;i3<e3;i3++){

                r(i1,i2,i3) = v(i1,i2,i3) - (
                    + operatorA[0] * u(i1,i2,i3)
#ifndef OPT_OPERATOR_A
                    + operatorA[1] * ( u(i1-1,  i2,  i3) + u(i1+1,  i2,  i3)
                                     + u(  i1,i2-1,  i3) + u(  i1,i2+1,  i3)
                                     + u(  i1,  i2,i3-1) + u(  i1,  i2,i3+1))
#endif
                    + operatorA[2] * ( u(i1-1,i2-1,  i3) + u(i1-1,i2+1,  i3)
                                     + u(i1-1,  i2,i3-1) + u(i1-1,  i2,i3+1)
                                     + u(i1+1,i2-1,  i3) + u(i1+1,i2+1,  i3)
                                     + u(i1+1,  i2,i3-1) + u(i1+1,  i2,i3+1)
                                     + u(  i1,i2-1,i3-1) + u(  i1,i2-1,i3+1)
                                     + u(  i1,i2+1,i3-1) + u(  i1,i2+1,i3+1))

                    + operatorA[3] * ( u(i1-1,i2-1,i3-1) + u(i1-1,i2-1,i3+1)
                                     + u(i1-1,i2+1,i3-1) + u(i1-1,i2+1,i3+1)
                                     + u(i1+1,i2-1,i3-1) + u(i1+1,i2-1,i3+1)
                                     + u(i1+1,i2+1,i3-1) + u(i1+1,i2+1,i3+1))
                    );
            }
        }
    }
#endif


    hit_patternDo( pattern_r[k]);
}





void apply_correction(HitTile_double u, HitTile_double r, int k){

	printfRootDebug(" = APPLY CORRECTION =, level %d \n",k);


#ifdef OPT_ZU

#define zu u

	HitTile_double r_1 = block_r[k - 1];
	HitTile_double z_1 = block_z[k - 1];

	// Same V-cycle till level k-1
	rprj3(r,r_1,k);


	v_cycle(r_1,z_1,k-1);

	// Not call zero at level k (zu = z_k+u)
	interp(z_1,zu,k-1);

	// resid and psinv as normal
	resid(zu,block_v,r,k);
	psinv(zu,r,k);

#else

	HitTile_double z = block_z[k];

	// Apply the V-cycle to obtain z
	v_cycle(r,z,k);

	// Add the correction z to u
	add(u,z);

#endif

}


/*
 * x += t
 */
void add(HitTile_double m_x, HitTile_double m_y){

#undef x
#undef y
#define x(a,b,c) hit_tileElemAtNoStride3(m_x,a,b,c)
#define y(a,b,c) hit_tileElemAtNoStride3(m_y,a,b,c)

    int i1,i2,i3;

    for(i1=0;i1<hit_tileDimCard(m_x,0);i1++)
    	 for(i2=0;i2<hit_tileDimCard(m_x,1);i2++)
    		 for(i3=0;i3<hit_tileDimCard(m_x,2);i3++)
    			 x(i1,i2,i3) += y(i1,i2,i3);

}



void v_cycle(HitTile_double r, HitTile_double z, int k){

	HitTile_double r_1 = block_r[k-1];
	HitTile_double z_1 = block_z[k-1];

    if(k > 0){

        rprj3(r,r_1,k);


        v_cycle(r_1,z_1,k-1);

        zero3(z);
        interp(z_1,z,k-1);

    	resid(z,r,r,k);

        psinv(z,r,k);

    } else {

        // z_1 =  S r_1
        zero3(z);
        psinv(z,r,0);
    }
}

/* r_k-1 = P r_k */
void rprj3(HitTile_double m_ro, HitTile_double m_r, int k){

	printfRootDebug(" = RPRJ3 =, level %d -> %d \n",k,k-1);

	//int b1o = hit_tileArray2Tile(m_ro,0,hit_shapeSig(block[k],0).begin);
	//int b2o = hit_tileArray2Tile(m_ro,1,hit_shapeSig(block[k],1).begin);
	//int b3o = hit_tileArray2Tile(m_ro,2,hit_shapeSig(block[k],2).begin);

    int n1o = hit_sigCard(hit_shapeSig(block[k],0));
    int n2o = hit_sigCard(hit_shapeSig(block[k],1));
    int n3o = hit_sigCard(hit_shapeSig(block[k],2));

    //int e1o = b1o + n1o;
    //int e2o = b2o + n2o;
    //int e3o = b3o + n3o;

    k--;

	int b1 = hit_tileArray2Tile(m_r,0,hit_shapeSig(block[k],0).begin);
	int b2 = hit_tileArray2Tile(m_r,1,hit_shapeSig(block[k],1).begin);
	int b3 = hit_tileArray2Tile(m_r,2,hit_shapeSig(block[k],2).begin);

    int n1 = hit_sigCard(hit_shapeSig(block[k],0));
    int n2 = hit_sigCard(hit_shapeSig(block[k],1));
    int n3 = hit_sigCard(hit_shapeSig(block[k],2));


    int e1 = b1 + n1;
    int e2 = b2 + n2;
    int e3 = b3 + n3;

#undef ro
#undef r
#define ro(a,b,c) hit_tileElemAtNoStride3(m_ro,a,b,c)
#define r(a,b,c) hit_tileElemAtNoStride3(m_r,a,b,c)

    /* Indices */
	int i1o,i2o,i3o;
	int i1,i2,i3;

	/* Offsets */
	int d1,d2,d3;

	if(n1o==1){
		d1 = 1;
	} else {
		d1 = 0;
	}
	if(n2o==1){
		d2 = 1;
	} else {
		d2 = 0;
	}
	if(n3o==1){
		d3 = 1;
	} else {
		d3 = 0;
	}




#ifdef OPT_RPRJ3_BUFFER


	double temp_x1[n3o+2];
	double temp_y1[n3o+2];

	double x2, y2;

	for(i1=b1;i1<e1;i1++){
		i1o = i1 * 2 - d1;
		for(i2=b2;i2<e2;i2++){
			i2o = i2 * 2 - d2;

			for(i3=b3;i3<e3+1;i3++){
				i3o = i3 * 2 - d3;
				temp_x1[i3o-1] = ro(  i1o,i2o-1,i3o-1) + ro(  i1o,i2o+1,i3o-1)
						  + ro(i1o-1,  i2o,i3o-1) + ro(i1o+1,  i2o,i3o-1);
				temp_y1[i3o-1] = ro(i1o-1,i2o-1,i3o-1) + ro(i1o+1,i2o-1,i3o-1)
						  + ro(i1o-1,i2o+1,i3o-1) + ro(i1o+1,i2o+1,i3o-1);
			}
			for(i3=b3;i3<e3;i3++){
				i3o = i3 * 2 - d3;

				y2 = ro(i1o-1,i2o-1,  i3o) + ro(i1o+1,i2o-1,  i3o)
				   + ro(i1o-1,i2o+1,  i3o) + ro(i1o+1,i2o+1,  i3o);
				x2 = ro(  i1o,i2o-1,  i3o) + ro(i1o,  i2o+1,  i3o)
				   + ro(i1o-1,  i2o,  i3o) + ro(i1o+1,  i2o,  i3o);

				r(i1,i2,i3) =
					+ operatorP[0] * ro(i1o,i2o,i3o)
					+ operatorP[1] * (ro(i1o,i2o,i3o-1) + ro(i1o,i2o,i3o+1) +x2)
					+ operatorP[2] * ( temp_x1[i3o-1] + temp_x1[i3o+1] + y2)
					+ operatorP[3] * ( temp_y1[i3o-1] + temp_y1[i3o+1] );
			}
		}
	}


#else


for(i1=b1;i1<e1;i1++){
    i1o = i1 * 2 - d1;

    for(i2=b2;i2<e2;i2++){
        i2o = i2 * 2 - d2;

        for(i3=b3;i3<e3;i3++){
            i3o = i3 * 2 - d3;

            r(i1,i2,i3) = (
                + operatorP[0] * ro(i1o,i2o,i3o)
                + operatorP[1] *
                    ( ro(i1o-1,  i2o,  i3o) + ro(i1o+1,  i2o,  i3o)
                    + ro(  i1o,i2o-1,  i3o) + ro(  i1o,i2o+1,  i3o)
                    + ro(  i1o,  i2o,i3o-1) + ro(  i1o,  i2o,i3o+1))
                + operatorP[2] *
                    ( ro(i1o-1,i2o-1,  i3o) + ro(i1o-1,i2o+1,  i3o)
                    + ro(i1o-1,  i2o,i3o-1) + ro(i1o-1,  i2o,i3o+1)
                    + ro(i1o+1,i2o-1,  i3o) + ro(i1o+1,i2o+1,  i3o)
                    + ro(i1o+1,  i2o,i3o-1) + ro(i1o+1,  i2o,i3o+1)
                    + ro(  i1o,i2o-1,i3o-1) + ro(  i1o,i2o-1,i3o+1)
                    + ro(  i1o,i2o+1,i3o-1) + ro(  i1o,i2o+1,i3o+1))
                + operatorP[3] *
                    ( ro(i1o-1,i2o-1,i3o-1) + ro(i1o-1,i2o-1,i3o+1)
                    + ro(i1o-1,i2o+1,i3o-1) + ro(i1o-1,i2o+1,i3o+1)
                    + ro(i1o+1,i2o-1,i3o-1) + ro(i1o+1,i2o-1,i3o+1)
                    + ro(i1o+1,i2o+1,i3o-1) + ro(i1o+1,i2o+1,i3o+1))
                );

        }
    }
}



#endif


	hit_patternDo(pattern_r[k]);


}



void interp(HitTile_double m_zo, HitTile_double m_z, int k){


	printfRootDebug(" = INTERP =, level %d -> %d \n",k,k+1);

	int b1o = hit_tileArray2Tile(m_zo,0,hit_shapeSig(block[k],0).begin);
	int b2o = hit_tileArray2Tile(m_zo,1,hit_shapeSig(block[k],1).begin);
	int b3o = hit_tileArray2Tile(m_zo,2,hit_shapeSig(block[k],2).begin);

    int n1o = hit_sigCard(hit_shapeSig(block[k],0));
    int n2o = hit_sigCard(hit_shapeSig(block[k],1));
    int n3o = hit_sigCard(hit_shapeSig(block[k],2));

    int e1o = b1o + n1o;
    int e2o = b2o + n2o;
    int e3o = b3o + n3o;

    k++;

	//int b1 = hit_tileArray2Tile(m_z,0,hit_shapeSig(block[k],0).begin);
	//int b2 = hit_tileArray2Tile(m_z,1,hit_shapeSig(block[k],1).begin);
	//int b3 = hit_tileArray2Tile(m_z,2,hit_shapeSig(block[k],2).begin);

    int n1 = hit_sigCard(hit_shapeSig(block[k],0));
    int n2 = hit_sigCard(hit_shapeSig(block[k],1));
    int n3 = hit_sigCard(hit_shapeSig(block[k],2));

    //int e1 = b1 + n1;
    //int e2 = b2 + n2;
    //int e3 = b3 + n3;



#   undef zo
#   undef z
#define zo(a,b,c) hit_tileElemAtNoStride3(m_zo,a,b,c)
#define z(a,b,c) hit_tileElemAtNoStride3(m_z,a,b,c)

    int i1o,i2o,i3o;
    int i1,i2,i3;



// Inactive if
if( layout[k-1].active ){


#ifdef OPT_INTERP_BUFFER


    double temp1[n3o+2];
    double temp2[n3o+2];
    double temp3[n3o+2];


    if( n1o != 1 && n2o != 1 && n3o != 1 ){

        for(i1o=b1o-1;i1o<e1o;i1o++){
            i1 = i1o * 2;

            for(i2o=b2o-1;i2o<e2o;i2o++){
                i2 = i2o * 2;

                for(i3o=b3o-1;i3o<e3o+1;i3o++){
                    temp1[i3o] = zo(  i1o,i2o+1,  i3o) + zo(  i1o,  i2o,  i3o);
                    temp2[i3o] = zo(i1o+1,  i2o,  i3o) + zo(  i1o,  i2o,  i3o);
                    temp3[i3o] = zo(i1o+1,i2o+1,  i3o) + zo(i1o+1,  i2o,  i3o)
                        + temp1[i3o];
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1,i2,i3) += operatorQ[0] * zo(  i1o,  i2o,  i3o);
                    z(i1,i2,i3+1) += operatorQ[1] *
                        (zo(  i1o,  i2o,i3o+1) + zo(  i1o,  i2o,  i3o));
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1,i2+1,i3) += operatorQ[1] * temp1[i3o];
                    z(i1,i2+1,i3+1) += operatorQ[2] *
                        ( temp1[i3o] + temp1[i3o+1 ] );
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1+1,i2,i3) += operatorQ[1] * temp2[i3o];
                    z(i1+1,i2,i3+1) += operatorQ[2] *
                        ( temp2[i3o] + temp2[i3o+1] );
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1+1,i2+1,i3) += operatorQ[2] * temp3[i3o];
                    z(i1+1,i2+1,i3+1) += operatorQ[3] *
                        ( temp3[i3o] + temp3[i3o+1]) ;
                }
            }
        }
#else



   if( n1o != 1 && n2o != 1 && n3o != 1 ){

        for(i1o=b1o-1;i1o<e1o;i1o++){
            i1 = i1o * 2;

            for(i2o=b2o-1;i2o<e2o;i2o++){
                i2 = i2o * 2;

   
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1,i2,i3) += operatorQ[0] * zo(  i1o,  i2o,  i3o);
                    z(i1,i2,i3+1) += operatorQ[1] *
                        (zo(  i1o,  i2o,i3o+1) + zo(  i1o,  i2o,  i3o));
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1,i2+1,i3) += operatorQ[1] * 
                    	( zo(  i1o,i2o+1,  i3o)   + zo(  i1o,  i2o,  i3o));
                    z(i1,i2+1,i3+1) += operatorQ[2] *
                        ( zo(  i1o,i2o+1,  i3o)   + zo(  i1o,  i2o,  i3o)
                        + zo(  i1o,i2o+1,  i3o+1) + zo(  i1o,  i2o,  i3o+1) );
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1+1,i2,i3) += operatorQ[1] * 
                    	( zo(i1o+1,  i2o,  i3o)   + zo(  i1o,  i2o,  i3o));
                    z(i1+1,i2,i3+1) += operatorQ[2] *
                        ( zo(i1o+1,  i2o,  i3o)   + zo(  i1o,  i2o,  i3o)
                        + zo(i1o+1,  i2o,  i3o+1) + zo(  i1o,  i2o,  i3o+1) );
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2;
                    z(i1+1,i2+1,i3) += operatorQ[2] * 
                    	( zo(i1o+1,i2o+1,  i3o)   + zo(i1o+1,  i2o,  i3o)
                    	+ zo(  i1o,i2o+1,  i3o)   + zo(  i1o,  i2o,  i3o));
                    z(i1+1,i2+1,i3+1) += operatorQ[3] *
                        ( zo(i1o+1,i2o+1,  i3o)   + zo(i1o+1,  i2o,  i3o)
                        + zo(  i1o,i2o+1,  i3o)   + zo(  i1o,  i2o,  i3o)
						+ zo(i1o+1,i2o+1,  i3o+1) + zo(i1o+1,  i2o,  i3o+1)
						+ zo(  i1o,i2o+1,  i3o+1) + zo(  i1o,  i2o,  i3o+1)) ;
                }
            }
        }


#endif


    } else {

        // dx are the offset for the first element
        // tx for the second one
        int d1,d2,d3;
        int t1,t2,t3;

        if(n1==1){
            d1 = 1;
            t1 = 1;
        } else {
            d1 = 0;
            t1 = 0;
        }
        if(n2==1){
            d2 = 1;
            t2 = 1;
        } else {
            d2 = 0;
            t2 = 0;
        }
        if(n3==1){
            d3 = 1;
            t3 = 1;
        } else {
            d3 = 0;
            t3 = 0;
        }

        for(i1o=b1o-1+t1;i1o<e1o;i1o++){
            i1 = i1o * 2 - d1;

            for(i2o=b2o-1+t2;i2o<e2o;i2o++){
                i2 = i2o * 2 - d2;

                for(i3o=b3o-1+t3;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1,i2,i3) += operatorQ[0] * zo(  i1o  ,i2o,  i3o);
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1,i2,i3+1) += operatorQ[1] *
                        (zo(  i1o,  i2o,i3o+1) + zo(  i1o,  i2o,  i3o));
                }
            }
            for(i2o=b2o-1;i2o<e2o;i2o++){
                i2 = i2o * 2 - d2;

                for(i3o=b3o-1+t3;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1,i2+1,i3) += operatorQ[1] *
                        ( zo(  i1o,i2o+1,  i3o) + zo(  i1o,  i2o,  i3o));
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1,i2+1,i3+1) += operatorQ[2] *
                        ( zo(  i1o,i2o+1,i3o+1) + zo(  i1o,  i2o,i3o+1)
                        + zo(  i1o,i2o+1,  i3o) + zo(  i1o,  i2o,  i3o));
                }
            }
        }
        for(i1o=b1o-1;i1o<e1o;i1o++){
            i1 = i1o * 2 - d1;

            for(i2o=b2o-1+t2;i2o<e2o;i2o++){
                i2 = i2o * 2 - d2;

                for(i3o=b3o-1+t3;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1+1,i2,i3) += operatorQ[1] *
                        (zo(i1o+1,  i2o,  i3o) + zo(  i1o,  i2o,  i3o));
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1+1,i2,i3+1) += operatorQ[2] *
                        ( zo(i1o+1,  i2o,i3o+1) + zo(i1o+1,  i2o,  i3o)
                        + zo(  i1o,  i2o,i3o+1) + zo(  i1o,  i2o,  i3o));
                }
            }
            for(i2o=b2o-1;i2o<e2o;i2o++){
                i2 = i2o * 2 - d2;
                for(i3o=b3o-1+t3;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1+1,i2+1,i3) += operatorQ[2] *
                        ( zo(i1o+1,i2o+1,  i3o) + zo(i1o+1,  i2o,  i3o)
                        + zo(  i1o,i2o+1,  i3o) + zo(  i1o,  i2o,  i3o));
                }
                for(i3o=b3o-1;i3o<e3o;i3o++){
                    i3 = i3o * 2 - d3;
                    z(i1+1,i2+1,i3+1) += operatorQ[3] *
                        ( zo(i1o+1,i2o+1,i3o+1) + zo(i1o+1,  i2o,i3o+1)
                        + zo(i1o+1,i2o+1,  i3o) + zo(i1o+1,  i2o,  i3o)
                        + zo(  i1o,i2o+1,i3o+1) + zo(  i1o,  i2o,i3o+1)
                        + zo(  i1o,i2o+1,  i3o) + zo(  i1o,  i2o,  i3o));
                }
            }
        }
    }


// Inactive if
}

    hit_patternDo( pattern_z_ex[k]);
}



void psinv(HitTile_double m_z, HitTile_double m_r, int k){

	printfRootDebug(" = PSINV =, level %d\n",k);

	int b1 = hit_tileArray2Tile(m_z,0,hit_shapeSig(block[k],0).begin);
	int b2 = hit_tileArray2Tile(m_z,1,hit_shapeSig(block[k],1).begin);
	int b3 = hit_tileArray2Tile(m_z,2,hit_shapeSig(block[k],2).begin);

    int n1 = hit_sigCard(hit_shapeSig(block[k],0));
    int n2 = hit_sigCard(hit_shapeSig(block[k],1));
    int n3 = hit_sigCard(hit_shapeSig(block[k],2));

    int e1 = b1 + n1;
    int e2 = b2 + n2;
    int e3 = b3 + n3;

#undef z
#undef r
#define z(a,b,c) hit_tileElemAtNoStride3(m_z,a,b,c)
#define r(a,b,c) hit_tileElemAtNoStride3(m_r,a,b,c)

    int i1,i2,i3;

    #ifdef OPT_PSINV_BUFFER


    double temp1[n3+2];
    double temp2[n3+2];

        for(i1=b1;i1<e1;i1++){
            for(i2=b2;i2<e2;i2++){
                for(i3=b3-1;i3<e3+1;i3++){
                    temp1[i3] = r(i1-1,  i2,  i3) + r(i1+1,  i2,  i3)
                              + r(  i1,i2-1,  i3) + r(  i1,i2+1,  i3);
                    temp2[i3] = r(i1-1,i2-1,  i3) + r(i1+1,i2-1,  i3)
                              + r(i1-1,i2+1,  i3) + r(i1+1,i2+1,  i3);
                }
                for(i3=b3;i3<e3;i3++){

                    z(i1,i2,i3) = z(i1,i2,i3) + (
                          operatorS[0] * r(i1,i2,i3)
                        + operatorS[1] * (temp1[i3] + r(i1,i2,i3-1) + r(i1,i2,i3+1))
                        + operatorS[2] * (temp2[i3] + temp1[i3-1] + temp1[i3+1])
    #ifndef OPT_OPERATOR_S
                        + operatorS[3] * (temp2[i3-1] + temp2[i3+1])
    #endif
                        );
                }
            }
        }

    #else

        for(i1=b1;i1<e1;i1++){
            for(i2=b2;i2<e2;i2++){
                for(i3=b3;i3<e3;i3++){

                    z(i1,i2,i3) = z(i1,i2,i3) + (
                        + operatorS[0] * r(i1,i2,i3)
                        + operatorS[1] * ( r(i1-1,  i2,  i3) + r(i1+1,  i2,  i3)
                                         + r(  i1,i2-1,  i3) + r(  i1,i2+1,  i3)
                                         + r(  i1,  i2,i3-1) + r(  i1,  i2,i3+1))
                        + operatorS[2] * ( r(i1-1,i2-1,  i3) + r(i1-1,i2+1,  i3)
                                         + r(i1-1,  i2,i3-1) + r(i1-1,  i2,i3+1)
                                         + r(i1+1,i2-1,  i3) + r(i1+1,i2+1,  i3)
                                         + r(i1+1,  i2,i3-1) + r(i1+1,  i2,i3+1)
                                         + r(  i1,i2-1,i3-1) + r(  i1,i2-1,i3+1)
                                         + r(  i1,i2+1,i3-1) + r(  i1,i2+1,i3+1))
    #ifndef OP_OPERATOR_S
                        + operatorS[3] * ( r(i1-1,i2-1,i3-1) + r(i1-1,i2-1,i3+1)
                                         + r(i1-1,i2+1,i3-1) + r(i1-1,i2+1,i3+1)
                                         + r(i1+1,i2-1,i3-1) + r(i1+1,i2-1,i3+1)
                                         + r(i1+1,i2+1,i3-1) + r(i1+1,i2+1,i3+1))
    #endif
                        );
                }
            }
        }

    #endif

     hit_patternDo( pattern_z[k]);
}



double norm2u3(HitTile_double m_r,int k){

	printfRootDebug(" = NORM2U3 =, level %d\n",k);

	int b1 = hit_tileArray2Tile(m_r,0,hit_shapeSig(block[k],0).begin);
	int b2 = hit_tileArray2Tile(m_r,1,hit_shapeSig(block[k],1).begin);
	int b3 = hit_tileArray2Tile(m_r,2,hit_shapeSig(block[k],2).begin);

    int n1 = hit_sigCard(hit_shapeSig(block[k],0));
    int n2 = hit_sigCard(hit_shapeSig(block[k],1));
    int n3 = hit_sigCard(hit_shapeSig(block[k],2));

    int e1 = b1 + n1;
    int e2 = b2 + n2;
    int e3 = b3 + n3;

#undef r
#define r(a,b,c) hit_tileElemAtNoStride3(m_r,a,b,c)

    HitTile_double sum, acum_sum;
    double res = 0.0;
  	hit_tileDomainAlloc(&sum, double, 1,1);
  	hit_tileDomainAlloc(&acum_sum, double, 1,1);

    int i3, i2, i1;

    hit_tileElemAt(sum,1,0) = 0;
    for(i1=b1;i1<e1;i1++){
        for(i2=b2;i2<e2;i2++){
            for(i3=b3;i3<e3;i3++){
            	hit_tileElemAt(sum,1,0) += pow(r(i1,i2,i3),2);
            }
        }
    }

    HitRanks root = {{0,0,0,-1}};
  	HitCom com_sum = hit_comReduce(layout[k_max], root, &sum, &acum_sum, HIT_DOUBLE, op_sum);
  	hit_comDo(&com_sum);
	hit_comFree(com_sum);

  	if( hit_Rank == 0  ){
  		res = hit_tileElemAt(acum_sum,1,0) / (problem_size[0] * problem_size[1] * problem_size[2]);
  		res = sqrt( res );
  	}

	hit_tileFree(sum);
	hit_tileFree(acum_sum);

  	return res;
}

/**
 * Warming up iteration for start up.
 */
void warming_up(){

    zero3(block_u);
    zran3(block_v);
    resid(block_u,block_v,block_r[k_max],k_max);
    apply_correction(block_u,block_r[k_max],k_max);
}


void free_resources(){

	printfRootDebug(" = FREE RESOURCES = \n");

	int k;

	/* Loop through all levels */
	for (k = k_num - 1; k >= 0; k--) {

		/* Free Layout */
		hit_layFree( layout[k] );

		/* Free for the top level: u, v, r and z. */
		if (k == k_max) {

			printfRootDebug(" -> Freeing blocks u, v, r, z\n");

			hit_tileFree(block_u);
			hit_tileFree(block_v);

			hit_tileFree(block_r[k]);
#ifndef OPT_ZU
			hit_tileFree(block_z[k]);
#endif

			/* Allocates for other levels: r and z. */
		} else {

			printfRootDebug(" -> Freeing blocks r, z\n");

			hit_tileFree(block_r[k]);
			hit_tileFree(block_z[k]);
		}

		/* Communication patterns */
		printfRootDebug("* Freeing Comm Pattern for level: %d\n",k);
		if (k == k_max)
			hit_patternFree(&pattern_v);
		hit_patternFree(&pattern_r[k]);
		hit_patternFree(&pattern_z[k]);
		hit_patternFree(&pattern_z_ex[k]);

	}

	/* Free Hit collective functions */
	hit_comOpFree(op_max);
	hit_comOpFree(op_min);
	hit_comOpFree(op_sum);

	/* Free topology */
	hit_topFree( topology );
}


#ifdef DEBUG
void printBlock(HitTile_double matrix, int k, int d){

	int b1 = hit_tileArray2Tile(matrix,0,hit_shapeSig(block[k],0).begin);
	int b2 = hit_tileArray2Tile(matrix,1,hit_shapeSig(block[k],1).begin);
	int b3 = hit_tileArray2Tile(matrix,2,hit_shapeSig(block[k],2).begin);

    int n1 = hit_sigCard(hit_shapeSig(block[k],0));
    int n2 = hit_sigCard(hit_shapeSig(block[k],1));
    int n3 = hit_sigCard(hit_shapeSig(block[k],2));

    int e1 = b1 + n1;
    int e2 = b2 + n2;
    int e3 = b3 + n3;

#undef m
#define m(a,b,c) hit_tileElemAtNoStride3(matrix,a,b,c)

    ALL_SEQUENTIAL_BEGIN(iii)

	if( !(n1==0 || n2 == 0 || n3 == 0)){

		/////////////
		int i3, i2, i1;
		printf("\n(%d) =============== Block ================ (%d,%d,%d)\n",HIT_TOPOLOGY_INFO.selfRank,n1,n2,n3);

		for(i1=b1-d;i1<e1+d;i1++){
			for(i2=b2-d;i2<e2+d;i2++){
				for(i3=b3-d;i3<e3+d;i3++){
					printf("%.2f\t ",m(i1,i2,i3)*100);
				}
				printf("\n");
			}
			printf("=======================================\n");
		}
		/////////////
	}

    ALL_SEQUENTIAL_END(iii)

}


void printfAllInternal( const char* format, ... ) {

	ALL_SEQUENTIAL_BEGIN(i)

	fprintf( stdout, "%2d: ",i);
	va_list args;
	va_start( args, format );
	vfprintf( stdout, format, args );
	va_end( args );

	ALL_SEQUENTIAL_END(i)
 }


void fill3(HitTile_double matrix,int k){

	int b1 = hit_shapeSig(block[k],0).begin;
	int b2 = hit_shapeSig(block[k],1).begin;
	int b3 = hit_shapeSig(block[k],2).begin;


	int e1 = hit_shapeSig(block[k],0).end;
	int e2 = hit_shapeSig(block[k],1).end;
	int e3 = hit_shapeSig(block[k],2).end;


#undef m
#define m(a,b,c) hit_tileElemAtArrayCoords3(matrix,a,b,c)

    int i1,i2,i3;

    for(i1=b1;i1<=e1;i1++)
        for(i2=b2;i2<=e2;i2++)
            for(i3=b3;i3<=e3;i3++)
                m(i1,i2,i3) = (100 * (i1+1) + 10 * (i2+1) + (i3+1)) / 100.0;

}

#endif

