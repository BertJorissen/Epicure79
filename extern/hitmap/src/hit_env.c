/**
 * Hitmap library
 * Reading environment variables for optional runtime features related to input/output
 *
 * @file hit_env.c
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

#include <hit_env.h>
#include <hit_com.h>

/* Return the value of an environment variable with only yes/no values. Default yes */
int hit_envYesNo( const char *var ) {
	const char *str_def = "yes";
	char *env_var = getenv( var );
	if ( env_var != NULL ) {
		if ( !strcmp( env_var, "y" ) ) return 1;
		if ( !strcmp( env_var, "yes" ) ) return 1;
		if ( !strcmp( env_var, "n" ) ) return 0;
		if ( !strcmp( env_var, "no" ) ) return 0;
		if ( hit_Rank == 0 ) hit_warn( var, "environment variable has an unknown value, using default value:", str_def );
	}
	else if ( hit_Rank == 0 ) hit_warn( var, "environment variable is not defined, using default value:", str_def );
	return 1;
}

/* Return the value of an environment variable with only yes/no values. Default no */
int hit_envNoYes( const char *var ) {
	const char *str_def = "no";
	char *env_var = getenv( var );
	if ( env_var != NULL ) {
		if ( !strcmp( env_var, "y" ) ) return 1;
		if ( !strcmp( env_var, "yes" ) ) return 1;
		if ( !strcmp( env_var, "n" ) ) return 0;
		if ( !strcmp( env_var, "no" ) ) return 0;
		if ( hit_Rank == 0 ) hit_warn( var, "environment variable has an unknown value, using default value:", str_def );
	}
	else if ( hit_Rank == 0 ) hit_warn( var, "environment variable is not defined, using default value:", str_def );
	return 0;
}

/* Deprecated */
/* Return the value of an environment variable with only yes/no values, specifying a default value */
int hit_envYesNoDefault( const char *var, int default_value ) {
	const char *str_def;
	if ( default_value ) str_def = "yes";
	else str_def = "no";

	char *env_var = getenv( var );
	if ( env_var != NULL ) {
		if ( !strcmp( env_var, "y" ) ) return 1;
		if ( !strcmp( env_var, "yes" ) ) return 1;
		if ( !strcmp( env_var, "n" ) ) return 0;
		if ( !strcmp( env_var, "no" ) ) return 0;
		if ( hit_Rank == 0 ) hit_warn( var, "environment variable has an unknown value, using default value:", str_def );
	}
	else if ( hit_Rank == 0 ) hit_warn( var, "environment variable is not defined, using default value:", str_def );
	return default_value;
}


/* Return the value of an environment variable from a list of posible values */
int hit_envOptions( const char *var, const char **options ) {
	char *env_var = getenv( var );
	if ( env_var != NULL ) {
		int ind;
		for (ind=0; options[ind] != NULL; ind++) 
			if ( !strcmp( env_var, options[ind] ) ) return ind;

		if ( options[ind] == NULL ) 
			if ( hit_Rank == 0 ) hit_warn( var, "environment variable has an unknown value, using default value:", options[0] );
	}
	else if ( hit_Rank == 0 ) hit_warn( var, "environment variable is not defined, using default value:", options[0] );
	return 0;
}


/* Return the value of an environment variable with an integer value */
int hit_envInteger( const char *var, int default_value ) {
	char *env_var = getenv( var );
	if ( env_var == NULL ) {
		if ( hit_Rank == 0 ) hit_warnInt( var, "environment variable is not defined, using default value:", default_value );
		return default_value;
	}
	int value;
	int ok = sscanf( env_var, "%d", &value );
	if ( ok != 1 ) {
		if ( hit_Rank == 0 ) hit_warnInt( var, "environment variable has a non-integer value, using default value:", default_value );
		return default_value;
	}
	return value;
}

