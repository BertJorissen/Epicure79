/**
 * Developers debug functions
 * They are used to dump the fields of tile structures in a stream
 *
 * @file hit_dump.h
 * @version 1.1
 * @author Arturo Gonzalez-Escribano
 * @date Sep 2012
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

#ifndef _HitDump_
#define _HitDump_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hit_sshape.h"
#include "hit_com.h"

//#include <hit_tile.h>
//#include "hit_error.h"
//#include "hit_shape.h"

/* 1. DEBUG: DUMP VARIABLE STRUCTURE IN stderr */
#define hit_dumpTile(var)	hit_dumpTileInternal(&(var), #var, stderr)

/* 2. DEBUG: DUMP VARIABLE STRUCTURE IN A PARTICUALR FILE FOR THIS PROCESS */
#define hit_dumpTileFile(var,prefix,comment)	{ char name[16]; sprintf(name,"%s.%d",prefix,hit_Rank); FILE *fich; fich=fopen(name, "a"); fprintf(fich, comment); hit_dumpTileInternal(&(var), #var, fich); fclose(fich); }


/* 3. PROTOTYPE FOR THE INTERNAL DUMPING FUNCTION */
void hit_dumpTileInternal(const void *var, const char * name, FILE *file);


/* END OF HEADER FILE _HitDump_ */
#endif
