/**
 * HitMap library main include file.
 *
 * This header file includes the header files needed to use all the functionalities of HitMap.
 * It is the only header file to be used in all Hitmap applications.
 *
 * @file hitmap.h
 * @version 1.3
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @date Mar 2019
 *
 */

/**
 * @mainpage
 * Hitmap is a highly-efficient library for hierarchical tiling and mapping of
 * arrays. It is designed to be used with an SPMD (Single Process, Multiple Data)
 * parallel programming language and it aims to simplify parallel programming,
 * providing functionalities to create, manipulate, distribute, and communicate
 * tiles and hierarchies of tiles.
 * 
 * Hitmap library supports functionalities to:
 * - Create and manipulate hierarchical dense or sparse data structures based on 
 *   arrays and graphs
 * - Generate a virtual topology structure arranging the available physical
 *   processors.
 * - Mapping domains and data to the different processor with chosen partitioning 
 *   and load-balancing techniques.
 * - Automatically determine inactive processors at any stage of the computation,
 *   preventing them to interfere with the active processors activities.
 * - Identify the neighbor processors to use in point-to-point or semi-collective
 *   communications.
 * - Build communication patterns to be reused across algorithm iterations.
 *
 * @section Architecture Architecture
 * Hitmap is designed with an object-oriented approach, although it is
 * implemented in C language. Each data structure is provided with a set of functions
 * that work as class methods.
 * The library functionalities can be classified in three sets or modules:
 * - \ref Tiling "Tiling module": Functionalities to manage
 *   	data structures and its hierarchical tiling.
 * - \ref Mapping "Mapping module": Functionalities to automatically part domains 
 *   and distribute data. 
 * - \ref Comm "Communication module": Functionalities to create reusable 
 *   communication patterns for distributed hierarchical tiles. 
 *
 * @section ExtLib	External Libraries
 *
 * Hitmap uses two external libraries related to the management of sparse data structures.
 * These libraries are open source code, and the versions tested with the library are
 * included with the Hitmap software package.
 *
 * \li <em>Harwell-Boeing I/O routines in C, v1.0</em>
 * 				A library to read/write sparse data structures in Harwell-Boeing file
 * 				format. It is distributed under an open software license
 * 				by <em>National Institute of Standards and Technology, MD. K.A. Remington</em>.
 * 				See more information and pemission notice in
 * 				<a href="http://math.nist.gov/~KRemington/harwell_io/README.html">Distribution Documentation for Harwell-Boeing I/O in C</a>.
 *
 * \li <em>Metis 4.0.1</em>
 * 			METIS is a set of serial programs for partitioning graphs, partitioning finite 
 * 			element meshes, and producing fill reducing orderings for sparse matrices. 
 * 			The algorithms implemented in METIS are based on the multilevel recursive-bisection, 
 * 			multilevel k-way, and multi-constraint partitioning schemes developed at <em>Karipys
 * 			Lab (University of Minnesota, USA)</em>. 
 * 			It is distributed under <a href="http://www.apache.org/licenses/LICENSE-2.0">Apache License Version 2.0</a>. See more information in <a href="http://glaros.dtc.umn.edu/gkhome/metis/metis/overview">METIS - Serial Graph Partitioning and Fill-reducing Matrix Ordering</a>.
 *
 * @section ExamplesSec	Application examples
 *
 * Hitmap library has been tested with several example applications and benchmarks used
 * for experimentation and publication. They include linear algebra aplications,
 * stencil codes, a couple of examples of the NAS Parallel Benchamarks, sorting algorithms,
 * sparse matrix programs, physical systems simulations using graphs, etc.
 *
 * They are described in the \ref Examples page.
 *
 * @section Publications Publications
 *
 * The Hitmap library started as a research prototype in the context of the Trasgo framework
 * project around 2005. In 2007 it took the form of an independent library and since then it
 * has been evolving and being used for research purposes. 
 *
 * There are several research publications related to the Hitmap library, its development, use,
 * advantages, performance, and comparisons with other related works.
 *
 * \todo Link to the Trasgo publications page, and full citation of the most
 * 	relevant publications
 *
 * @section License Hitmap license
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
 *
 */

 /**
  * @page Examples Application examples
  *
  * Documentation of the application examples included with the Hitmap library.
  */

 /**
 *
 * @defgroup Tiling Tiling module
 * 		Functionalities for managing hierarchical titling data structures.
 *
 * 		This module contains functionalities for creation, allocation, hierarchical tiling, 
 * 		and elements access for dense and sparse data structures (arrays and graphs).
 *
 * @defgroup FileOps Input/output management
 * 		Functionalities to read/write data from/to files.
 *
 * 		This module contains functionalities to read/write the data of whole tile
 * 		variables from/to files. It includes functions for dense array tiles that can
 * 		bu used to read/write distributed tiles in parallel. For sparse data
 * 		structures in includes functionalities to read/write tiles in Harwell-Boeing
 * 		file format.
 *
 * 		@ingroup Tiling
 *
 * @defgroup Mapping Mapping module
 * 		Functionalities for automatically mapping and distributing domains and tiles.
 *
 * 		This module contains functionalities to automatically part domains and distribute data,
 * 		on function of abstract virtual topology and layout policies, that automatically
 * 		adapt their results to the actual physical topology at run-time.
 *
 * @defgroup Comm Communication module
 * 		Functionalities for communication of hierarchical tiles.
 *
 * 		This module contains functionalities to build reusable communication objects and patterns
 * 		for hierarchical tiles, in terms of auotmatic mapping results.
 *
 * @defgroup Dev Hitmap developers module
 * 		This module contains utilities and other functionalities used in the Hitmap library
 * 		codes. These interfaces are not designed for Hitmap library users.
 */
#ifndef _HITMAP_
#define _HITMAP_

/**
 * Version of this Hitmap library release
 */
#define HITMAP_VERSION	1.3

#include "hit_funcop.h"

#include "hit_topology.h"
#include "hit_layout.h"
#include "hit_pattern.h"

//#include "hit_shape.h"
#include "hit_sshape.h"
#include "hit_cshape.h"
#include "hit_bshape.h"

#include "hit_dump.h"

#include "hit_tile.h"
#include "hit_tileFile.h"
#include "hit_gctile.h"
#include "hit_gbtile.h"
#include "hit_mctile.h"
#include "hit_mbtile.h"


#include "hit_utils.h"
#include "hit_env.h"
#include "hit_file.h"

#include "hit_blockTile.h"

#include "hit_domain.h"
#include "hit_view.h"


#endif
