/**
 * Hierarchical tiling of multi-dimensional dense array.
 * Tool to convert formats of Hitmap files
 *
 * @file hitmap_files.c
 * @version 2.0
 * @author Arturo Gonzalez-Escribano
 * @date Aug 2023
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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<hitmap2.h>

#define	DEFAULT_FORMAT_SIZE_1	10
#define	DEFAULT_FORMAT_SIZE_2	0

hit_tileNewType( char );
hit_tileNewType( short );
hit_tileNewType( int );
hit_tileNewType( long );
hit_tileNewType( float );
hit_tileNewType( double );

char *program_name;

void print_usage() {
	fprintf( stderr, "Usage: %s [-f] <in_file_name> <out_file_name> <txt|bin> [ <format_size> <format_dec> ]\n\n", program_name );
	fprintf( stderr, "\t-f                Force to write ouput without warnings even if input and output would be the same\n" );
	fprintf( stderr, "\t<in_file_name>    Input file name: Array file or tile file with any rank\n" );
	fprintf( stderr, "\t<out_file_name>   Output file name (without extension suffix): Array file with the result\n" );
	fprintf( stderr, "\t<txt|bin>         Ouput format: text or binary\n" );
	fprintf( stderr, "\t<format_size>     Only for text output: Number of characters (default %d)\n", DEFAULT_FORMAT_SIZE_1 );
	fprintf( stderr, "\t<format_dec>      Only for text output: Number of decimal digits (default %d). Ignored for integer numbers\n", DEFAULT_FORMAT_SIZE_2 );
	fprintf( stderr, "\n" );
	exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] ) {
	/* 0. Initialize Hitmap */
	hit_comInit( &argc, &argv );

	/* 1. Read arguments */
	program_name = argv[0];

	int force = 0;
	if ( argc < 2 ) print_usage();
	if ( !strcmp( argv[1], "-f" ) ) force = 1;
	if ( argc < force+4 ) print_usage();

	const char *file_name = argv[force+1];
	const char *out_file_name = argv[force+2];

	int output_format;
	if ( ! strcmp(argv[force+3],"bin") ) output_format = HIT_FILE_BINARY;
	else if ( ! strcmp(argv[force+3],"txt") ) output_format = HIT_FILE_TEXT;
	else {
		fprintf( stderr, "\nError: Unknown output_format -- txt or bin\n\n" );
		print_usage();
	}

	int formatSize1 = DEFAULT_FORMAT_SIZE_1;
	int formatSize2 = DEFAULT_FORMAT_SIZE_2;
	if ( output_format == HIT_FILE_TEXT && argc > force+4 ) {
		int ok = sscanf( argv[force+4], "%d", &formatSize1 );
		if ( ok != 1 || formatSize1 < 1 ) {
			fprintf( stderr, "\nError: Incorrect format size: %s\n\n", argv[force+4] );
			print_usage();
		}
	}
	if ( output_format == HIT_FILE_TEXT && argc > force+5 ) {
		int ok = sscanf( argv[force+5], "%d", &formatSize2 );
		if ( ok != 1 || formatSize2 < 0 ) {
			fprintf( stderr, "\nError: Incorrect number of decimals: %s\n\n", argv[force+5] );
			print_usage();
		}
		if ( formatSize2 > 0 && formatSize2 > formatSize1 - 2 ) {
			fprintf( stderr, "\nError: When using decimals the format size should be at least the number of decimals plus two\n\n" );
			print_usage();
		}
	}
	if ( output_format == HIT_FILE_BINARY && argc > force+4) {
		fprintf( stderr, "\nError: More arguments than expected for a binary format output\n\n" );
		print_usage();
	}

	/* 2. Header metadata */
	/* 2.1. Read header */
	HitTile_FileHeaderInfo info = hit_tileFileReadHeader( file_name );

	#ifdef DEBUG
	printf("\n==== Hitmap_files ====\n");
	printf("\tfileFormat:\t%d\n", info.fileFormat);
	printf("\ttileMode:\t%d\n", info.tileMode);
	printf("\ttype:\t%d\n", info.type);
	printf("\ttypeSize:\t%ld\n", info.typeSize);
	printf("\trank:\t%d\n", info.rank);
	printf("\tnprocs:\t%d\n", info.nprocs);
	printf("\tnum_dims:\t%d\n", info.num_dims);
	printf("\tcards:\t%d, %d\n", info.cards[0], info.cards[1]);
	for (int d=0; d<infor.num_dims; d++)
		printf(" (%d,%d,%d)",
			hit_shapeSig( info.tileShape, d ).begin,
			hit_shapeSig( info.tileShape, d ).end,
			hit_shapeSig( info.tileShape, d ).stride
			);
	printf("\n\n");
	#endif 

	/* 2.2. Check and avoid foo conversions */
	if ( ! force ) {
		if ( output_format == HIT_FILE_BINARY && info.fileFormat == HIT_FILE_BINARY && info.tileMode == HIT_FILE_ARRAY ) {
			fprintf( stderr, "\nError: Not needed conversion from binary to binary array format\n\n" );
			exit( EXIT_FAILURE );
		}
		if ( output_format == HIT_FILE_TEXT
			&& info.fileFormat == HIT_FILE_TEXT 
			&& info.tileMode == HIT_FILE_ARRAY
			&& info.formatSize1 == formatSize1
			&& info.formatSize2 == formatSize2
			) {
			fprintf( stderr, "\nError: Not needed conversion from text to text array format: %d.%d\n\n", formatSize1, formatSize2 );
			exit( EXIT_FAILURE );
		}
	}

	/* 2.3. Get file name prefix and suffix */
	char prefix[ strlen( file_name ) + 1 ];
	char suffix[ strlen( file_name ) + 1 ];
	prefix[0] = '\0';
	suffix[0] = '\0';
	if ( info.tileMode == HIT_FILE_TILE ) {
		char file_name_copy[ strlen( file_name ) + 1 ];
		strcpy( file_name_copy, file_name );
		char *separator = strrchr( file_name_copy, (int)'.' );
		if ( separator == NULL ) strcpy( prefix, file_name );
		else {
			*separator = '\0';
			strcpy( prefix, file_name_copy );
			strcpy( suffix, separator+1 );
		}
	} else strcpy( prefix, file_name );


	/* 3. Allocate and initialize matrix */
	HitTile_char tile_char = HIT_TILE_NULL_STATIC;
	HitTile_short tile_short = HIT_TILE_NULL_STATIC;
	HitTile_int tile_int = HIT_TILE_NULL_STATIC;
	HitTile_long tile_long = HIT_TILE_NULL_STATIC;
	HitTile_float tile_float = HIT_TILE_NULL_STATIC;
	HitTile_double tile_double = HIT_TILE_NULL_STATIC;

	HitShape shp = HIT_SHAPE_WHOLE_STATIC;
	hit_shapeDimsSet( shp, info.num_dims );
	for (int dim=0; dim<info.num_dims; dim++) 
		hit_shapeSig(shp,dim) = hit_sigStd( info.cards[dim] );

	HitTile *p_array = NULL;
	switch( info.type ) {
	case HIT_FILE_CHAR:
		p_array = (HitTile *)&tile_char;
		hit_tileDomainShapeAlloc( p_array, char, shp );
		break;
	case HIT_FILE_SHORT:
		p_array = (HitTile *)&tile_short;
		hit_tileDomainShapeAlloc( p_array, short, shp );
		break;
	case HIT_FILE_INT:
		p_array = (HitTile *)&tile_int;
		hit_tileDomainShapeAlloc( p_array, int, shp );
		break;
	case HIT_FILE_LONG:
		p_array = (HitTile *)&tile_long;
		hit_tileDomainShapeAlloc( p_array, long, shp );
		break;
	case HIT_FILE_FLOAT:
		p_array = (HitTile *)&tile_float;
		hit_tileDomainShapeAlloc( p_array, float, shp );
		break;
	case HIT_FILE_DOUBLE:
		p_array = (HitTile *)&tile_double;
		hit_tileDomainShapeAlloc( p_array, double, shp );
		break;
	}


	/* 4. Read data */
	/* 4.1. Read array */
	if ( info.tileMode == HIT_FILE_ARRAY ) {
		hit_tileFileReadOptions( p_array, prefix, suffix, -1, info.fileFormat, HIT_FILE_ARRAY, HIT_FILE_HEADER, info.type, info.formatSize1, info.formatSize2 );
	}
	/* 4.2. Read tiles */
	else {
		/* 4.2.2. READ TILES ON ARRAY */
		for( int proc = 0; proc < info.nprocs; proc++ ) {
			hit_tileFileReadOptions( p_array, prefix, "", proc, info.fileFormat, HIT_FILE_TILE_SELECT, HIT_FILE_HEADER, info.type, info.formatSize1, info.formatSize2 );
		}
	}

	/* 5. Write output */	
	hit_tileFileWriteOptions( p_array, out_file_name, NULL, -1, output_format, HIT_FILE_ARRAY, HIT_FILE_HEADER, info.type, formatSize1, formatSize2 );

	/* 7. End */
	hit_comFinalize();
	return EXIT_SUCCESS;
}

