/**
 * HitMap library
 * Reading environment variables for optional runtime features related to input/output
 *
 * @file hit_env.h
 * @ingroup FileOps
 * @version 1.0
 * @author Arturo Gonzalez-Escribano
 * @date Aug 2023
 */

/*
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

#ifndef _HITMAP_ENV_
#define _HITMAP_ENV_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hit_error.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Return the value of an environment variable with only yes/no values. Default: yes
 * 
 * @param var \e const char *, string with the name of the environment variable
 * @return \e int, value 1 (yes) or 0 (no). In case var is not defined the default value is 1
 * @see hit_envOptions(), hit_envInteger(), hit_envNoYes()
 */
int hit_envYesNo( const char *var );

/**
 * Return the value of an environment variable with only yes/no values. Default: no
 * 
 * @param var \e const char *, string with the name of the environment variable
 * @return \e int, value 1 (yes) or 0 (no). In case var is not defined the default value is 0
 * @see hit_envOptions(), hit_envInteger(), hit_envYesNo()
 */
int hit_envNoYes( const char *var );

/**
 * Return the value of an environment variable from a list of posible values
 * 
 * @param var \e const char *, string with the name of the environment variable
 * @param opts \e const char**, array of strings with the list of the possible values. Last value should be NULL.
 * @return \e int, index of the value in the list of options. In case var is not defined the default value is 0
 * @see hit_envYesNo(), hit_envInteger()
 */
int hit_envOptions( const char *var, const char **options );

/**
 * Return the value of an environment variable with an integer value
 * 
 * @param var \e const char *, string with the name of the environment variable
 * @param default_value \e int, default value to return in case that var is not defined or it is not an integer value
 * @return \e int, value of the variable, or default value in case or errors.
 * @see hit_envYesNo(), hit_envOptions()
 */
int hit_envInteger( const char *var, int default_value );

#ifdef __cplusplus
	}
#endif

#endif // hit_env.h
