/**
* Definition of functionalites to perform automatic load balancing between processes.
* Redistribute work load and data from the less powerful processes to
* the most powerful ones.
*
* @file hit_alb.h
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

#ifndef _HitAvg_
#define _HitAvg_

#include <stdio.h>
#include <stdlib.h>
#include "hit_tile.h"
#include "hit_layout.h"
#include "hit_pattern.h"
#include "hit_sshape.h"
#include "hit_view.h"
#include "hitmap2.h"

#ifdef __cplusplus
	extern "C" {
#endif // cplusplus


/**
 * Type for Pattern creation functions.
 */ 
typedef HitPattern (*HitPatternFunction)(void *, HitLayout *);

/**
 * Type for Tile initialization functions.
 */ 
typedef void (*HitTileInitFunction)(HitTile *);



/**
 * Flag to tell that an HitAvg's array data is not full.
 * @hideinitializer
 */ 
#define HITAVG_NOT_FULL -1

/**
 * Flag to tell that an HitAvg's array data is full.
 * @hideinitializer
 */ 
#define HITAVG_FULL 1

/**
 * Null HitAvg.
 * @hideinitializer
 */
#define HITAVG_NULL {0, 0, 0, 0, 0, 0, 0, HITAVG_SMA}


/** Available moving average types. */
enum HitAvgMode {
    HITAVG_SMA, /**<Simple Moving Average. */
    HITAVG_WMA, /**<Weighted Moving Average. */
    HITAVG_EMA /**<Exponential Moving Average. */
};

/**
 * Structure to manage moving averages (to store time of alb functions).
 * 
 * A moving average basic elements are a data array, a weights array,
 * a window size and a type (simple, weighted or exponential).
 * 
 * New values can be inserted in the average but cannot be removed.
 * If the data array is already full the new value will replace the oldest one.
 */  
typedef struct HitAvg{
    int windowSize;     /**<Number of elements in the average. */
    int pointer;        /**<Pointer to the next position to fill. */
    int first;          /**<Pointer to the least recent element. */
    int full;           /**<Flag to check if the data array is full. */
    double * data;      /**<Array of data. */
    double * weights;   /**<Array of weights for each data.*/
    double previousAvg; /**<Last value of the mean calculated. */
    enum HitAvgMode mode;  /**<Average type. */
} HitAvg;


/**
 * Extern variable to use in examples.
 * 
 * @private
 */ 
extern HitAvg sysAvg;


/**
 * Reset the average structure to its initial state.
 * 
 * Deletes all data and previous avg value, and sets \c avg.pointer and \c avg.first to 0.
 * @param[in,out] avg Pointer to the HitAvg object.
 */ 
void hit_avgResetData(HitAvg *avg);

/**
 * Compute Simple Moving Average.
 * 
 * @private
 * 
 * @param[in] avg The HitAvg object.
 * @retval HITAVG_NOT_FULL If the avg's data array is not full.
 * @retval double The SMA otherwise.
 */ 
double hit_avgGetSMA(HitAvg avg);

/**
 * Compute Weighted Moving Average.
 * 
 * @private
 * 
 * @param[in] avg The HitAvg object.
 * @retval HITAVG_NOT_FULL If the avg's data array is not full.
 * @retval double The WMA otherwise.
 */
double hit_avgGetWMA(HitAvg avg);

/**
 * Update the value of an Exponential Moving Average.
 * If the Avg's data array is not full it does nothing.
 * 
 * @private
 * 
 * @param[in] avg Pointer to the HitAvg object.
 * @param[in] d New data.
 */
void hit_avgUpdateEMA(HitAvg *avg, double d);

/**
 * Insert new data in the data array.
 * The mean is updated with this new value.
 * 
 * @param[in,out] avg Pointer to the HitAvg object.
 * @param[in] d New data.
 */ 
void hit_avgInsertData(HitAvg *avg, double d);

/**
 * Get current value of the mean.
 * 
 * @param[in] avg The HitAvg object.
 * @retval HITAVG_NOT_FULL If the avg's data array is not full.
 * @retval double Otherwise it returns the average (simple, weighted or
 *              exponential, depending on the object's type).
 */ 
double hit_avgGetAvg(HitAvg avg);


/** @name HitAvg Constructors */
/** @{*/
/* CONSTRUCTORS */

/**
 * Simple Moving Average constructor function.
 * 
 * In this type of Avg all the weights values are 1.
 * 
 * @param[in] size The size of the window.
 * @retval HITAVG_NULL If there was a problem with memory allocation.
 * @retval HitAvg The new object otherwise.
 */  
HitAvg hit_avgSimple(int size);


/**
 * Weighted Moving Average constructor function.
 * 
 * The weights are provided by the user.
 * 
 * @param[in] size The size of the window. 
 * @param[in] weights Pointer to a weights array. The order should be that the
 *          first value is the weight for the least recent data and the last
 *          value is for the most recent.
 * @retval HITAVG_NULL If there was a problem with memory allocation.
 * @retval HitAvg The new object otherwise.
 */  
HitAvg hit_avgWeighted(int size, double * weights);

/**
 * Linear Weighted Moving Average constructor function.
 * 
 * This is a particular case of the WMA type. Here, the weights
 * values are [1, size+1], the bigger weight assigned to the newest data.
 * 
 * @param[in] size The size of the window.
 * @retval HITAVG_NULL If there was a problem with memory allocation.
 * @retval HitAvg The new object otherwise.
 */ 
HitAvg hit_avgLinearlyWeighted(int size);

/**
 * Exponential Moving Average constructor function.
 * 
 * In this type of Avg, the weights are not necessary since
 * the formula for the average does not use them.
 * 
 * @param[in] size The size of the window.
 * @retval HITAVG_NULL If there was a problem with memory allocation.
 * @retval HitAvg The new object otherwise.
 */ 
HitAvg hit_avgExponential(int size);
/** @} */

/**
 * Print the data array of an Avg from the least recent element to the last element inserted.
 * 
 * @param[in] avg The HitAvg object.
 */ 
void hit_avgPrintData(HitAvg avg);


/** @name Automatic Load Balancing Functions*/
/** @{*/
    
/**
 * Perform an load balancing and redistribution function based on the average
 * times of execution of each process.
 * The redistribution interval has a fixed size.
 * 
 * If the average data array is not full the redistribution is not done. Else, it updates
 * the Layout, Tiles and Patterns with the new load.
 * 
 * @param ttile Array of pointers to HitTiles.
 * @param numTiles Number of HitTiles in the array.
 * @param lay Pointer to the HitLayout.
 * @param avg Pointer to the average structure.
 * @param patt Array of pointers to HitPatterns.
 * @param numPatterns Number of HitPatterns in the array.
 * @param pattf Pointer to the function that updates the Pattern.
 * @param tilef Pointer to a function that initializes the tiles before the redistribution.
 * @param shpview HitShapeView to transform local HitShape.
 */ 
void hit_albWeightedRedistributeFixedInterval(HitTile * ttile[], int numTiles, HitLayout * lay, HitAvg * avg,
	HitPattern * patt[], int numPatterns, HitPatternFunction pattf, HitTileInitFunction tilef,
	HitShpView shpview, double time);


/**
 * Perform an load balancing and redistribution function based on the average
 * times of execution of each process.
 * The redistribution interval increases its size with each redistribution.
 * 
 * If the average data array is not full the redistribution is not done. Else, it updates
 * the Layout, Tiles and Patterns with the new load.
 * 
 * @param ttile Array of pointers to HitTiles.
 * @param numTiles Number of HitTiles in the array.
 * @param lay Pointer to the HitLayout.
 * @param avg Pointer to the average structure.
 * @param patt Array of pointers to HitPatterns.
 * @param numPatterns Number of HitPatterns in the array.
 * @param pattf Pointer to the function that updates the Pattern.
 * @param tilef Pointer to a function that initializes the tiles before the redistribution.
 * @param shpview HitShapeView to transform local HitShape.
 */ 
void hit_albWeightedRedistributeIncrInterval(HitTile * ttile[], int numTiles, HitLayout * lay, HitAvg * avg,
	HitPattern * patt[], int numPatterns, HitPatternFunction pattf, HitTileInitFunction tilef,
	HitShpView shpview, double time);

/** @}*/

#ifdef __cplusplus
	}
#endif // cplusplus

#endif
