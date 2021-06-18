/**
 * Heat distribution in a graph.
 * Hitmap version of the synthetic benchmark.
 * 
 * @file heat.c
 * @author Javier Fresno
 * @date 18-04-2011
 * @date 05-10-2015
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

#include <hitmap.h>
#include <unistd.h>

/**
 * Enable simulated load
 */
#define SIMULATED_LOAD


/* OUTPUT FUNCIONS */
#define printfRootInternal(...) { if( hit_Rank == 0 ) { printf(__VA_ARGS__); fflush(stdout); }}
#define printfRoot(...) printfRootInternal(__VA_ARGS__)


/* HITMAP DOUBLE TILE */
hit_tileNewType(double);

/**
 * Prints the command line usage.
 * @param Name of the program.
 */
void print_help(char * name);

/**
 * Init the graph with random data.
 * @param graph The graph.
 * @param shape_global Shape of the global graph.
 */
void init_graph(HitTile_double graph, HitShape shape_global);

/**
 * Perform an iteration of the heat distribution equation.
 * @param The local shape of the graph.
 * @param graph The graph.
 * @return The graph after the iteration.
 */
HitTile_double heat_iteration(HitShape shape, HitTile_double graph);

/**
 * Calculate the verification sum.
 * @param shape The local shape of the graph.
 * @param graph The local graph.
 * @return The sum.
 */
double result_check(HitShape shape, HitTile_double graph);


/**
 * Default number of iterations.
 */ 
#define ITERATIONS 100

/**
 * Number of init values.
 */
#define INIT_VALUES 10

/**
 * An auxiliary graph.
 */
HitTile_double graph_aux;

/**
 * Layout object.
 */
HitLayout lay = HIT_LAYOUT_NULL_STATIC;


#ifdef SIMULATED_LOAD
	int simulated = 0;
#endif




/**
 * Main function
 */
int main(int argc, char ** argv){

	// Clock
	HitClock timeclock;

	// Graph file name.
	char * graph_file = NULL;
	
	// Iterations.
	int iterations = ITERATIONS;
	
	// Argument.
	char c;

	// Parse the command-line arguments.
	while ((c = (char) getopt (argc, argv, "i:s:")) != -1)
	switch (c) {
		case 'i':
			sscanf(optarg,"%d",&iterations);
			break;
#ifdef SIMULATED_LOAD
		case 's':
			sscanf(optarg,"%d",&simulated);
			break;
#endif
		default:
			abort ();
			break;
	}

	// Show help if there is no input file.
	if(optind == argc){
		print_help(argv[0]);
	}

	// Get input file.
	graph_file = argv[optind];

	/* Initialize hitmap communication */
	hit_comInit(&argc,&argv);
	
	// Print info.
	printfRoot("Graph: %s, Iter: %d\n",graph_file,iterations);
	printfRoot("Nprocs: %d\n",hit_NProcs);
	
	hit_clockStart( timeclock );

	// Global graph
	HitShape shape_global;
	if(hit_Rank == 0) shape_global = hit_fileHBRead(graph_file);
	printfRoot("Vertices: %d\n",hit_cShapeNvertices(shape_global));

	// Create the topology object.
	HitTopology topo = hit_topology(plug_topPlain);

	// Distribute the graph among the processors.
	lay = hit_layout(plug_layMetis,topo,&shape_global);

	// Get the shapes for the local and local extended graphs.
	HitShape shape = hit_layShape(lay);
	HitShape ext_shape = hit_cShapeExpand(shape,shape_global,1);

	// Allocate memory for the graphs.
	HitTile_double graph;
	hit_gcTileDomainShapeAlloc(&graph, double, ext_shape, HIT_VERTICES);
	hit_gcTileDomainShapeAlloc(&graph_aux, double, ext_shape, HIT_VERTICES);

	// Init the local graph.
	init_graph(graph, shape_global);

	// Create the communicator and send the initial values of the neighbor vertices.
	HitCom com = hit_comSparseUpdate(lay, &graph, HIT_DOUBLE);
	hit_comDo(&com);

	hit_clockStop( timeclock );
	printfRoot("Time init: %f\n",hit_clockGetSeconds(timeclock));
	hit_clockStart( timeclock );

	int i;
	// Perform the equation.
	for(i=0; i<iterations; i++){

		// Update the graph.
		graph = heat_iteration(shape,graph);
		// Update the communication object with the current graph.
		hit_comUpdateOriginData(&com,&graph);
		// Communication.
		hit_comDo(&com);

	}


	hit_clockStop( timeclock );


	double result = result_check(shape,graph);

	printfRoot("Result: %f\n",result);
	printfRoot("Time: %f\n",hit_clockGetSeconds(timeclock));

	// Free all the used resources.
	hit_comFree(com);
	hit_tileFree(graph);
	hit_tileFree(graph_aux);
	hit_layFree(lay);
	hit_topFree(topo);
	hit_shapeFree(shape_global);

	hit_comFinalize();
	return 0;

}


void print_help(char * name){
	printf("%s [-i ITERATIONS] FILE \n",name);

	printf("  -i ITERATIONS   number of iterations (default 100)\n");
	printf("  FILE            input graph file\n");
 
	exit(0);
}


void init_graph(HitTile_double graph, HitShape shape_global){

	srandom(1);

	// Set all the vector to 0.
	int i;
	for(i=0;i<hit_cShapeNvertices(hit_tileShape(graph));i++)
		hit_gcTileVertexAt(graph,i) = 0.0;


	// If there are a few nodes, set only the first one.
	if( hit_cShapeNvertices(shape_global) < (2*INIT_VALUES)){
		int local = hit_cShapeVertexToLocal(hit_tileShape(graph),0);
		if(local != -1) hit_gcTileVertexAt(graph,local) = 100.0;
		return;
	}

	// Init the nodes.
	for(i=0;i<INIT_VALUES;i++){

		int first = (i * hit_cShapeNvertices(shape_global)) / INIT_VALUES;
		int last = ((i+1) * hit_cShapeNvertices(shape_global)) / INIT_VALUES;

		int mod = last - first;

		int in = first + (int)random() % mod;

		int local = hit_cShapeVertexToLocal(hit_tileShape(graph),in);
		if(local != -1) hit_gcTileVertexAt(graph,local) = 100.0;

	}

}




HitTile_double heat_iteration(HitShape shape, HitTile_double graph){

	int vertex;
	HitShape ext_shape = hit_tileShape(graph);

	// Iterate trough all the vertices.
	hit_cShapeVertexIterator(vertex,shape) {

		int edge;

		// Set new value to 0.
		double value = 0;

		hit_cShapeEdgeIterator(edge,ext_shape,vertex){

			// Get the neighbor.
			int neighbor = hit_cShapeEdgeTarget(ext_shape,edge);

			// Add its contribution.
			value +=  hit_gcTileVertexAt(graph,neighbor);

		}


#ifdef SIMULATED_LOAD
		int i;
		for(i=0;i<simulated;i++){
			value = sin(value+1);
		}
#endif

		int nedge = hit_cShapeNEdgesFromVertex(ext_shape,vertex);

		// Update the value of the vertex.
		hit_gcTileVertexAt(graph_aux,vertex) =  (value / nedge);
	}


	HitTile_double aux = graph_aux;
	graph_aux = graph;
	return aux;

}


double result_check(HitShape shape, HitTile_double graph){

	HitTile_double sum, sum_all;

	hit_tileDomainAlloc(&sum, double, 1,1);
	hit_tileDomainAlloc(&sum_all, double, 1,1);

	hit_tileElemAt(sum,1,0) = 0;

	int i;
	for(i=0; i< hit_cShapeNvertices(shape); i++){
		hit_tileElemAt(sum,1,0) += hit_gcTileVertexAt(graph,i);
	}

	HitOp op_sum;
	hit_comOp(hit_comOpSumDouble,op_sum);
	HitCom com_sum = hit_comReduce(lay, HIT_RANKS_NULL, &sum, &sum_all, HIT_DOUBLE, op_sum);

	hit_comDo(&com_sum);

	double res = hit_tileElemAt(sum_all,1,0);

	hit_comFree(com_sum);
	hit_tileFree(sum);
	hit_tileFree(sum_all);

	return res;
}

