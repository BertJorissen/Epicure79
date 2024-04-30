/**
* Implememntation of functionalites to perform automatic load balancing between processes.
* Redistribute work load and data from the less powerful processes to
* the most powerful ones.
*
* @file hit_alb.c
* @version 1.0
* @author Maria Sanchez Giron
* @date Sep 2020
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


#include "hit_alb.h"
#include <hit_tile.h>
#include <hit_tileP.h>
#include <hit_layout.h>
#include <hit_view.h>
#include <hit_shape.h>
#include <hit_mpi.h>
#include <hitmap.h>
#include <hitmap2.h>

#ifdef DEBUG
/**
 * Pretty print an array of floats.
 * 
 * @param a An array of floats.
 * @param size_a The number of elements in @code a.
 */
void printArrayFloat(float a[], int size_a){
    for(int i=0; i<size_a; i++){
        printf("%.5f ", a[i]);
    }
    printf("\n");

}
#endif //DEBUG

hit_tileNewType(double);


/**
 * Reset the average structure to its initial state.
 */ 
void hit_avgResetData(HitAvg *avg){
    for(int i=0; i<avg->windowSize; i++) avg->data[i] = -1;
    avg->full = HITAVG_NOT_FULL;
    avg->pointer = 0;
    avg->first = 0;
    avg->previousAvg = 0;
}

/**
 * Compute Simple Moving Average.
 */ 
double hit_avgGetSMA(HitAvg avg){
    if (avg.full == HITAVG_NOT_FULL) return HITAVG_NOT_FULL;

    double sumData = 0.0;

    for(int i=0; i<avg.windowSize; i++){
        sumData += avg.data[i];
    }

    return sumData/avg.windowSize;
}

/**
 * Compute Weighted Moving Average.
 */
double hit_avgGetWMA(HitAvg avg){
    if (avg.full == HITAVG_NOT_FULL) return HITAVG_NOT_FULL;

    double sumData = 0.0;
    double sumWeights = 0.0;

    for(int i=0; i<avg.windowSize; i++){
        sumData += avg.data[(i+avg.first)%avg.windowSize]*avg.weights[i];
        sumWeights += avg.weights[i];
    }

    return sumData/sumWeights;
}

/**
 * Update the value of an Exponential Moving Average.
 * If the Avg's data array is not full it does nothing.
 */
void hit_avgUpdateEMA(HitAvg *avg, double d){
    if (avg->full == HITAVG_NOT_FULL) return;
    if (avg->previousAvg == 0) {
        double sma = hit_avgGetSMA(*avg);
        avg->previousAvg = sma;
    }

    double k = 2.0/(avg->windowSize + 1);
    double ema = d*k + avg->previousAvg*(1-k);
    avg->previousAvg = ema;
}

/**
 * Insert new data in the data array of a HitAvg.
 * The mean is updated with this new value.
 */ 
void hit_avgInsertData(HitAvg *avg, double d){
    avg->data[avg->pointer] = d;
    avg->pointer++;
	if (avg->full == HITAVG_FULL){
		avg->first++;
        if(avg->first == avg->windowSize) avg->first = 0;
	}
    if(avg->pointer == avg->windowSize){
        avg->full = HITAVG_FULL;
        avg->pointer = 0;
    }

    double a;
    switch(avg->mode){
        case HITAVG_SMA:
            a = hit_avgGetSMA(*avg);
            if (a != HITAVG_NOT_FULL) avg->previousAvg = a;
            break;
        case HITAVG_WMA:
            a = hit_avgGetWMA(*avg);
            if (a != HITAVG_NOT_FULL) avg->previousAvg = a;
            break;
        case HITAVG_EMA:
            hit_avgUpdateEMA(avg, d);
            break;
    }
}

/**
 * Get current value of the mean.
 */ 
double hit_avgGetAvg(HitAvg avg){
    if (avg.full == HITAVG_NOT_FULL) return HITAVG_NOT_FULL;
    return avg.previousAvg;
}


/**
 * Print the data array from the least recent element
 * to the last element inserted.
 */ 
void hit_avgPrintData(HitAvg avg){
    for(int i=avg.first; i<avg.windowSize; i++) printf("%.5lf\t", avg.data[i]);
    for(int i=0; i<avg.first; i++) printf("%.5lf\t", avg.data[i]);
    printf("\n");
}


/**
 * Perform an load balancing and redistribution function based on the average
 * times of execution of each process.
 * The redistribution interval has a fixed size.
 * If the average data array is not full the redistribution is not done.
 */ 
void hit_albWeightedRedistributeFixedInterval(HitTile * ttile[], int numTiles, HitLayout * lay, HitAvg * avg,
	HitPattern * patt[], int numPatterns, HitPatternFunction pattf, HitTileInitFunction tilef,
	HitShpView shpview, double time){

	static HitClock callClock = {HIT_CLOCK_STOPPED, -1, 0, 0, 0, 0};
	static HitLayout layoutComm;	// Communications layout. All processes active
	static HitTile_double allTimes;
	static MPI_Request req;
	HitTile_double times;

	if (callClock.seconds == -1){	//First call to the function, initialize layoutComm
		layoutComm = hit_layout_freeTopo( plug_layCopy, hit_topology(plug_topPlain), hit_shapeStd(1, 1) );
	}
	else{
        #ifdef DEBUG
		    hit_clockStop(callClock);
		    if (hit_layImActive(*lay)) printf("Process[%d] Time between calls: %lf\n", hit_Rank, callClock.seconds);
        #endif
	}

	if (! hit_layImActive(*lay)) time = 0.0;

    #ifdef DEBUG
	    printf("Process[%d] Time parameter: %lf\n", hit_Rank, time);
    #endif

	hit_avgInsertData(avg, time);
	double average = hit_avgGetAvg(*avg);

	if(average != -1){
		if (avg->first == 0 && avg->pointer == 0){	// first time that the data array is full
			double zero = 0;
			hit_tileDomainAlloc(&times, double, 1, hit_NProcs);
			hit_tileDomainAlloc(&allTimes, double, 1, hit_NProcs);
			hit_tileFill(&allTimes, &zero);
			double timePerRow;
			if (! hit_layImActive(*lay)) timePerRow = 0.0;
			else timePerRow = average / hit_tileDimCard((*(HitTile *)ttile[0]), 0);
			hit_tileFill(&times, &timePerRow);
			
			HitCom com = hit_comAlltoall(layoutComm, &times, &allTimes, HIT_DOUBLE, 1);
			int ok = MPI_Ialltoall(com.dataSend, com.count, com.typeSend, com.dataRecv, com.count, com.typeSend, com.comm, &req );
			hit_mpiTestError(ok,"Failed ialltoall send");
		} else {
			int ok = MPI_Wait( &req, MPI_STATUS_IGNORE );
			hit_mpiTestError(ok,"Failed ialltoall wait");

			/* COMPUTE NEW WEIGHTS */
			HitWeights weights;
			float normalizedWeights[hit_NProcs];
			double suma = 0;

			for (int k=0; k<hit_NProcs; k++) suma += hit_tileElemAt(allTimes, 1, k);
			for (int k=0; k<hit_NProcs; k++) {
				if (hit_tileElemAt(allTimes, 1, k) == 0.0) normalizedWeights[k] = 0.0;
				else {
					normalizedWeights[k] = (float) (suma / hit_tileElemAt(allTimes, 1, k));
				}
			}
			if(suma == 0.0) normalizedWeights[0] = 1;
			weights = hitWeights( hit_NProcs, normalizedWeights );

            #ifdef DEBUG
                if (hit_layImLeader((layoutComm))){
                    printf("Process[%d] weights: ", hit_Rank);
                    printArrayFloat(normalizedWeights, hit_NProcs);
                    fflush(stdout);
                }
            #endif
			

			/* CREATE NEW LAYOUT */
			HitLayout layout2;
			layout2 = hit_layout_freeTopo(plug_layDimWeighted_Blocks, hit_topology(plug_topPlain), lay->origShape, 0, weights);

			/* CREATE NEW TILES AND PATTERNS */
			for(int t=0; t<numTiles; t++){
				HitTile *tile = (HitTile *)ttile[t];
				HitPattern *pattern = (HitPattern *) patt[t];
				HitShape expandedShape;
				HitTile tile2;
				HitTile *origTile = hit_tileRoot(tile);
				int i;

				/* Copy original tile in new tile */
				tile2 = *tile;

				/* Apply transform to local shape */
				expandedShape = hit_layShape(layout2);

				for(i=0; i<shpview.num_transforms; i++ )	
					expandedShape = hit_shapeTransform(expandedShape, shpview.transforms[i].dimension,
						shpview.transforms[i].action, shpview.transforms[i].offset );

				hit_tileSelect(tile, origTile, expandedShape);
				hit_tileAlloc(tile);
			
				/* Init tile */
				tilef(tile);


				/* REDISTRIBUTE */
				hit_patternDoOnce(hit_patternLayRedistribute(*lay, layout2, &tile2, tile, HIT_DOUBLE));
				
				#ifdef DEBUG
                    int oldcard = hit_tileDimCard(tile2, 0);
                    int newcard = hit_tileDimCard((*tile), 0);
                    printf("Process[%d] array[%d] New Shape: (%d:%d)(%d:%d) - Old Shape: (%d:%d)(%d:%d)\n \
            Cards[%d -> %d], diff = %+d\n", hit_Rank, t,
                        hit_tileDimBegin((*tile), 0),
                        hit_tileDimEnd((*tile), 0),
                        hit_tileDimBegin((*tile), 1),
                        hit_tileDimEnd((*tile), 1),
                        hit_tileDimBegin(tile2, 0),
                        hit_tileDimEnd(tile2, 0),
                        hit_tileDimBegin(tile2, 1),
                        hit_tileDimEnd(tile2, 1),
                        oldcard,
                        newcard,
                        newcard - oldcard
                    );
				#endif

				
				/* Update Pattern */
				if (t<numPatterns){
					HitPattern patt2 = pattf(tile, &layout2);
					hit_patternFree(pattern);
					*pattern = patt2;
				}

				/* Free */
				hit_tileFree(tile2);
				hit_tileFree(*origTile);
			}

			/* Update Layout */
			hit_layFree(*lay);
			*lay = layout2;

			/* Reset average */
			hit_avgResetData(avg);

			/* Free */
			hit_tileFree(times);
			hit_tileFree(allTimes);
		}
	}	

	hit_clockStart(callClock);	
}

/**
 * Perform an load balancing and redistribution function based on the average
 * times of execution of each process.
 * The redistribution interval increases its size with each redistribution.
 * If the average data array is not full the redistribution is not done.
 */ 
void hit_albWeightedRedistributeIncrInterval(HitTile * ttile[], int numTiles, HitLayout * lay, HitAvg * avg,
	HitPattern * patt[], int numPatterns, HitPatternFunction pattf, HitTileInitFunction tilef,
	HitShpView shpview, double time){

	static HitClock callClock = {HIT_CLOCK_STOPPED, -1, 0, 0, 0, 0};
	static HitLayout layoutComm;	// Communications layout. All processes active
	static HitTile_double allTimes;	// Time averages of all processes
	static int redisInterval;		// Interval of redistributions
	static int valuesToRedis;		// Number of averages calculated (to measure iterations left to redistribution)
	static MPI_Request req;
	HitTile_double times;

	if (callClock.seconds == -1){	// First call to the function, do nothing but initialize static variables
		layoutComm = hit_layout_freeTopo( plug_layCopy, hit_topology(plug_topPlain), hit_shapeStd(1, 1) );
		redisInterval = avg->windowSize;
		valuesToRedis = avg->windowSize - 1;
	}
	else{
        #ifdef DEBUG
            hit_clockStop(callClock);
            if (hit_layImActive(*lay)) printf("Process[%d] Time between calls: %lf\n", hit_Rank, callClock.seconds);
        #endif
    }

	if (! hit_layImActive(*lay)) time = 0.0;
	hit_avgInsertData(avg, time);
	double average = hit_avgGetAvg(*avg);

	if(average != -1){
		valuesToRedis++;

		if (valuesToRedis == redisInterval){
			double zero = 0;
			hit_tileDomainAlloc(&times, double, 1, hit_NProcs);
			hit_tileDomainAlloc(&allTimes, double, 1, hit_NProcs);
			hit_tileFill(&allTimes, &zero);

			double timePerRow;
			if (! hit_layImActive(*lay)) timePerRow = 0.0;
			else timePerRow = average / hit_tileDimCard((*(HitTile *)ttile[0]), 0);
			hit_tileFill(&times, &timePerRow);
			
			HitCom com = hit_comAlltoall(layoutComm, &times, &allTimes, HIT_DOUBLE, 1);
			int ok = MPI_Ialltoall(com.dataSend, com.count, com.typeSend, com.dataRecv, com.count, com.typeSend, com.comm, &req );
			hit_mpiTestError(ok,"Failed ialltoall send");
		}

		else if (valuesToRedis == redisInterval + 1){
			int ok = MPI_Wait( &req, MPI_STATUS_IGNORE );
			hit_mpiTestError(ok,"Failed ialltoall wait");

			/* COMPUTE NEW WEIGHTS */
			HitWeights weights;
			float normalizedWeights[hit_NProcs];
			double suma = 0;

			for (int k=0; k<hit_NProcs; k++) suma += hit_tileElemAt(allTimes, 1, k);
			for (int k=0; k<hit_NProcs; k++) {
				if (hit_tileElemAt(allTimes, 1, k) == 0.0) normalizedWeights[k] = 0.0;
				else {
					normalizedWeights[k] = (float) (suma / hit_tileElemAt(allTimes, 1, k));
				}
			}
			if(suma == 0.0) normalizedWeights[0] = 1;
			weights = hitWeights( hit_NProcs, normalizedWeights );

            #ifdef DEBUG
                if (hit_layImLeader((layoutComm))){
                    printf("Process[%d] weights: ", hit_Rank);
                    printArrayFloat(normalizedWeights, hit_NProcs);
                    fflush(stdout);
                }
            #endif

			/* CREATE NEW LAYOUT */
			HitLayout layout2;
			layout2 = hit_layout_freeTopo(plug_layDimWeighted_Blocks, hit_topology(plug_topPlain), lay->origShape, 0, weights);

			/* CREATE NEW TILES AND PATTERNS */
			for(int t=0; t<numTiles; t++){
				HitTile *tile = (HitTile *)ttile[t];
				HitPattern *pattern = (HitPattern *) patt[t];
				HitShape expandedShape;
				HitTile tile2;
				HitTile *origTile = hit_tileRoot(tile);
				int i;

				/* Copy original tile in new tile */
				tile2 = *tile;

				/* Apply transformation to local shape */
				expandedShape = hit_layShape(layout2);

				for(i=0; i<shpview.num_transforms; i++ )	
					expandedShape = hit_shapeTransform(expandedShape, shpview.transforms[i].dimension,
						shpview.transforms[i].action, shpview.transforms[i].offset );

				hit_tileSelect(tile, origTile, expandedShape);
				hit_tileAlloc(tile);
			
				/* Init tile */
				tilef(tile);


				/* REDISTRIBUTE */
				hit_patternDoOnce(hit_patternLayRedistribute(*lay, layout2, &tile2, tile, HIT_DOUBLE));
				
				#ifdef DEBUG
                    int oldcard = hit_tileDimCard(tile2, 0);
                    int newcard = hit_tileDimCard((*tile), 0);
                    printf("Process[%d] array[%d] New Shape: (%d:%d)(%d:%d) - Old Shape: (%d:%d)(%d:%d)\n \
            Cards[%d -> %d], diff = %+d\n", hit_Rank, t,
                        hit_tileDimBegin((*tile), 0),
                        hit_tileDimEnd((*tile), 0),
                        hit_tileDimBegin((*tile), 1),
                        hit_tileDimEnd((*tile), 1),
                        hit_tileDimBegin(tile2, 0),
                        hit_tileDimEnd(tile2, 0),
                        hit_tileDimBegin(tile2, 1),
                        hit_tileDimEnd(tile2, 1),
                        oldcard,
                        newcard,
                        newcard - oldcard
                    );
				#endif

				
				/* Update Pattern */
				if (t<numPatterns){
					HitPattern patt2 = pattf(tile, &layout2);
					hit_patternFree(pattern);
					*pattern = patt2;
				}

				/* Free */
				hit_tileFree(tile2);
				hit_tileFree(*origTile);
			}

			/* Update Layout */
			hit_layFree(*lay);
			*lay = layout2;

			/* Reset average */
            hit_avgResetData(avg);

			/* Free */
			hit_tileFree(times);
			hit_tileFree(allTimes);

			/* Increase interval */
			redisInterval += avg->windowSize;
			valuesToRedis = avg->windowSize - 1;
		}
	}	

	hit_clockStart(callClock);	
}

/**
 * Perform an average-based weighted redistribute function.
 * The redistribution interval increases its size with each redistribution.
 * If the average data array is not full the redistribution is not done.
 */ 
void hit_albWeightedRedistributeIncrIntervalBloq(HitTile * ttile[], int numTiles, HitLayout * lay, HitAvg * avg,
	HitPattern * patt[], int numPatterns, HitPatternFunction pattf, HitTileInitFunction tilef,
	HitShpView shpview, double time){
	
	static HitClock callClock = {HIT_CLOCK_STOPPED, -1, 0, 0, 0, 0};
	static HitLayout layoutComm;	// Communications layout. All processes active
	static int redisInterval;		// Interval of redistributions
	static int valuesToRedis;		// Number of averages calculated (to measure iterations left to redistribution)
	HitTile_double allTimes;	// Time averages of all processes
	HitTile_double times;

	if (callClock.seconds == -1){	// First call to the function, do nothing but initialize static variables
		layoutComm = hit_layout_freeTopo( plug_layCopy, hit_topology(plug_topPlain), hit_shapeStd(1, 1) );
		redisInterval = avg->windowSize;
		valuesToRedis = avg->windowSize - 1;
	}
	else{
        #ifdef DEBUG
            hit_clockStop(callClock);
            if (hit_layImActive(*lay)) printf("Process[%d] Time between calls: %lf\n", hit_Rank, callClock.seconds);
        #endif
    }

	if (! hit_layImActive(*lay)) time = 0.0;

    #ifdef DEBUG
	    printf("Process[%d] Time parameter: %lf\n", hit_Rank, time);
    #endif

	hit_avgInsertData(avg, time);
	double average = hit_avgGetAvg(*avg);

	if(average != -1){
		valuesToRedis++;

		if (valuesToRedis == redisInterval){
			double zero = 0;
			hit_tileDomainAlloc(&times, double, 1, hit_NProcs);
			hit_tileDomainAlloc(&allTimes, double, 1, hit_NProcs);
			hit_tileFill(&allTimes, &zero);

			double timePerRow;
			if (! hit_layImActive(*lay)) timePerRow = 0.0;
			else timePerRow = average / hit_tileDimCard((*(HitTile *)ttile[0]), 0);
			hit_tileFill(&times, &timePerRow);
			
			HitCom com = hit_comAlltoall(layoutComm, &times, &allTimes, HIT_DOUBLE, 1);
			hit_comDo(&com);

			/* COMPUTE NEW WEIGHTS */
			HitWeights weights;
			float normalizedWeights[hit_NProcs];
			double suma = 0;

			for (int k=0; k<hit_NProcs; k++) suma += hit_tileElemAt(allTimes, 1, k);
			for (int k=0; k<hit_NProcs; k++) {
				if (hit_tileElemAt(allTimes, 1, k) == 0.0) normalizedWeights[k] = 0.0;
				else {
					normalizedWeights[k] = (float) (suma / hit_tileElemAt(allTimes, 1, k));
				}
			}
			if(suma == 0.0) normalizedWeights[0] = 1;

			weights = hitWeights( hit_NProcs, normalizedWeights );

            #ifdef DEBUG
                if (hit_layImLeader((layoutComm))){
                    printf("Process[%d] weights: ", hit_Rank);
                    printArrayFloat(normalizedWeights, hit_NProcs);
                    fflush(stdout);
                }
            #endif

			/* CREATE NEW LAYOUT */
			HitLayout layout2;
			layout2 = hit_layout_freeTopo(plug_layDimWeighted_Blocks, hit_topology(plug_topPlain), lay->origShape, 0, weights);

			/* CREATE NEW TILES AND PATTERNS */
			for(int t=0; t<numTiles; t++){
				HitTile *tile = (HitTile *)ttile[t];
				HitPattern *pattern = (HitPattern *) patt[t];
				HitShape expandedShape;
				HitTile tile2;
				HitTile *origTile = hit_tileRoot(tile);
				int i;

				/* Copy original tile in new tile */
				tile2 = *tile;

				/* Apply transformation to local shape */
				expandedShape = hit_layShape(layout2);

				for(i=0; i<shpview.num_transforms; i++ )	
					expandedShape = hit_shapeTransform(expandedShape, shpview.transforms[i].dimension,
						shpview.transforms[i].action, shpview.transforms[i].offset );

				hit_tileSelect(tile, origTile, expandedShape);
				hit_tileAlloc(tile);
			
				/* Init tile */
				tilef(tile);


				/* REDISTRIBUTE */
				hit_patternDoOnce(hit_patternLayRedistribute(*lay, layout2, &tile2, tile, HIT_DOUBLE));
				
				#ifdef DEBUG
                    int oldcard = hit_tileDimCard(tile2, 0);
                    int newcard = hit_tileDimCard((*tile), 0);
                    printf("Process[%d] array[%d] New Shape: (%d:%d)(%d:%d) - Old Shape: (%d:%d)(%d:%d)\n \
            Cards[%d -> %d], diff = %+d\n", hit_Rank, t,
                        hit_tileDimBegin((*tile), 0),
                        hit_tileDimEnd((*tile), 0),
                        hit_tileDimBegin((*tile), 1),
                        hit_tileDimEnd((*tile), 1),
                        hit_tileDimBegin(tile2, 0),
                        hit_tileDimEnd(tile2, 0),
                        hit_tileDimBegin(tile2, 1),
                        hit_tileDimEnd(tile2, 1),
                        oldcard,
                        newcard,
                        newcard - oldcard
                    );
				#endif

				
				/* Update Pattern */
				if (t<numPatterns){
					HitPattern patt2 = pattf(tile, &layout2);
					hit_patternFree(pattern);
					*pattern = patt2;
				}

				/* Free */
				hit_tileFree(tile2);
				hit_tileFree(*origTile);
			}

			/* Update Layout */
			hit_layFree(*lay);
			*lay = layout2;

			/* Reset average */
            hit_avgResetData(avg);

			/* Free */
			hit_tileFree(times);
			hit_tileFree(allTimes);

			/* Increase interval */
			redisInterval += avg->windowSize;
			valuesToRedis = avg->windowSize - 1;
		}

		#undef DEBUG
	}	

	hit_clockStart(callClock);	
}


/**
 * Weighted Moving Average constructor function.
 * 
 * The weights are provided by the user.
 */  
HitAvg hit_avgSimple(int size){
    HitAvg avg = HITAVG_NULL;

    avg.data = (double *)malloc((unsigned int)size * sizeof(double));
    if(avg.data == NULL){
        printf("Error allocating memory.\n");
        return avg;
    }

    avg.weights = (double *)malloc((unsigned int)size * sizeof(double));
    if(avg.weights == NULL){
        printf("Error allocating memory.\n");
        return avg;
    }

    avg.windowSize = size;
    avg.mode = HITAVG_SMA;
    for(int i=0; i<avg.windowSize; i++) avg.weights[i] = 1;
    hit_avgResetData(&avg);

    return avg;
}


/**
 * Weighted Moving Average constructor function.
 * 
 * The weights are provided by the user.
 */  
HitAvg hit_avgWeighted(int size, double * weights){
    HitAvg avg = HITAVG_NULL;

    avg.data = (double *)malloc((unsigned int) size * sizeof(double));
    if(avg.data == NULL){
        printf("Error allocating memory.\n");
        return avg;
    }

    avg.weights = weights;
    avg.windowSize = size;
    avg.mode = HITAVG_WMA;
    hit_avgResetData(&avg);

    return avg;
}

/**
 * Linear Weighted Moving Average constructor function.
 * 
 * This is a particular case of the WMA type. Here, the weights
 * values are [1, size+1], the bigger weight assigned to the newest data.
 * 
 */ 
HitAvg hit_avgLinearlyWeighted(int size){
    HitAvg avg = HITAVG_NULL;

    avg.data = (double *)malloc((unsigned int) size * sizeof(double));
    if(avg.data == NULL){
        printf("Error allocating memory.\n");
        return avg;
    }

    avg.weights = (double *)malloc((unsigned int) size * sizeof(double));
    if(avg.weights == NULL){
        printf("Error allocating memory.\n");
        return avg;
    }

    avg.windowSize = size;
    avg.mode = HITAVG_WMA;
    for(int i=0; i<avg.windowSize; i++) avg.weights[i] = i+1;
    hit_avgResetData(&avg);

    return avg;
}

/**
 * Exponential Moving Average constructor function.
 */ 
HitAvg hit_avgExponential(int size){
    HitAvg avg = HITAVG_NULL;

    avg.data = (double *)malloc((unsigned int) size * sizeof(double));
    if(avg.data == NULL){
        printf("Error allocating memory.\n");
        return avg;
    }

    avg.weights = (double *)malloc((unsigned int) size * sizeof(double));
    if(avg.weights == NULL){
        printf("Error allocating memory.\n");
        return avg;
    }

    avg.windowSize = size;
    avg.mode = HITAVG_EMA;
    for(int i=0; i<avg.windowSize; i++) avg.weights[i] = 1;
    hit_avgResetData(&avg);

    return avg;
}
