

/*
 * This software is provided to enhance knowledge and encourage progress
 * in the scientific community and are to be used only for research and
 * educational purposes. Any reproduction or use for commercial purpose
 * is prohibited without the prior express written permission.
 * 
 * This software is provided "as is" and without any express or implied
 * warranties, including, without limitation, the implied warranties of
 * merchantability and fitness for a particular purpose.
 * 
 * Copyright (c) 2007-2011, Arturo Gonzalez Escribano
 *                          Javier Fresno Bausela
 *                          Carlos de Blas Carton
 *                          Yuri Torres de la Sierra
 * All Rights Reserved.
 */

#ifndef _HitSave_
#define _HitSave_

#include <hit_topology.h>
#include <hit_mpi.h>

#define hit_topCommit( topo )	hit_comTopologyCommitInternal( &(topo), *hit_Comm );
void hit_comTopologyCommitInternal( HitTopology *topo, MPI_Comm oldComm );

#endif
