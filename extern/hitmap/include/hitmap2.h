/**
 * HitMap library main include file.
 *
 * This header file includes the header files needed to use all the functionalities of HitMap.
 * It is the only header file to be used in all Hitmap applications.
 *
 * @file hitmap2.h
 * @version 2.0.1
 * @author Arturo Gonzalez-Escribano
 * @date Oct 2017, Jun 2021
 *
 */

/*
 *
 * @section License Hitmap license
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
 *
 */

#ifndef _HITMAP2_
#define _HITMAP2_

#include "hitmap.h"

/**
 * Version of this Hitmap library release
 */
#undef	HITMAP_VERSION
#define HITMAP_VERSION	2.0.1

/**
 *  @name Macros for the new v2.0 interface
 */
/** @{ */
/**
 * Count the number of parameters in a variadic function.
 * @hideinitializer
 * 
 * @param ... List of parameters.
 * @return \e int Number of parameters in that list.
 */
#define HIT2_COUNTPARAM( ... )	HIT2_COUNTPARAM_N(__VA_ARGS__, 20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1 )
/** @cond INTERNAL */
#define HIT2_COUNTPARAM_N( n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, num, ... )	num
/** @endcond */

/** @hideinitializer */
#define HIT2_CHECKVARPARAM( ... )	HIT2_CHECKVARPARAM_N(__VA_ARGS__, N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,1 )
/** @cond INTERNAL */
#define HIT2_CHECKVARPARAM_N( n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, select, ... )	select
/** @endcond */
/** @} */

/**
 *  @name New Tile access macros
 */
/** @{ */
/**
 * Access to a HitTile with no stride.
 * @hideinitializer
 * 
 * @param var a HitTile.
 * @param ... Number of dimensions of the Tile and list of positions in each dimension.
 * @see hit_tileElemAtNoStride()
 */
#define hit(var, ...)	HIT2_HIT_VARIADIC( var, HIT2_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
/** @cond INTERNAL */
#define HIT2_HIT_VARIADIC(var, num, ...)	hit_tileElemAtNoStride( var, num, __VA_ARGS__ )
/** @endcond */

/**
 * Access to a HitTile.
 * @hideinitializer
 * 
 * @param var a HitTile.
 * @param ... List of positions in each dimension.
 * @see hit_tileElemAt()
 */
#define hitStrided(var, ...)	HIT2_HITSTRIDED_VARIADIC( var, HIT2_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
/** @cond INTERNAL */
#define HIT2_HITSTRIDED_VARIADIC(var, num, ...)	hit_tileElemAt( var, num, __VA_ARGS__ )
/** @endcond */
/** @} */

/**
 * New Shape constructor macro
 * 
 * @hideinitializer
 */
#define hitShape( ... )	HIT2_SHAPE_VARIADIC( HIT2_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
/** @cond INTERNAL */
#define HIT2_SHAPE_VARIADIC( num, ... )	HIT2_SHAPE_VARIADIC_2( num, __VA_ARGS__ )
#define HIT2_SHAPE_VARIADIC_2( num, ... )	hit_shape( num, HIT2_HITSIGS_##num( __VA_ARGS__ ) )
#define HIT2_HITSIGS_1( tuple )	hitSig tuple
#define HIT2_HITSIGS_2( tuple, ... )	hitSig tuple, HIT2_HITSIGS_1( __VA_ARGS__ )
#define HIT2_HITSIGS_3( tuple, ... )	hitSig tuple, HIT2_HITSIGS_2( __VA_ARGS__ )
#define HIT2_HITSIGS_4( tuple, ... )	hitSig tuple, HIT2_HITSIGS_3( __VA_ARGS__ )
/** @endcond */
#define hitShapeSize( ... ) HIT2_SHAPE_VARIADIC_SIZE( HIT2_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
/** @cond INTERNAL */
#define HIT2_SHAPE_VARIADIC_SIZE( num, ... ) HIT2_SHAPE_VARIADIC_SIZE_2( num, __VA_ARGS__ )
#define HIT2_SHAPE_VARIADIC_SIZE_2( num, ... ) hit_shape( num, HIT2_HITSIGS_SIZE_##num( __VA_ARGS__ ) )
#define HIT2_HITSIGS_SIZE_1( size ) hitSig(0, size - 1, 1)
#define HIT2_HITSIGS_SIZE_2( size, ... ) hitSig(0, size - 1, 1), HIT2_HITSIGS_SIZE_1( __VA_ARGS__ )
#define HIT2_HITSIGS_SIZE_3( size, ... ) hitSig(0, size - 1, 1), HIT2_HITSIGS_SIZE_2( __VA_ARGS__ )
#define HIT2_HITSIGS_SIZE_4( size, ... ) hitSig(0, size - 1, 1), HIT2_HITSIGS_SIZE_3( __VA_ARGS__ )
/** @endcond */



/**
 * New Signature constructor macro
 * 
 * @hideinitializer
 * 
 * @param ... A list of parameters specifying the HitSig attributes.
 * 			1 parameter: similar to hit_sigStd()
 * 			2 parameters: begin and end indeces. The value of the stride is 1.
 * 			3 parameters: similar to hit_sig()
 * @see hit_sigStd()
 * @see hit_sig()
 */
#define hitSig( ... )	HIT2_HITSIG_VARIADIC( HIT2_COUNTPARAM(__VA_ARGS__), __VA_ARGS__ )
/** @cond INTERNAL */
#define HIT2_HITSIG_VARIADIC( num, ... )	HIT2_HITSIG_VARIADIC_2( num, __VA_ARGS__ )
#define HIT2_HITSIG_VARIADIC_2( num, ... ) 	hitSig_##num( __VA_ARGS__ )
#define hitSig_1( a )		hit_sig( 0, a-1, 1 )
#define hitSig_2( a, b )	hit_sig( a, b, 1 )
#define hitSig_3( a, b, c )	hit_sig( a, b, c )
/** @endcond */



/**
 * New Tile Type definition including specialized static inline constructors.
 * 
 * @hideinitializer
 * 
 * @param type Name of a valid native or derived C type.
 */
#define hitNewType( type ) 	hit_tileNewType( type ); 			\
	static inline HitTile_##type hitTile_##type( HitShape shp ) {		\
		HitTile_##type newTile = HIT_TILE_NULL_STATIC;			\
		hit_tileDomainShapeAlloc( &newTile, type, shp );		\
		return newTile;							\
	}									\
	static inline HitTile_##type hitTileNoMem_##type( HitShape shp ) {	\
		HitTile_##type newTile = HIT_TILE_NULL_STATIC;			\
		hit_tileDomainShape( &newTile, type, shp );			\
		return newTile;							\
	}																			

/**
 * Redefine the free function to eliminate the distribution data if it exists.
 * 
 * @hideinitializer
 * 
 * @param var a HitTile.
 */
#undef hit_tileFree
#define hit_tileFree(var) {										\
	if ( (var).hierDepth ) {									\
		hit_tileFreeRecInternal(&(var));							\
	}												\
	if ((var).memStatus==HIT_MS_OWNER) {								\
		free((var).memPtr); (var).memPtr=NULL; (var).data=NULL; (var).memStatus=HIT_MS_NOMEM;	\
	}												\
}

/**
 *  @name Generic local tile constructors
 */
/** @{ */

/**
 * Initialize the shape and base type of a new array Tile and allocate memory
 * for its elements.
 *
 * @hideinitializer
 * 
 * @param type The type of the Tile elements. A valid native or derived C type.
 * @param shape \e HitShape Shape object for the new Tile.
 * @return \e HitTile_\<type\> A new HitTile object.
 */
#define hitTile( type, shape )	hitTile_##type( shape )

/**
 * Initialize the shape and base type of a new array Tile.
 * 
 * This function does not allocate memory for the elements of the Tile.
 * 
 * @hideinitializer
 * 
 * @param type The type of the Tile elements.
 * @param shape \e HitShape Shape object for the new array.
 * @return \e HitTile_\<type\> A new HitTile object.
 */
#define hitTileNoMem( type, shape )	hitTileNoMem_##type( shape )
/** @} */

/*
 * Access to cardinality and limits of the tile shape
 * ...
 */

/**
 *  @name Register of topology and layout plugins
 */
/** @{ */

/**
 * Plain Topology
 * @hideinitializer
 */
#define HIT2_TOPO_Plain	hit_topology( plug_topPlain )

/**
 * 2D Array Topology
 * @hideinitializer
 */
#define HIT2_TOPO_Array2D	hit_topology( plug_topArray2D )
/** @hideinitializer */
#define HIT2_TOPO_ArrayDimProjection( dim )	hit_topology( plug_topArrayDimProjection, dim )

/**
 * Blocks Layout
 * @hideinitializer
 */
#define HIT2_LAY_NAME_Blocks plug_layBlocks
/** @hideinitializer */
#define HIT2_LAY_ARGS_Blocks

/**
 * DimBlocks Layout
 * @hideinitializer
 */
#define HIT2_LAY_NAME_DimBlocks( dim ) plug_layDimBlocks
/** @hideinitializer */
#define HIT2_LAY_ARGS_DimBlocks( dim ) , dim

/**
 * Copy Layout
 * @hideinitializer
 */
#define HIT2_LAY_NAME_Copy plug_layCopy
/** @hideinitializer */
#define HIT2_LAY_ARGS_Copy

/**
 * AllInLeader Layout
 * @hideinitializer
 */
#define HIT2_LAY_NAME_InLeader plug_layInLeader
/** @hideinitializer */
#define HIT2_LAY_ARGS_InLeader
/** @} */


/*
 * Shortcuts for other collective communications
 */

/* Reduce */
/**
 * Creates a communication and performs a reduce operation.
 * @hideinitializer
 */
#define hit_comDoReduceTile( ... ) hit_comDoOnce( hit_comReduceTile( __VA_ARGS__ ) )
/**
 * Creates a communication for a reduce operation.
 * @hideinitializer
 * 
 * @return \e HitCom communication structure.
 */
#define hit_comReduceTile( ... ) \
	HIT2_COM_REDUCE_VARIADIC( HIT2_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
/** @cond INTERNAL */
#define HIT2_COM_REDUCE_VARIADIC( num, ... ) \
	HIT2_COM_REDUCE_VARIADIC_2( num, __VA_ARGS__ )
#define HIT2_COM_REDUCE_VARIADIC_2( num, ... ) \
	HIT2_COM_REDUCE_##num( __VA_ARGS__ ) 
#define HIT2_COM_REDUCE_4( tile, root, baseType, operation )	hit_comReduce( hit_tileLayout(tile), root, &HIT_TILE_NULL, &tile, baseType, operation )
#define HIT2_COM_REDUCE_5( tile1, tile2, root, baseType, operation )	hit_comReduce( hit_tileLayout(tile1), root, &tile1, &tile2, baseType, operation )
/** @endcond */


/* Broadcast */
/**
 * Creates a communication and performs a broadcast operation.
 * @hideinitializer
 */
#define hit_comDoBroadcastTile( ... ) hit_comDoOnce( hit_comBroadcastTile( __VA_ARGS__ ) )
/**
 * Creates a communication for a broadcast operation.
 * @hideinitializer
 * 
 * @return \e HitCom communication structure.
 */
#define hit_comBroadcastTile( ... ) \
	HIT2_COM_BROADCAST_VARIADIC( HIT2_COUNTPARAM( __VA_ARGS__ ), __VA_ARGS__ )
/** @cond INTERNAL */
#define HIT2_COM_BROADCAST_VARIADIC( num, ... ) \
	HIT2_COM_BROADCAST_VARIADIC_2( num, __VA_ARGS__ )
#define HIT2_COM_BROADCAST_VARIADIC_2( num, ... ) \
	HIT2_COM_BROADCAST_##num( __VA_ARGS__ ) 
#define HIT2_COM_BROADCAST_3( tile, root, baseType )	hit_comBroadcast( hit_tileLayout(tile), root, &tile, baseType )
/** @endcond */

#endif // HITMAP v2.0
