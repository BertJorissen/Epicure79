/**
* Functions to manipulate clocks and obtain times to measure codes.
 * 
* @file hit_utils.h
* @version 1.1
* @author Arturo Gonzalez-Escribano
* @author Javier Fresno Bausela
* @author Carlos de Blas Carton
* @date May 2012
*
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

#ifndef _HitUtils_
#define _HitUtils_

/**
 * hit_clockSynchronize: Synchronize the processors active in a layout.
 * @param layout HitLayout object.
 */
#define hit_clockSynchronize(lay)	MPI_Barrier((lay).topo.pTopology->comm)
/**
 * hit_clockSynchronize: Synchronize all the processors.
 */
#define hit_clockSynchronizeAll()	MPI_Barrier( MPI_COMM_WORLD );

/**
* Hit MPI BASED CLOCKS, easily portable to other libraries with time utilities
*/
typedef struct {
	int		state;		/**< Clock state. */
	double	seconds;	/**< Seconds. */
	double	min;   	    /**< Seconds (Min). */
	double	max;   	    /**< Seconds (Max). */
	double	avg;   	    /**< Seconds (Avg). */
	double	buffer;
} HitClock;

/** Clock state running. */
#define	HIT_CLOCK_RUNNING	0
/** Clock state stopped. */
#define	HIT_CLOCK_STOPPED	1

/**
* reset of a clock. The time is reset to 0.
* @param c clock
*/
#define hit_clockReset(c)	{			\
		c.seconds = 0.0;					\
		c.state = HIT_CLOCK_STOPPED;	\
		}

/**
* The clock starts to measure time.
* @param c clock
*/
#define hit_clockStart(c)	{			\
		c.seconds= MPI_Wtime();		\
		c.state = HIT_CLOCK_RUNNING;	\
		}

/**
* All the processors from a topology make a reduction to get the maximun time
* of their respective clock. Processor number 0 gets the time inside its clock
* @param topo topology
* @param c clock
*/
#define hit_clockMaxReduce(topo, c)	{										\
		MPI_Reduce( &c.seconds, &c.buffer, 1, MPI_DOUBLE,	 				\
									MPI_MAX, 0, (topo).pTopology->comm );							\
		c.seconds = c.buffer;												\
		}

/**
* The clock stops to measure time. The seconds stored are the difference between the start of the clock
* and the call to this function.
* @param c clock
*/
#define hit_clockStop(c)	{					\
		c.seconds= MPI_Wtime() - c.seconds;	\
		c.state = HIT_CLOCK_STOPPED;			\
		}



/**
* The clock continues measuring time and the time is updated with the start time.
* The time between hit_clockStop and this function is not measured.
* @param c clock
*/
#define hit_clockContinue(c)	{				\
		c.seconds = MPI_Wtime() - c.seconds;	\
		c.state = HIT_CLOCK_RUNNING;			\
		}

/**
* Returns true if the clock is running
* @param c clock
*/
#define hit_clockIsRunning(c)	( c.state == HIT_CLOCK_RUNNING )


/**
* All the processors active in a layout make a reduction to get the maximun, minimum and average
* time of their respective clock. Processor number 0 gets the reduced times
* @param lay HitLayout object
* @param c clock
*/
#define hit_clockReduce(lay, c)	{										\
		MPI_Reduce( &c.seconds, &c.buffer, 1, MPI_DOUBLE,	 				\
							MPI_MAX, 0, (lay).pTopology[0]->comm );	\
		c.max = c.buffer;													\
		MPI_Reduce( &c.seconds, &c.buffer, 1, MPI_DOUBLE,	 				\
							MPI_MIN, 0, (lay).pTopology[0]->comm );	\
		c.min = c.buffer;													\
		MPI_Reduce( &c.seconds, &c.buffer, 1, MPI_DOUBLE,	 				\
							MPI_SUM, 0, (lay).pTopology[0]->comm );	\
		c.avg = c.buffer / hit_layNumActives( lay );								\
		}

/*
* All processors make a reduction to get the maximun, minimum and average
* time of their respective clock. Processor number 0 gets the reduced times
* @param c clock
*/
#define hit_clockWorldReduce(c)	{										\
		MPI_Reduce( &c.seconds, &c.buffer, 1, MPI_DOUBLE,	 				\
							MPI_MAX, 0, MPI_COMM_WORLD );	\
		c.max = c.buffer;													\
		MPI_Reduce( &c.seconds, &c.buffer, 1, MPI_DOUBLE,	 				\
							MPI_MIN, 0, MPI_COMM_WORLD );	\
		c.min = c.buffer;													\
		MPI_Reduce( &c.seconds, &c.buffer, 1, MPI_DOUBLE,	 				\
							MPI_SUM, 0, MPI_COMM_WORLD );	\
		c.avg = c.buffer / hit_NProcs;								\
		}

/**
* Prints a clock
* @param c clock
*/
#define hit_clockPrint(c)	{												\
		if (hit_Rank==0) printf("HitClock_%s: %14.6lf\n", #c, c.seconds);	\
		}
#define hit_clockPrintMax(c)	{												\
		if (hit_Rank==0) printf("HitClock_%s Max: %14.6lf\n", #c, c.max);	\
		}
#define hit_clockPrintMin(c)	{												\
		if (hit_Rank==0) printf("HitClock_%s Min: %14.6lf\n", #c, c.min);	\
		}
#define hit_clockPrintAvg(c)	{												\
		if (hit_Rank==0) printf("HitClock_%s Avg: %14.6lf\n", #c, c.avg);	\
		}


/**
* Gets the seconds of a clock
* @param c clock
*/
#define hit_clockGetSeconds(c)	(c.seconds)
#define hit_clockGetMaxSeconds(c)	(c.max)
#define hit_clockGetMinSeconds(c)	(c.min)
#define hit_clockGetAvgSeconds(c)	(c.avg)




/**
* Hit MPI BASED INTEGER COUNTERS, easily portable to other libraries
*/
typedef struct {
	int	value;		/**< Counter value. */
	int	min;   	    /**< Value (Min). */
	int	max;   	    /**< Value (Max). */
	int	sum;   	    /**< Value (Sum). */
	int	avg;   	    /**< Value (Avg). */
	int	buffer;
} HitCounter;

/**
* Access counter value
* @param c Counter
*/
#define hit_counterValue(c)		((c).value)

/**
* Reset of a counter to 0.
* @param c Counter
*/
#define hit_counterReset(c)		(c).value = 0

/**
* Add value to a counter.
* @param c Counter
* @param value Value
*/
#define hit_counterAdd(c,value)		(c).value += (value)

/**
* Increment a counter
* @param c Counter
*/
#define hit_counterInc(c)		(c).value++

/**
* Decrement a counter
* @param c Counter
*/
#define hit_counterDec(c)		(c).value--


/**
* All the processors active in a layout make a reduction to get the maximun, minimum and average
* time of their respective counter. Processor number 0 gets the reduced values
* @param lay HitLayout object
* @param c Counter
*/
#define hit_counterReduce(lay, c)	{										\
		MPI_Reduce( &c.value, &c.buffer, 1, MPI_INT,	 				\
							MPI_MAX, 0, (lay).pTopology[0]->comm );	\
		c.max = c.buffer;													\
		MPI_Reduce( &c.value, &c.buffer, 1, MPI_INT,	 				\
							MPI_MIN, 0, (lay).pTopology[0]->comm );	\
		c.min = c.buffer;													\
		MPI_Reduce( &c.value, &c.buffer, 1, MPI_INT,	 				\
							MPI_SUM, 0, (lay).pTopology[0]->comm );	\
		c.sum = c.buffer;													\
		c.avg = c.buffer / hit_layNumActives( lay );								\
		}

/*
* All processors make a reduction to get the maximun, minimum and average
* value of their respective counter. Processor number 0 gets the reduced values
* @param c counter
*/
#define hit_counterWorldReduce(c)	{										\
		MPI_Reduce( &c.value, &c.buffer, 1, MPI_INT,	 				\
							MPI_MAX, 0, MPI_COMM_WORLD );	\
		c.max = c.buffer;													\
		MPI_Reduce( &c.value, &c.buffer, 1, MPI_INT,	 				\
							MPI_MIN, 0, MPI_COMM_WORLD );	\
		c.min = c.buffer;													\
		MPI_Reduce( &c.value, &c.buffer, 1, MPI_INT,	 				\
							MPI_SUM, 0, MPI_COMM_WORLD );	\
		c.sum = c.buffer;													\
		c.avg = c.buffer / hit_NProcs;								\
		}

/**
* Prints a counter
* @param c counter
*/
#define hit_counterPrint(c)	{												\
		if (hit_Rank==0) printf("HitCounter_%s: %010d\n", #c, c.value);	\
		}
#define hit_counterPrintMax(c)	{												\
		if (hit_Rank==0) printf("HitCounter_%s Max: %010d\n", #c, c.max);	\
		}
#define hit_counterPrintMin(c)	{												\
		if (hit_Rank==0) printf("HitCounter_%s Min: %010d\n", #c, c.min);	\
		}
#define hit_counterPrintSum(c)	{												\
		if (hit_Rank==0) printf("HitCounter_%s Sum: %010d\n", #c, c.sum);	\
		}
#define hit_counterPrintAvg(c)	{												\
		if (hit_Rank==0) printf("HitCounter_%s Avg: %010d\n", #c, c.avg);	\
		}


/**
* Gets the reduced values of a counter
* @param c counter
*/
#define hit_counterMax(c)	(c.max)
#define hit_counterMin(c)	(c.min)
#define hit_counterSum(c)	(c.sum)
#define hit_counterAvg(c)	(c.avg)

/* END OF HEADER FILE _HitUtils_ */
#endif
