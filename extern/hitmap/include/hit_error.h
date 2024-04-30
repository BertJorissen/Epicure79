/**
 * 
 * Hitmap error macros. A set of macro-functions to show error and warnings.
 * 
 * @file hit_error.h
 * @version 1.2
 * @author Javier Fresno Bausela
 * @author Arturo Gonzalez-Escribano
 * @date Mar 2013
 * 
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

#ifndef _HitError_
#define _HitError_

#include <stdio.h>

/* MACRO TO AVOID WARNINGS WHEN A PARAMETER IS NOT USED */
#define HIT_NOT_USED(x) ((void)(x))

/* ERROR CODES */
#define HIT_OK			0
#define HIT_ERROR		255
#define HIT_ERR_INTERNAL	250
#define HIT_ERR_USER		240





/* INTERNAL ERROR AND WARNING MESSAGES */
#define hit_errInternal( routine, text, extraParam, file, numLine )	\
	{							\
	fprintf(stderr,"Hit RunTime-Error (%s): Internal - %s %s, used in %s[%d]\n", routine, text, extraParam, file, numLine); \
	exit( HIT_ERR_INTERNAL );		\
	}

#define hit_warnInternal( routine, text, extraParam, file, numLine )	\
	{							\
	fprintf(stderr,"Hit RunTime-Warning (%s): Internal - %s %s, used in %s[%d]\n", routine, text, extraParam, file, numLine); \
	}


/* ERROR AND WARNING MESSAGES FOR PROGRAMMERS/USERS */
#define hit_error( name, file, numLine )	\
	{							\
	fprintf(stderr,"Hit Programmer, RunTime-Error: %s, in %s[%d]\n", name, file, numLine);				\
	exit( HIT_ERR_USER );		\
	}

#define hit_warning( name, file, numLine )	\
	{							\
	fprintf(stderr,"Hit Programmer, RunTime-Warning: %s, in %s[%d]\n", name, file, numLine); \
	}

#define hit_error_here(name)	hit_error(name, __FILE__, __LINE__)
#define hit_warning_here(name)	hit_warning(name, __FILE__, __LINE__)


#define hit_err( prefix, text, postfix )	\
	{					\
	fprintf(stderr,"Hit RunTime-Error: %s %s %s\n", prefix, text, postfix); \
	exit( HIT_ERR_USER );			\
	}
#define hit_errInt( prefix, text, postfix )	\
	{					\
	fprintf(stderr,"Hit RunTime-Error: %s %s %d\n", prefix, text, postfix); \
	exit( HIT_ERR_USER );			\
	}
#define hit_warn( prefix, text, postfix )	\
	{							\
	fprintf(stderr,"Hit RunTime-Warning: %s %s %s\n", prefix, text, postfix); \
	}
#define hit_warnInt( prefix, text, postfix )	\
	{							\
	fprintf(stderr,"Hit RunTime-Warning: %s %s %d\n", prefix, text, postfix); \
	}



/**
 * This is a function that return the gdb trace of the current process.
 * It is a usefull function for debugging but it only works in gcc.
 */
char * get_gdb_trace();

#endif

/* END OF HEADER FILE _HitError_ */
