/**
 * IS Hitmap
 *
 * NAS IntegerSort Benchmark implemented using Hitmap library.
 *
 * @version 5.1
 * @date Jun 2012
 * @author Arturo Gonzalez-Escribano, Javier Fresno Bausela
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

#include <stdlib.h>
#include <stdio.h>
#include <hitmap.h>
#include "prototypes.h"

/******************/
/* default values */
/******************/
#ifndef CLASS
#define CLASS 'S'
#endif
#define MIN_PROCS            1


/*************/
/*  CLASS S  */
/*************/
#if CLASS == 'S'
#define  TOTAL_KEYS_LOG_2    16
#define  MAX_KEY_LOG_2       11
#define  NUM_BUCKETS_LOG_2   9
#endif


/*************/
/*  CLASS W  */
/*************/
#if CLASS == 'W'
#define  TOTAL_KEYS_LOG_2    20
#define  MAX_KEY_LOG_2       16
#define  NUM_BUCKETS_LOG_2   10
#endif

/*************/
/*  CLASS A  */
/*************/
#if CLASS == 'A'
#define  TOTAL_KEYS_LOG_2    23
#define  MAX_KEY_LOG_2       19
#define  NUM_BUCKETS_LOG_2   10
#endif


/*************/
/*  CLASS B  */
/*************/
#if CLASS == 'B'
#define  TOTAL_KEYS_LOG_2    25
#define  MAX_KEY_LOG_2       21
#define  NUM_BUCKETS_LOG_2   10
#endif


/*************/
/*  CLASS C  */
/*************/
#if CLASS == 'C'
#define  TOTAL_KEYS_LOG_2    27
#define  MAX_KEY_LOG_2       23
#define  NUM_BUCKETS_LOG_2   10
#endif


/*************/
/*  CLASS D  */
/*************/
#if CLASS == 'D'
#define  TOTAL_KEYS_LOG_2    29
#define  MAX_KEY_LOG_2       27
#define  NUM_BUCKETS_LOG_2   10
#undef   MIN_PROCS
#define  MIN_PROCS           4
#endif


#define  TOTAL_KEYS          (1 << TOTAL_KEYS_LOG_2)
#define  MAX_KEY             (1 << MAX_KEY_LOG_2)
#define  NUM_BUCKETS         (1 << NUM_BUCKETS_LOG_2)
//#define  NUM_KEYS            (TOTAL_KEYS/(numProcs)*MIN_PROCS)

/*****************************************************************/
/* On larger number of processors, since the keys are (roughly)  */ 
/* gaussian distributed, the first and last processor sort keys  */ 
/* in a large interval, requiring array sizes to be larger. Note */
/* that for large NUM_PROCS, NUM_KEYS is, however, a small number*/
/* The required array size also depends on the bucket size used. */
/* The following values are validated for the 1024-bucket setup. */
/*****************************************************************/

// This is now set dynamically.
/*
#if   NUM_PROCS < 256
#define  SIZE_OF_BUFFERS     3*NUM_KEYS/2
#elif NUM_PROCS < 512
#define  SIZE_OF_BUFFERS     5*NUM_KEYS/2
#elif NUM_PROCS < 1024
#define  SIZE_OF_BUFFERS     4*NUM_KEYS
#else
#define  SIZE_OF_BUFFERS     13*NUM_KEYS/2
#endif
*/


/*****************************************************************/
/* NOTE: THIS CODE CANNOT BE RUN ON ARBITRARILY LARGE NUMBERS OF */
/* PROCESSORS. THE LARGEST VERIFIED NUMBER IS 1024. INCREASE     */
/* MAX_PROCS AT YOUR PERIL                                       */
/*****************************************************************/
#if CLASS == 'S'
#define  MAX_PROCS           128
#else
#define  MAX_PROCS           1024
#endif

#define  MAX_ITERATIONS      10
#define  TEST_ARRAY_SIZE     5



HitClock	mainTimer;

/***********************************/
/* Enable separate communication,  */
/* computation timing and printout */
/***********************************/
/* #define  TIMING_ENABLED         */
#ifdef TIMING_ENABLED
HitClock	timers[3];
#endif


/*************************************/
/* Typedef: if necessary, change the */
/* size of int here by changing the  */
/* int type to, say, long            */
/*************************************/
typedef  int  INT_TYPE;
typedef  long INT_TYPE2;


/********************/
/* Some global info */
/********************/
HitCounter passed_verification;
                                 

/************************************/
/* These are the three main arrays. */
/* See SIZE_OF_BUFFERS def above    */
/************************************/

HitLayout layout_keys;
HitTopology topology;

hit_tileNewType(int);
hit_tileNewType(double);
hit_tileNewType(HitTile_int);
typedef HitTile_int KEY_TYPE;

KEY_TYPE key_array, key_buff1, key_buff2 = HIT_TILE_NULL_STATIC, key_buff3 = HIT_TILE_NULL_STATIC, 
		 full_buckets_plus_test, full_buckets_plus_test2, 
		 bucket_size, bucket_size_totals, test_keys_all, test_keys;


#define hit_key_array(a)	hit_tileElemAtNoStride1(key_array,a)
#define hit_key_buff1(a)	hit_tileElemAtNoStride1(key_buff1,a)
#define hit_key_buff2(a)	hit_tileElemAtNoStride1(key_buff2,a)
#define hit_key_buff3(a)	hit_tileElemAtNoStride1(key_buff3,a)

#define hit_bucket_size(a)			hit_tileElemAtNoStride1(bucket_size,(a))
#define hit_bucket_size_totals(a)	hit_tileElemAtNoStride1(bucket_size_totals,(a))
#define hit_test_keys(a)			hit_tileElemAtNoStride1(test_keys,(a))
#define hit_test_keys_all(a)		hit_tileElemAtNoStride1(test_keys_all,(a))

INT_TYPE bucket_ptrs[NUM_BUCKETS];

HitCom commBucketSizes;



/**********************/
/* Partial verif info */
/**********************/
INT_TYPE2 test_index_array[TEST_ARRAY_SIZE],
         test_rank_array[TEST_ARRAY_SIZE],

         S_test_index_array[TEST_ARRAY_SIZE] = 
                             {48427,17148,23627,62548,4431},
         S_test_rank_array[TEST_ARRAY_SIZE] = 
                             {0,18,346,64917,65463},

         W_test_index_array[TEST_ARRAY_SIZE] = 
                             {357773,934767,875723,898999,404505},
         W_test_rank_array[TEST_ARRAY_SIZE] = 
                             {1249,11698,1039987,1043896,1048018},

         A_test_index_array[TEST_ARRAY_SIZE] = 
                             {2112377,662041,5336171,3642833,4250760},
         A_test_rank_array[TEST_ARRAY_SIZE] = 
                             {104,17523,123928,8288932,8388264},

         B_test_index_array[TEST_ARRAY_SIZE] = 
                             {41869,812306,5102857,18232239,26860214},
         B_test_rank_array[TEST_ARRAY_SIZE] = 
                             {33422937,10244,59149,33135281,99}, 

         C_test_index_array[TEST_ARRAY_SIZE] = 
                             {44172927,72999161,74326391,129606274,21736814},
         C_test_rank_array[TEST_ARRAY_SIZE] = 
                             {61147,882988,266290,133997595,133525895},

         D_test_index_array[TEST_ARRAY_SIZE] = 
                             {1317351170,995930646,1157283250,1503301535,1453734525},
         D_test_rank_array[TEST_ARRAY_SIZE] = 
                             {1,36538729,1978098519,2145192618,2147425337};




/*****************************************************************/
/************   F  I  N  D  _  M  Y  _  S  E  E  D    ************/
/************                                         ************/
/************ returns parallel random number seq seed ************/
/*****************************************************************/

/*
 * Create a random number sequence of total length nn residing
 * on np number of processors.  Each processor will therefore have a 
 * subsequence of length nn/np.  This routine returns that random 
 * number which is the first random number for the subsequence belonging
 * to processor rank kn, and which is used as seed for proc kn ran # gen.
 */

double   find_my_seed( int  kn,       /* my processor rank, 0<=kn<=num procs */
                       int  np,       /* np = num procs                      */
                       long nn,       /* total num of ran numbers, all procs */
                       double s,      /* Ran num seed, for ex.: 314159265.00 */
                       double a )     /* Ran num gen mult, try 1220703125.00 */
{

  long   i, mq,nq,kk,ik;
  double t1,t2,t3,an;



      nq = nn / np;

      for( mq=0; nq>1; mq++,nq/=2 )
          ;

      t1 = a;

      for( i=1; i<=mq; i++ )
        t2 = randlc( &t1, &t1 );

      an = t1;

      kk = kn;
      t1 = s;
      t2 = an;

      for( i=1; i<=100; i++ )
      {
        ik = kk / 2;
        if( 2 * ik !=  kk ) 
            t3 = randlc( &t1, &t2 );
        if( ik == 0 ) 
            break;
        t3 = randlc( &t2, &t2 );
        kk = ik;
      }
	  // @arturo Ago 2015: Avoid a warning of variable initialized but not used
	  (void)t3;

      return( t1 );

}




/*****************************************************************/
/*************      C  R  E  A  T  E  _  S  E  Q      ************/
/*****************************************************************/

void	create_seq( double seed, double a, int num_keys )
{
	double x;
	int    i, k;

        k = MAX_KEY/4;

	for (i=0; i<num_keys; i++)
	{
	    x = randlc(&seed, &a);
	    x += randlc(&seed, &a);
		x += randlc(&seed, &a);
	    x += randlc(&seed, &a);  

		hit_key_array(i) = (int) (k*x);
	}
}



/*****************************************************************/
/*************             R  A  N  K             ****************/
/*****************************************************************/


void rank( int iteration )
{
    INT_TYPE    i, k, key, min_key_val, max_key_val, zero = 0, *key_buff_ptr;
    INT_TYPE    shift = MAX_KEY_LOG_2 - NUM_BUCKETS_LOG_2;
	HitLayout	layout;
	double		weights[NUM_BUCKETS];

	/* Iteration alteration of keys */  
	if ( hit_layImLeader( layout_keys ) )
    {
      hit_key_array(iteration) = iteration;
      hit_key_array(iteration+MAX_ITERATIONS) = MAX_KEY - iteration;
    }

	/* Initialize */
	hit_tileFill( &full_buckets_plus_test, &zero );
	hit_tileFill( &full_buckets_plus_test2, &zero );

	/* Determine where the partial verify test keys are, load into  */
	/* their own tile												*/
	hit_tileForDimDomain( test_keys, 0, i )
		if( hit_tileDimHasArrayCoord( key_array, 0, test_index_array[i] ) )
            hit_test_keys(i) = 
					hit_tileElemAtArrayCoords1( key_array, test_index_array[i] );

	/* Determine the number of local keys in each bucket */
    hit_tileForDimDomain( key_array, 0, i ) 
        hit_bucket_size( hit_key_array(i) >> shift )++;

	/* Accumulative bucket sizes are the bucket pointers */
    bucket_ptrs[0] = 0;
    for( i=1; i< hit_tileCard( bucket_size ); i++ )  
        bucket_ptrs[i] = bucket_ptrs[i-1] + hit_bucket_size(i-1);

	/* Sort into appropriate bucket */
	hit_tileDomainAlloc( &key_buff1, int, 1, hit_layCard( layout_keys ) );
    hit_tileForDimDomain( key_array, 0, i ) 
    {
        key = hit_key_array(i);
        hit_key_buff1(bucket_ptrs[key >> shift]++) = key;
    }

#ifdef  TIMING_ENABLED
    hit_clockStop( timers[1] );
    hit_clockContinue( timers[2] );
#endif

	/* Get the bucket size totals for the entire problem. These		*/
    /* will be used to determine the redistribution of keys     	*/
	/* All processes get all the hidden test keys, in case they need them	*/
	/* for the partial verification stage									*/
	hit_comDo( &commBucketSizes );

#ifdef  TIMING_ENABLED
    hit_clockStop( timers[2] );
    hit_clockContinue( timers[1] );
#endif

    /* Transform bucket population in weigths for distribution calculations */
	hit_tileForDimDomain( bucket_size_totals, 0, i )
		weights[i] = (double) hit_bucket_size_totals(i);

	/* Layout with the redistribution of buckets */
	layout = hit_layout( plug_layContiguous, topology, hit_tileShape( bucket_size ), weights );

	/* Extract information used by IS from the layout */
/*
    INT_TYPE2	total_lesser_keys = 0;
    INT_TYPE2	total_local_keys = 0;
	INT_TYPE	num_local_buckets = 0;
	INT_TYPE	num_previous_buckets = 0;
	for(i=0;i<NUM_BUCKETS;i++) {
		int	elementAssignedTo = hit_lgr_elementGroup( layout, i );
		if ( elementAssignedTo < hit_layGroup( layout ) ) {
			total_lesser_keys += hit_bucket_size_totals(i);
			num_previous_buckets ++;
		}
		else if ( elementAssignedTo == hit_layGroup( layout ) ) {
			total_local_keys += hit_bucket_size_totals(i);
			num_local_buckets ++;
		}
		else break;
	}
*/

/* DEBUG: SHOW LOADS AND BUCKETS FOR ALL PROCESSORS IN iteration 1 */
#ifdef DEBUG
if ( iteration == 1 ) {
//printf("[%d] total keys: %ld,%ld,%d,%d\n", hit_Rank, total_local_keys, total_lesser_keys, num_local_buckets, num_previous_buckets );
printf("[%d] total KEYS: %ld,%ld,%d,%d\n", hit_Rank, (long)hit_layLoad( layout ),
			(long)hit_layPredLoad( layout, 0 ), hit_lgr_card( layout ), hit_lgr_cardPred( layout ) );
}
#endif

	/* Dynamically reallocate the buffer for the number of keys to be received */
	hit_tileFree( key_buff2 );
	hit_tileDomainAlloc( &key_buff2, INT_TYPE, 1, (long)hit_layLoad( layout ) );

#ifdef  TIMING_ENABLED
    hit_clockStop( timers[1] );
    hit_clockContinue( timers[2] );
#endif

	/* Redistribute keys */
	hit_comDoOnce( hit_comAllDistribute( layout, &key_buff1, &hit_bucket_size(0), &key_buff2, HIT_INT ) );
	hit_layFree( layout );
	hit_tileFree( key_buff1 );

#ifdef  TIMING_ENABLED
    hit_clockStop( timers[2] );
    hit_clockContinue( timers[1] );
#endif


	/*  The starting and ending bucket numbers on each processor are
    multiplied by the interval size of the buckets to obtain the 
    smallest possible min and greatest possible max value of any 
    key on each processor                                          */
    min_key_val = ( hit_lgr_cardPred( layout ) ) << shift;
    max_key_val = (( hit_lgr_cardPred( layout ) + hit_lgr_card( layout ) ) << shift) -1;

	/* Reallocate and clear the buffer to compute the local keys population */
	hit_tileFree( key_buff3 );
	hit_tileDomainShapeAlloc( &key_buff3, int, hit_shape(1, hit_sig( min_key_val, max_key_val, 1 ) ) );
	hit_tileFill( &key_buff3, &zero );

	/* Ranking of all keys occurs in this section:                 */
	/* shift it backwards so no subtractions are necessary in loop */
    key_buff_ptr = & hit_tileElemAtArrayCoords1( key_buff3, 0 );

	/* In this section, the keys themselves are used as their 
    own indexes to determine how many of each there are: their
    individual population                                       */
	hit_tileForDimDomain( key_buff2, 0, i ) 
        key_buff_ptr[hit_key_buff2(i)]++;  /* Now they have individual key   */
                                     	   /* population                     */

	/* To obtain ranks of each key, successively add the individual key
    population, not forgetting the total of lesser keys, m.
    NOTE: Since the total of lesser keys would be subtracted later 
    in verification, it is no longer added to the first key population 
    here, but still needed during the partial verify test.  This is to 
    ensure that 32-bit key_buff can still be used for class D.           */
	for( i=1; i<hit_tileCard( key_buff3 ); i++ )
		hit_key_buff3( i ) += hit_key_buff3( i-1 );

	/* This is the partial verify test section */
	/* Observe that test_rank_array vals are   */
	/* shifted differently for different cases */

    hit_tileForDimDomain( test_keys_all, 0, i )
    {                                             
        k = hit_test_keys_all(i);
        if( hit_tileHasArrayCoords1( key_buff3, k ) )
        {
            /* Add the total of lesser keys, m, here */
			// @arturo: POTENTIAL BUG IN THE ORIGINAL CODE: 
			// 			IF k == min_key_val, OUT OF BUFFER ACCESING key_buff_ptr[ k-1 ]
            INT_TYPE2 key_rank = hit_tileElemAtArrayCoords1( key_buff3, k-1 ) + (long)hit_layPredLoad( layout, 0 );
            int failed = 0;

            switch( CLASS )
            {
                case 'S':
                    if( i <= 2 )
                    {
                        if( key_rank != test_rank_array[i]+iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    else
                    {
                        if( key_rank != test_rank_array[i]-iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    break;
                case 'W':
                    if( i < 2 )
                    {
                        if( key_rank != test_rank_array[i]+(iteration-2) )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    else
                    {
                        if( key_rank != test_rank_array[i]-iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    break;
                case 'A':
                    if( i <= 2 )
        	    {
                        if( key_rank != test_rank_array[i]+(iteration-1) )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
        	    }
                    else
                    {
                        if( key_rank != test_rank_array[i]-(iteration-1) )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    break;
                case 'B':
                    if( i == 1 || i == 2 || i == 4 )
        	    {
                        if( key_rank != test_rank_array[i]+iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
        	    }
                    else
                    {
                        if( key_rank != test_rank_array[i]-iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    break;
                case 'C':
                    if( i <= 2 )
        	    {
                        if( key_rank != test_rank_array[i]+iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
        	    }
                    else
                    {
                        if( key_rank != test_rank_array[i]-iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    break;
                case 'D':
                    if( i < 2 )
        	    {
                        if( key_rank != test_rank_array[i]+iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
        	    }
                    else
                    {
                        if( key_rank != test_rank_array[i]-iteration )
                            failed = 1;
                        else
                            hit_counterInc( passed_verification );
                    }
                    break;
            }
            if( failed == 1 )
                printf( "Failed partial verification: "
                        "iteration %d, processor %d, test key %d\n", 
                         iteration,  hit_laySelfRanksDim( layout_keys, 0 ), (int)i );
        }
    }
}




/*****************************************************************/
/*************             M  A  I  N             ****************/
/*****************************************************************/

int main( int argc, char **argv )
{
	int i, iteration;

	/* Initialize Hitmap */
    hit_comInit(&argc,&argv);
   
	/* Initialize the verification arrays if a valid class */
    for( i=0; i<TEST_ARRAY_SIZE; i++ )
        switch( CLASS )
        {
            case 'S':
                test_index_array[i] = S_test_index_array[i];
                test_rank_array[i]  = S_test_rank_array[i];
                break;
            case 'A':
                test_index_array[i] = A_test_index_array[i];
                test_rank_array[i]  = A_test_rank_array[i];
                break;
            case 'W':
                test_index_array[i] = W_test_index_array[i];
                test_rank_array[i]  = W_test_rank_array[i];
                break;
            case 'B':
                test_index_array[i] = B_test_index_array[i];
                test_rank_array[i]  = B_test_rank_array[i];
                break;
            case 'C':
                test_index_array[i] = C_test_index_array[i];
                test_rank_array[i]  = C_test_rank_array[i];
                break;
            case 'D':
                test_index_array[i] = D_test_index_array[i];
                test_rank_array[i]  = D_test_rank_array[i];
                break;
        };


	/* Create a topology: Restrict to a number of processes which is power of 2 */
	topology = hit_topology( plug_topPlainPower2 );

	/* Printout initial NPB info */
    if ( hit_topImLeader( topology ) )
    {
        printf( "\n\n NAS Parallel Benchmarks 3.3 -- IS Benchmark\n\n" );
        printf( " Size:  %ld  (class %c)\n", (long)TOTAL_KEYS*MIN_PROCS, CLASS );
        printf( " Iterations:   %d\n", MAX_ITERATIONS );
        printf( " Number of processes:     %d\n", hit_topCard( topology ) );
    }

	/* Check to see whether total number of processes is within bounds.
    This could in principle be checked in setparams.c, but it is more
    convenient to do it here                                               */
	if( hit_topCard( topology ) < MIN_PROCS || hit_topCard( topology ) > MAX_PROCS)
	{
		if ( hit_topImLeader( topology ) )
			printf( "\n ERROR: number of processes %d not within range %d-%d"
				"\n Exiting program!\n\n", hit_topCard( topology ), MIN_PROCS, MAX_PROCS);
		hit_comFinalize();
		exit( 1 );
	}

	/* Create non-allocated global array of keys */
	KEY_TYPE global_keys;
	hit_tileDomain( &global_keys, INT_TYPE, 1, TOTAL_KEYS*MIN_PROCS );

	/* Build layout for keys */
	layout_keys = hit_layout( plug_layBlocks, topology, hit_tileShape( global_keys ) );

	if ( hit_layImActive( layout_keys ) ) {

		int numProcs = hit_layNumActives( layout_keys );

		/* Create allocated array for bucket sizes + hidden test keys
		 * and prepare tile selections for easy access 					*/
		hit_tileDomainAlloc( &full_buckets_plus_test, INT_TYPE, 1, NUM_BUCKETS+TEST_ARRAY_SIZE );
		hit_tileDomainAlloc( &full_buckets_plus_test2, INT_TYPE, 1, NUM_BUCKETS+TEST_ARRAY_SIZE );
		hit_tileSelect( &bucket_size, &full_buckets_plus_test, hit_shape(1, hit_sigStd( NUM_BUCKETS )) );
		hit_tileSelect( &bucket_size_totals, &full_buckets_plus_test2, hit_shape(1, hit_sigStd( NUM_BUCKETS )) );
		hit_tileSelect( &test_keys, &full_buckets_plus_test, hit_shape(1, hit_sig( NUM_BUCKETS, NUM_BUCKETS+TEST_ARRAY_SIZE-1, 1 )) );
		hit_tileSelect( &test_keys_all, &full_buckets_plus_test2, hit_shape(1, hit_sig( NUM_BUCKETS, NUM_BUCKETS+TEST_ARRAY_SIZE-1, 1 )) );

		/* Select and allocate ancillary tiles and buffers */
		hit_tileSelect( &key_array, &global_keys, hit_layShape( layout_keys ) );
		hit_tileAlloc( &key_array );

		/* Create communication object for the bucket sizes and test keys */
		commBucketSizes = hit_comReduce(layout_keys, HIT_RANKS_NULL, &full_buckets_plus_test, &full_buckets_plus_test2, HIT_INT, HIT_OP_SUM_INT );

		/* Generate random number sequence and subsequent keys on all procs */
		create_seq( find_my_seed( hit_laySelfRanksDim( layout_keys, 0 ),
							numProcs, 
							4*(long)TOTAL_KEYS*MIN_PROCS,
							314159265.00,     /* Random number gen seed */
							1220703125.00 ),  /* Random number gen mult */
					1220703125.00,                  /* Random number gen mult */
					hit_layCard( layout_keys ) );	/* Number of keys in each process */
	
		/*  Do one interation for free (i.e., untimed) to guarantee initialization of  
		all data and code pages and respective tables */
		rank( 1 );

		/*  Start verification counter */
		hit_counterReset( passed_verification );

#ifdef  TIMING_ENABLED 
		/* Initialize separate communication, computation timing */
		hit_clockReset( timers[2] );
#endif

		/* Start timer */             
		hit_clockStart( mainTimer );

#ifdef  TIMING_ENABLED
		hit_clockStart( timers[0] );
		hit_clockStart( timers[1] );
#endif

		/* This is the main iteration */
		for( iteration=1; iteration<=MAX_ITERATIONS; iteration++ )
		{
			if ( CLASS != 'S' && hit_layImLeader( layout_keys ) )  
				printf( "\tIteration %d\n", iteration );
			rank( iteration );
		}

		/* End of timing, obtain maximum time of all processors */
		hit_clockStop( mainTimer );
		hit_clockReduce( layout_keys,  mainTimer );

		/* Free tiles not needed in full verification */
		hit_tileFree( key_array );
		hit_tileFree( full_buckets_plus_test );
		hit_tileFree( full_buckets_plus_test2 );

#ifdef  TIMING_ENABLED
		/* Show timers */
		hit_clockStop( timers[1] );
		hit_clockStop( timers[0] );
		if ( hit_topImLeader( topology ) )
			printf( "\nTimers 0/1/2 = total/computation/communication times\n");
		hit_clockReduce( layout_keys, timers[0] );
		hit_clockPrintMax( timers[0] );
		hit_clockPrintMin( timers[0] );
		hit_clockPrintAvg( timers[0] );
		hit_clockReduce( layout_keys, timers[1] );
		hit_clockPrintMax( timers[1] );
		hit_clockPrintMin( timers[1] );
		hit_clockPrintAvg( timers[1] );
		hit_clockReduce( layout_keys, timers[2] );
		hit_clockPrintMax( timers[2] );
		hit_clockPrintMin( timers[2] );
		hit_clockPrintAvg( timers[2] );
#endif

		/* Tests that keys are in sequence: sorting of last ranked key seq
		occurs here, but is an untimed operation                             */
		full_verify();

		/* Free tiles used for full verification */
		hit_tileFree( key_buff2 );
		hit_tileFree( key_buff3 );

		/* Obtain verification counter sum */
		hit_counterReduce( layout_keys, passed_verification );
		int verificationSum = hit_counterSum( passed_verification );

		/* The final printout  */
		if ( hit_layImLeader( layout_keys ) ) {
			if( verificationSum != 5*MAX_ITERATIONS + numProcs )
				verificationSum = 0;
			c_print_results( "IS",
							 CLASS,
							 (int)(TOTAL_KEYS),
							 MIN_PROCS,
							 0,
							 MAX_ITERATIONS,
							 -1,
							 numProcs,
							 hit_clockGetMaxSeconds( mainTimer ),
							 ((double) (MAX_ITERATIONS)*TOTAL_KEYS*MIN_PROCS)
												  /hit_clockGetMaxSeconds( mainTimer)/1000000.0,
							 "keys ranked", 
							 verificationSum,
							 "", // NPBVERSION
							 "", // COMPILETIME
							 "", // MPICC
							 "", // CLINK
							 "", // CMPI_LIB
							 "", // CMPI_INC
							 "", // CFLAGS
							 ""); // CLINKFLAGS
		}
	} // END hit_layImActive

	/* Free communication objects, layouts, and topology */
	hit_comFree( commBucketSizes );
	hit_layFree( layout_keys );
	hit_topFree( topology );
    hit_comFinalize();

    return 0;
         /**************************/
}        /*  E N D  P R O G R A M  */
         /**************************/



/*****************************************************************/
/*************    F  U  L  L  _  V  E  R  I  F  Y     ************/
/*****************************************************************/

void full_verify( void )
{
    
    INT_TYPE    i;
    KEY_TYPE	largest_key;

	/* Allocate the key array to sort the keys in buff2 */
	hit_tileDomainAlloc( &key_array, int, 1, hit_tileCard( key_buff2 ) );

	/* Now, finally, sort the keys:  */
	INT_TYPE *key_buff_ptr = & hit_tileElemAtArrayCoords1( key_buff3, 0 );
	hit_tileForDimDomain( key_buff2, 0, i ) 
        hit_key_array( --key_buff_ptr[hit_key_buff2(i)] ) = hit_key_buff2(i);

	/* Get the largest key */
	hit_tileDomainAlloc( &largest_key, int, 1, 1 );
	hit_tileElemAt( largest_key, 1, 0 ) = (hit_tileCard( key_buff2 ) > 0) ? hit_key_array( hit_tileDimEnd( key_buff2, 0 ) ) : 0;

	/* Communication: Shift largest key value to next processor  */
    hit_comDoOnce( hit_comShiftDim( layout_keys, 0, +1, &largest_key, HIT_INT ) );

	/* Confirm that neighbor's greatest key value 
    is not greater than my least key value       */              
    int j = 0;
    if( ! hit_layImLeader( layout_keys ) && hit_tileCard( key_buff2 ) > 0 )
        if( hit_tileElemAt( largest_key, 1 , 0 ) > hit_key_array(0) )
            j++;

#ifdef DEBUG
    if( j != 0 )
    {
        printf( "Processor %d:  Full_verify: Before: %d, first: %d!!!: %d\n", hit_laySelfRanksDim( layout_keys, 0 ), j, hit_tileElemAt( largest_key, 1, 0 ), hit_key_array(0) );
    }
#endif

	/* Confirm keys correctly sorted: count incorrectly sorted keys, if any */
    for( i=1; i<hit_tileCard( key_buff2 ); i++ )
        if( hit_key_array(i-1) > hit_key_array(i) )
            j++;

    if( j != 0 )
    {
        printf( "Processor %d:  Full_verify: number of keys out of sort: %d\n", hit_laySelfRanksDim( layout_keys, 0 ), j );
    }
    else
		hit_counterInc( passed_verification );

	/* Free resources */
	hit_tileFree( key_array );
	hit_tileFree( largest_key );
}

