/**
 * Hierarchical tiling of multi-dimensional dense array.
 * Reading/Writing dense tiles with MPI File functionalities
 *
 * @file hit_tileFile.c
 * @version 1.4
 * @author Arturo Gonzalez-Escribano
 * @date Nov 2012, May 2018, Aug 2023
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

#include <stdio.h>
#include <unistd.h>
#include <hit_tileFile.h>
#include <hit_env.h>
#include <hit_com.h>
#include <hit_error.h>

/* Hit FUNCTION TO OPEN FILE AND READ FILE HEADER INFO */
HitTile_FileHeaderInfo hit_tileFileReadHeader( const char *fileName ) {
	FILE *fichSh = fopen( fileName, "r" );
	if ( fichSh == NULL )
		hit_err( "Impossible to open file for reading", fileName, "" ); 

	HitTile_FileHeaderInfo info = hit_tileFileStreamReadHeader( fichSh, fileName );

	fclose( fichSh );

	return info;
}


/* Hit FUNCTION TO READ FILE HEADER INFO FROM STREAM */
HitTile_FileHeaderInfo hit_tileFileStreamReadHeader( FILE *fichSh, const char *fileName ) {
	HitTile_FileHeaderInfo	info;
	size_t header_size = 17;
	int ok;

	/* 1. MAGIC */
	char c_tileMode[6], c_fileFormat[4];
	ok = fscanf( fichSh, "Hitmap %5s %3s\n", c_tileMode, c_fileFormat );
	if ( ok != 2 ) 
		hit_err( "Wrong magic characters in reading file", fileName, "" ); 
	if ( strcmp( c_tileMode, "Tiles" ) && strcmp( c_tileMode, "Array" ) )
		hit_err( "Wrong array/tiles mode in file header", fileName, "" );
	if ( !strcmp( c_tileMode, "Array" ) ) info.tileMode = HIT_FILE_ARRAY;
	else info.tileMode = HIT_FILE_TILE;

	if ( strcmp( c_fileFormat, "BIN" ) && strcmp( c_fileFormat, "TXT" ) )
		hit_err( "Wrong bin/txt mode in file header", fileName, "" );

	if ( !strcmp( c_fileFormat, "BIN" ) ) info.fileFormat = HIT_FILE_BINARY;
	else info.fileFormat = HIT_FILE_TEXT;

	if ( !strcmp( c_fileFormat, "TXT" ) ) {
		int s1, s2;
		ok = fscanf( fichSh, "%2d %2d\n", &s1, &s2 );
		if ( ok != 2 )
			hit_err( "Wrong format description in text file", fileName, "" );
		info.formatSize1 = s1;
		info.formatSize2 = s2;
		header_size += 6;
	}

	/* 2. TYPE */
	char str_type[7];
	ok = fscanf( fichSh, "%6s %10ld\n", str_type, &info.typeSize );
	if ( ok != 2)
		hit_err( "Wrong type description format in file", fileName, "" );
	if ( strcmp( str_type, "char" )
		&& strcmp( str_type, "short" )
		&& strcmp( str_type, "int" )
		&& strcmp( str_type, "long" )
		&& strcmp( str_type, "float" )
		&& strcmp( str_type, "doubl" )
		&& strcmp( str_type, "unk" ) )
		hit_err( "Wrong type name in file", fileName, "" );

	if ( ! strcmp( str_type, "char" ) ) info.type = HIT_FILE_CHAR;
	else if ( ! strcmp( str_type, "short" ) ) info.type = HIT_FILE_SHORT;
	else if ( ! strcmp( str_type, "int" ) ) info.type = HIT_FILE_INT;
	else if ( ! strcmp( str_type, "long" ) ) info.type = HIT_FILE_LONG;
	else if ( ! strcmp( str_type, "float" ) ) info.type = HIT_FILE_FLOAT;
	else if ( ! strcmp( str_type, "double" ) ) info.type = HIT_FILE_DOUBLE;
	else if ( ! strcmp( str_type, "unk" ) ) info.type = HIT_FILE_TYPE_UNKNOWN;
	else hit_err( "Unknown type description in file", fileName, "" );

	header_size += 18;

	/* 3. TILE MODE: RANK AND PROCESSES */
	if ( info.tileMode != HIT_FILE_ARRAY ) {
		int rank = 0, procs = 0;
		if ( info.fileFormat == HIT_FILE_BINARY ) {
			fread( &rank, sizeof(hit_Rank), 1, fichSh );
			fread( &procs, sizeof(hit_NProcs), 1, fichSh );
			header_size += sizeof( hit_Rank ) + sizeof( hit_NProcs );
		}
		else {
			ok = fscanf( fichSh, "%10d %10d\n", &rank, &procs );
			if ( ok != 2 ) 
				hit_err( "Wrong rank/procs specification reading tiles file", fileName, "" );
			header_size += 22;
		}
		info.rank = rank;
		info.nprocs = procs;
	}
		
	/* 4. ARRAY DIMS AND CARDINALITIES */
	int num_dims = 0;
	if ( info.fileFormat == HIT_FILE_BINARY ) {
		fread( &num_dims, sizeof( num_dims ), 1, fichSh );
		for (int dim=0; dim<num_dims; dim++ ) {
			info.cards[dim] = 0;
			fread( &info.cards[dim], sizeof( info.cards[dim] ), 1, fichSh );
		}
		header_size += sizeof( num_dims ) + (size_t)num_dims * sizeof( info.cards[0] );
	}
	else {
		fscanf( fichSh, "%1d", &num_dims );
		for (int dim=0; dim<num_dims; dim++ ) {
			info.cards[dim] = 0;
			ok = fscanf( fichSh, " %20d", &info.cards[dim] );
			if ( ok != 1 )
				hit_err( "Reading array cardinalities in file", fileName, "" );
		}
		fscanf( fichSh, "\n" );
		header_size += (size_t)(2 + 21 * num_dims);
	}
	info.num_dims = num_dims;

	/* 5. TILE MODE: TILE SHAPE */
	if ( info.tileMode != HIT_FILE_ARRAY ) {
		HitShape sh_read = { HIT_SIG_SHAPE, { { 0, { } } } };
		hit_shapeDimsSet( sh_read, num_dims );
		if ( info.fileFormat == HIT_FILE_BINARY ) {
			for (int dim=0; dim<num_dims; dim++ ) {
				fread( &(hit_shapeSig(sh_read,dim).begin), sizeof( hit_shapeSig(sh_read,dim).begin ), 1, fichSh );
				fread( &(hit_shapeSig(sh_read,dim).end), sizeof( hit_shapeSig(sh_read,dim).end ), 1, fichSh );
				fread( &(hit_shapeSig(sh_read,dim).stride), sizeof( hit_shapeSig(sh_read,dim).stride ), 1, fichSh );
				header_size += (size_t)num_dims *
					( sizeof( hit_shapeSig(sh_read,dim).begin ) +
					sizeof( hit_shapeSig(sh_read,dim).end ) +
					sizeof( hit_shapeSig(sh_read,dim).stride )
					);
			}
		}
		else {
			for (int dim=0; dim<num_dims; dim++ ) {
				fscanf( fichSh, " %20d", &(hit_shapeSig(sh_read,dim).begin) );
				fscanf( fichSh, " %20d", &(hit_shapeSig(sh_read,dim).end) );
				fscanf( fichSh, " %20d", &(hit_shapeSig(sh_read,dim).stride) );
			}
			fscanf( fichSh, "\n" );
			header_size += (size_t)( 1 + 63 * (size_t)num_dims );
		}
		info.tileShape = sh_read;
	}
	else info.tileShape = HIT_SHAPE_NULL;

	info.header_size = header_size;
	return info;
}

/* Hit FUNCTION TO WRITE FILE HEADER INFO 
 * Return: Header size
 */
size_t hit_tileFileStreamWriteHeader( FILE *fichSh, HitTile_FileHeaderInfo info ) {
	size_t header_size = 17;

	/* 1. MAGIC */
	if ( info.tileMode == HIT_FILE_ARRAY )
		fprintf( fichSh, "Hitmap Array" );
	else
		fprintf( fichSh, "Hitmap Tiles" );

	if ( info.fileFormat == HIT_FILE_BINARY )
		fprintf( fichSh, " BIN\n" );
	else {
		fprintf( fichSh, " TXT\n%2d %2d\n", info.formatSize1, info.formatSize2 );
		header_size += 6;
	}

	/* 2. TYPE */
	switch ( info.type ) {
	case HIT_FILE_CHAR: fprintf( fichSh, "char  " ); break;
	case HIT_FILE_SHORT: fprintf( fichSh, "short " ); break;
	case HIT_FILE_INT: fprintf( fichSh, "int   " ); break;
	case HIT_FILE_LONG: fprintf( fichSh, "long  " ); break;
	case HIT_FILE_FLOAT: fprintf( fichSh, "float " ); break;
	case HIT_FILE_DOUBLE: fprintf( fichSh, "double" ); break;
	case HIT_FILE_TYPE_UNKNOWN: fprintf( fichSh, "unk   " ); break;
	}
	fprintf( fichSh, " %10ld\n", info.typeSize );
	header_size += 18;

	/* 3. TILE MODE: RANK AND PROCESSES */
	if ( info.tileMode != HIT_FILE_ARRAY ) {
		if ( info.fileFormat == HIT_FILE_BINARY ) {
			fwrite( &hit_Rank, sizeof(hit_Rank), 1, fichSh );
			fwrite( &hit_NProcs, sizeof(hit_NProcs), 1, fichSh );
			header_size += sizeof( hit_Rank ) + sizeof( hit_NProcs );
		}
		else {
			fprintf( fichSh, "%10d", hit_Rank );
			fprintf( fichSh, " %10d\n", hit_NProcs );
			header_size += 22;
		}
	}
		
	/* 4. ARRAY DIMS AND CARDINALITIES */
	if ( info.fileFormat == HIT_FILE_BINARY ) {
		fwrite( &info.num_dims, sizeof( info.num_dims ), 1, fichSh );
		for (int dim=0; dim<info.num_dims; dim++ )
			fwrite( &info.cards[dim], sizeof( info.cards[dim] ), 1, fichSh );
		header_size += sizeof( info.num_dims ) + (size_t)info.num_dims * sizeof( info.cards[0] );
	}
	else {
		fprintf( fichSh, "%1d", info.num_dims );
		for (int dim=0; dim<info.num_dims; dim++ )
			fprintf( fichSh, " %20d", info.cards[dim] );
		fprintf( fichSh, "\n" );
		header_size += (size_t)(2 + 21 * info.num_dims );
	}

	/* 5. TILE MODE: TILE SHAPE */
	if ( info.tileMode != HIT_FILE_ARRAY ) {
		if ( info.fileFormat == HIT_FILE_BINARY ) {
			for (int dim=0; dim<info.num_dims; dim++ ) {
				fwrite( &(hit_shapeSig(info.tileShape,dim).begin), sizeof( hit_shapeSig(info.tileShape,dim).begin ), 1, fichSh );
				fwrite( &(hit_shapeSig(info.tileShape,dim).end), sizeof( hit_shapeSig(info.tileShape,dim).end ), 1, fichSh );
				fwrite( &(hit_shapeSig(info.tileShape,dim).stride), sizeof( hit_shapeSig(info.tileShape,dim).stride ), 1, fichSh );
				header_size += (size_t)info.num_dims *
					( sizeof( hit_shapeSig(info.tileShape,dim).begin ) +
					sizeof( hit_shapeSig(info.tileShape,dim).end ) +
					sizeof( hit_shapeSig(info.tileShape,dim).stride )
					);
			}
		}
		else {
			for (int dim=0; dim<info.num_dims; dim++ ) {
				fprintf( fichSh, " %20d", hit_shapeSig(info.tileShape,dim).begin );
				fprintf( fichSh, " %20d", hit_shapeSig(info.tileShape,dim).end );
				fprintf( fichSh, " %20d", hit_shapeSig(info.tileShape,dim).stride );
			}
			fprintf( fichSh, "\n" );
			header_size += (size_t)( 1 + 63 * (size_t)info.num_dims );
		}
	}

	return header_size;
}


/* Hit FUNCTION TO READ/WRITE TILES FROM/TO BINARY OR TEXT FILES USING A GIVEN FORMAT */
int hit_tileFileInternal(
		void * varP, 
		int fileFormat, 
		int fileMode, 
		int tileMode, 
		int fileHeader, 
		int type, 
		int formatSize1,
		int formatSize2, 
		int fileRank,
		const char *fileNamePrefix, 
		const char *fileNameSuffix, 
		const char *debugVarName, 
		const char *debugCodeFile, 
		int debugCodeLine
		) {
  	
	HitTile *var = (HitTile *)varP;

	/* 0. READ RUNTIME ENVIRONMENT OPTIONS */
	// PRIVATE CONSTANTS
	#define HIT_FILE_NOT_SET	-1000
	#define HIT_FILE_SHARED	0
	#define HIT_FILE_MPI	1
	// PRIVATE STATIC VARIABLES
	static int io_filesystem = HIT_FILE_NOT_SET;
	static int io_fileFormat = HIT_FILE_NOT_SET;
	static int io_tileMode = HIT_FILE_NOT_SET;
	static int io_fileHeader = HIT_FILE_NOT_SET;
	static int io_formatSize1 = HIT_FILE_NOT_SET;
	static int io_formatSize2 = HIT_FILE_NOT_SET;
	

	// OPTION: SELECT THE FILE BACKEND (MPI vs. SHARED FILE SYSTEM USING C STANDARD INTERFACE)
	const char *options_filesystem[] = { "shared", "mpi", NULL };
	if ( io_filesystem == HIT_FILE_NOT_SET )
		io_filesystem = hit_envOptions( "HIT_FILESYSTEM", options_filesystem );
	
	// OPTION: BINARY/TEXT FORMAT
	if ( fileFormat == HIT_FILE_RUNTIME ) {
		if ( io_fileFormat == HIT_FILE_NOT_SET ) {
			io_fileFormat = hit_envNoYes( "HIT_FILE_TEXT" );
		}
		fileFormat = io_fileFormat;
	}

	// OPTION: COORDINATE SYSTEM
	const char *options_tileMode[] = { "array", "tile", "tile_select", NULL };
	if ( tileMode == HIT_FILE_RUNTIME ) {
		if ( io_tileMode == HIT_FILE_NOT_SET ) {
			io_tileMode = hit_envOptions( "HIT_FILE_COORD", options_tileMode );
		}
		tileMode = io_tileMode;
	}

	// OPTION: ADDING HEADERS WITH THE ARRAY/TILE/FORMAT DESCRIPTION IN FILES
	if ( fileHeader == HIT_FILE_RUNTIME ) {
		if ( io_fileHeader == HIT_FILE_NOT_SET ) {
			io_fileHeader = hit_envYesNo( "HIT_FILE_HEADER" );
		}
		fileHeader = io_fileHeader;
	}

	// OPTION: TEXT FILES DATA FORMAT (SIZE)
	if ( formatSize1 == HIT_FILE_RUNTIME ) {
		if ( io_formatSize1 == HIT_FILE_NOT_SET ) {
			io_formatSize1 = hit_envInteger( "HIT_FILE_TXT_SIZE", 12 );
		}
		formatSize1 = io_formatSize1;
	}

	// OPTION: TEXT FILES DATA FORMAT (DECIMALS)
	if ( formatSize2 == HIT_FILE_RUNTIME ) {
		if ( io_formatSize2 == HIT_FILE_NOT_SET ) {
			io_formatSize2 = hit_envInteger( "HIT_FILE_TXT_DECIMALS", 6 );
		}
		formatSize2 = io_formatSize2;
	}

	// OPTION: FILE RANK
	if ( fileRank == HIT_FILE_RUNTIME ) fileRank = hit_Rank;

	// INTEGER TYPES: SET DECIMALS TO 0 IN HEADERS OF TEXT FILES FOR INTEGER TYPES
	switch ( type ) {
		case HIT_FILE_CHAR:
		case HIT_FILE_SHORT:
		case HIT_FILE_INT:
		case HIT_FILE_LONG: formatSize2 = 0;
	}

	// OPTIONS INCOMPATIBILITY CHECKS
	if ( fileMode == HIT_FILE_WRITE && tileMode == HIT_FILE_TILE_SELECT )
		hit_errInternal(__FUNCTION__, "Tile select mode is only defined for read operations", "", debugCodeFile, debugCodeLine); 

	if ( formatSize1 < formatSize2+1 ) {
		char formatOptionsStr[50];
		sprintf( formatOptionsStr, "(Txt size: %d, Decimals: %d)", formatSize1, formatSize2 );
		hit_errInternal(__FUNCTION__, "Text format sizes, too many decimals for the field size", formatOptionsStr, debugCodeFile, debugCodeLine); 
	}

	// BUILD COMPLETE FILE NAME
	int file_name_length = (int)strlen( fileNamePrefix );
	if ( fileNameSuffix != NULL ) file_name_length += (int)strlen( fileNameSuffix );
	char fileName[ file_name_length + 11 ]; // MPI ranks are int (max. 10 decimal characters)

	/* ADD DATA FORMAT EXTENSION TO THE FILE NAME IF REQUIRED */
	if ( fileNameSuffix == NULL ) {
		if ( fileFormat == HIT_FILE_BINARY ) {
			if ( fileHeader == HIT_FILE_HEADER ) fileNameSuffix = ".hdat";
			else fileNameSuffix = ".dat";
		}
		else {
			if ( fileHeader == HIT_FILE_HEADER ) fileNameSuffix = ".htxt";
			else fileNameSuffix = ".txt";
		}
	}

	/* ADD hit_Rank TO THE FILE NAME IF TILES MODE IS SELECTED */
	if ( tileMode != HIT_FILE_ARRAY ) 
		sprintf( fileName, "%s%s.tile%d", fileNamePrefix, fileNameSuffix, fileRank );
	else
		sprintf( fileName, "%s%s", fileNamePrefix, fileNameSuffix );


	/* 1. CHECK FILE MODE PARAMETER.
	 * 		WRITE MODE: ENSURE THAT THE FILE IS CREATED 
	 * 		(IT AVOIDS CONCURRENCY PROBLEMS WHEN OPENING IN OVER-WRITE MODE) */
	if ( io_filesystem == HIT_FILE_SHARED ) {
		//char command[1024];
		int ok;
		switch ( fileMode ) {
			case HIT_FILE_READ: break;

			case HIT_FILE_WRITE: 
				/* @arturo: 2023/07/21
				* Fix: Create file in case it does not exist in shared mode
				*/
				//strcpy( command, "touch " );
				//strcat( command, fileName );
				//system( command );
				ok = open( fileName, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR );
				if (ok >= 0 ) 
					close( ok );
				//else
				//	hit_errInternal(__FUNCTION__, "Creating-Opening file", fileName, debugCodeFile, debugCodeLine); 
				// TODO: Change COMM_WORLD by the communicator in the tile layout
				//MPI_Barrier( MPI_COMM_WORLD );
				break;

			default:
				hit_errInternal(__FUNCTION__, "Unknown file mode", debugVarName, debugCodeFile, debugCodeLine);
				break;
		}
	}

	/* @arturo Feb 2013: Open file before checking if the variable is empty to generate
	 * 			and empty file instead of skipping the file creation for empty variables. 
	 * 			This helps in tracing, debugging and scripting.
	 */
	/* 2. SKIP HIT_TILE_NULL VARIABLE AND VARIABLES WITH NO MEMORY */
	if ( var->memStatus == HIT_MS_NULL || var->memStatus == HIT_MS_NOMEM ) return 1;

	/* 3. DECLARE VARIABLES */
	int ind[HIT_MAXDIMS],i,j,indsh[HIT_MAXDIMS];
	size_t offset;
	long file_offset = 0;
	void *ptr = NULL;
	char patternRead[32];
	char patternWrite[32];

	/* 4. OPEN FILE FOR READ/WRITE */
	int ok;
	FILE *fichSh = NULL;
	MPI_File fich;

	if ( io_filesystem == HIT_FILE_SHARED ) {
		fichSh = fopen(fileName,"r+");
		if ( fichSh == NULL ) 
			hit_errInternal(__FUNCTION__, "Fail opening file", fileName, debugCodeFile, debugCodeLine); 
	}
	else {
		// TODO: Change COMM_WORLD by the communicator in the tile layout
		switch ( fileMode ) {
			case HIT_FILE_READ: 
						ok = MPI_File_open( MPI_COMM_WORLD, fileName, MPI_MODE_RDONLY, MPI_INFO_NULL, &fich );	
						if ( ok != MPI_SUCCESS ) hit_errInternal(__FUNCTION__, "Opening file", fileName, debugCodeFile, debugCodeLine); 
						break;

			case HIT_FILE_WRITE: 
						// Open and close to destroy previous file if it exists
						ok = MPI_File_open( MPI_COMM_WORLD, fileName, MPI_MODE_CREATE | MPI_MODE_DELETE_ON_CLOSE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fich );	
						if ( ok != MPI_SUCCESS ) hit_errInternal(__FUNCTION__, "Overwriting file", fileName, debugCodeFile, debugCodeLine); 
						ok = MPI_File_close( &fich );
						if ( ok != MPI_SUCCESS ) hit_errInternal(__FUNCTION__, "Overwriting file", fileName, debugCodeFile, debugCodeLine); 
						ok = MPI_File_open( MPI_COMM_WORLD, fileName, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fich );	
						if ( ok != MPI_SUCCESS ) hit_errInternal(__FUNCTION__, "Opening file", fileName, debugCodeFile, debugCodeLine); 
						// Deactivate atomic operations
						ok = MPI_File_set_atomicity(fich, 0);
						if ( ok != MPI_SUCCESS ) hit_errInternal(__FUNCTION__, "Deactivating atomicity of operations in file", fileName, debugCodeFile, debugCodeLine); 
						break;

			default:
				hit_errInternal(__FUNCTION__, "Unknown file mode", debugVarName, debugCodeFile, debugCodeLine);
				break;
		}
	}

	/* 5. BUILD FORMAT */
	if ( fileFormat != HIT_FILE_BINARY ) {
		switch ( type ) {
			case HIT_FILE_CHAR:
						sprintf( patternRead, "%%0%dc\n", formatSize1 );
						sprintf( patternWrite, "%%0%dc\n", formatSize1 );
						break;
			case HIT_FILE_SHORT:
						sprintf( patternRead, "%%0%dhd\n", formatSize1 );
						sprintf( patternWrite, "%%0%dhd\n", formatSize1 );
						break;
			case HIT_FILE_INT:
						sprintf( patternRead, "%%0%dd\n", formatSize1 );
						sprintf( patternWrite, "%%0%dd\n", formatSize1 );
						break;
			case HIT_FILE_LONG:
						sprintf( patternRead, "%%0%dld\n", formatSize1 );
						sprintf( patternWrite, "%%0%dld\n", formatSize1 );
						break;
			case HIT_FILE_FLOAT:
						sprintf( patternRead, "%%0%df\n", formatSize1 );
						sprintf( patternWrite, "%%0%d.%df\n", formatSize1, formatSize2 );
						break;
			case HIT_FILE_DOUBLE:
						sprintf( patternRead, "%%0%dlf\n", formatSize1 );
						sprintf( patternWrite, "%%0%d.%dlf\n", formatSize1, formatSize2 );
						break;
			default:
				hit_errInternal(__FUNCTION__, "Unknown data type for text mode", debugVarName, debugCodeFile, debugCodeLine); 
				break;
		}
	}

	/* 6. LOCATE THE ROOT TILE TO GET THE ORIG ACUM CARDS OF THE ARRAY COORDINATES 
	 * 		THEY ARE USED TO COMPUTE POSITIONS IN THE FILE WHEN USING ARRAY COORDINATES MODE
	 *
	 * 		THIS CARDS ARE DIFFERENT FROM CURRENT origAcumCard WHEN AN ANCESTOR OF THIS VARIABLE
	 * 		IS A SELECTION OF THE ARRAY WHICH ALLOCATED MEMORY BEFORE CREATING NEW SELECTIONS 
	 * 		OF IT 
	 *
	 * 		WARNING: IT DOES NOT WORK FOR ROOT ARRAYS THAT DO NOT START AT 0, OR HAVE INITIAL STRIDE
	 */
	HitTile *rootTile;
	for (rootTile = var; rootTile->ref != NULL; rootTile = rootTile->ref);

	/* Patch for BlockTiles (TO BE DEPRECATED) */
	for ( ; rootTile->ancestor != NULL; rootTile = rootTile->ancestor);
	//int *rootChildSize = rootTile->childSize;
	//int acumChildSizes[5] = { 0, 0, 0, 0, 0 };
	//acumChildSizes[ hit_shapeDims(var->shape) ] = 1;
	//for( i=hit_shapeDims(var->shape)-1; i>=0; i-- ) acumChildSizes[i] = acumChildSizes[i+1] * rootChildSize[i]; 
	for ( ; rootTile->ref != NULL; rootTile = rootTile->ref);
	/* END Patch for BlockTiles */

	/* Patch for Padded tiles, used in BlockTiles (TO BE DEPRECATED) */
	if ( var->unpadded != NULL ) var = var->unpadded;
	/* END Patch for Padded tiles */


	int *rootAcumCard = rootTile->origAcumCard;

	/* 7.1. READ/WRITE HEADER */
	size_t header_size = 0;
	if ( fileHeader == HIT_FILE_HEADER ) {
		if ( io_filesystem == HIT_FILE_MPI ) 
			hit_errInternal(__FUNCTION__, "File headers not yet supported for MPI filesystem", fileName, debugCodeFile, debugCodeLine); 

		HitTile_FileHeaderInfo info;
		if ( fileMode == HIT_FILE_WRITE ) {
			info.fileFormat = fileFormat;
			info.tileMode = tileMode;
			info.type = type;
			info.typeSize = var->baseExtent;
			info.formatSize1 = formatSize1;
			info.formatSize2 = formatSize2;
			info.rank = hit_Rank;
			info.nprocs = hit_NProcs;
			info.num_dims = hit_tileDims( *var );
			for (int dim=0; dim<hit_tileDims( *rootTile ); dim++) 
				info.cards[dim] = hit_tileDimCard( *rootTile, dim );
			info.tileShape = hit_tileShape( *var );
			header_size = hit_tileFileStreamWriteHeader( fichSh, info );
		}
		else {
			info = hit_tileFileStreamReadHeader( fichSh, fileName );

			#ifdef DEBUG
			printf("\nRead Header Info\n");
			printf("\tfileFormat:\t%d\n", info.fileFormat);
			printf("\ttileMode:\t%d\n", info.tileMode);
			printf("\ttype:\t%d\n", info.type);
			printf("\ttypeSize:\t%ld\n", info.typeSize);
			printf("\trank:\t%d\n", info.rank);
			printf("\tnprocs:\t%d\n", info.nprocs);
			printf("\tnum_dims:\t%d\n", info.num_dims);
			printf("\tcards:\t%d, %d\n", info.cards[0], info.cards[1]);
			printf("\tshape:\t" );
			for (int d=0; d<infor.num_dims; d++)
				printf(" (%d,%d,%d)",
					hit_shapeSig( info.tileShape, d ).begin,
					hit_shapeSig( info.tileShape, d ).end,
					hit_shapeSig( info.tileShape, d ).stride
					);
			printf("\n\n");
			#endif 

			/* CHECK MATCHING OF FUNCTION PARAMETERS AND HEADERS INFO */
			if ( fileFormat == HIT_FILE_TEXT && info.fileFormat != HIT_FILE_TEXT )
				hit_errInternal(__FUNCTION__, "Request to read in text mode from binary file", fileName, debugCodeFile, debugCodeLine); 
			if ( fileFormat == HIT_FILE_BINARY && info.fileFormat != HIT_FILE_BINARY )
				hit_errInternal(__FUNCTION__, "Request to read in binary mode from text file", fileName, debugCodeFile, debugCodeLine); 
			if ( tileMode == HIT_FILE_ARRAY && info.tileMode != HIT_FILE_ARRAY )
				hit_errInternal(__FUNCTION__, "Request to read array from tile file", fileName, debugCodeFile, debugCodeLine); 
			if ( tileMode != HIT_FILE_ARRAY && info.tileMode == HIT_FILE_ARRAY )
				hit_errInternal(__FUNCTION__, "Request to read tile from array file", fileName, debugCodeFile, debugCodeLine); 
			if ( fileFormat == HIT_FILE_TEXT ) {
				if ( info.formatSize1 != formatSize1 || info.formatSize2 != formatSize2 ) 
					hit_errInternal(__FUNCTION__, "Text format description in file differs from description required in parameters", fileName, debugCodeFile, debugCodeLine); 
			}
			for (int dim=0; dim<hit_tileDims( *rootTile ); dim++) 
				if ( info.cards[dim] != hit_tileDimCard( *rootTile, dim ) )

			if ( info.num_dims != hit_tileDims(*rootTile) )
				hit_errInternal(__FUNCTION__, "Number of array dimensions in file differs from parameter specification", fileName, debugCodeFile, debugCodeLine); 
			for (int dim=0; dim<info.num_dims; dim++ ) 
				if ( info.cards[dim] != hit_tileDimCard(*rootTile,dim) )
					hit_errInternal(__FUNCTION__, "Array cardinality in file differs from parameter specification", fileName, debugCodeFile, debugCodeLine); 

			if ( tileMode == HIT_FILE_TILE ) {  // Not for tile_select
				if ( info.rank != hit_Rank && info.nprocs != hit_NProcs ) 
					hit_errInternal(__FUNCTION__, "MPI rank and #procs differ reading tiles file", fileName, debugCodeFile, debugCodeLine); 
				if ( info.rank != hit_Rank ) 
					hit_errInternal(__FUNCTION__, "MPI rank differs reading tiles file", fileName, debugCodeFile, debugCodeLine); 
				if ( info.nprocs != hit_NProcs ) 
					hit_errInternal(__FUNCTION__, "MPI #procs differs reading tiles file", fileName, debugCodeFile, debugCodeLine); 
			}

			if ( tileMode == HIT_FILE_TILE ) {
				for (int dim=0; dim<hit_tileDims(*rootTile); dim++ ) {
					if ( hit_shapeSig(info.tileShape,dim).begin != hit_tileDimBegin(*var,dim) 
						|| hit_shapeSig(info.tileShape,dim).end != hit_tileDimEnd(*var,dim) 
						|| hit_shapeSig(info.tileShape,dim).stride != hit_tileDimStride(*var,dim) 
						)
						hit_errInternal(__FUNCTION__, "Tile signature in file differs from tile parameter while reading file", fileName, debugCodeFile, debugCodeLine); 
				}
			}
			header_size = info.header_size;

			/* MODE: TILE_SELECT, SELECT ON VAR THE TILE SHAPE IN THE FILE HEADER,
			 * AND USE THE SELECTION AS VAR FOR THE REST OF THE FUNCTION */
			if ( tileMode == HIT_FILE_TILE_SELECT ) {
				HitTile selection;
				hit_tileSelectArrayCoords( &selection, var, info.tileShape );
				var = &selection;
			}
		}
	}


	/* 8. INITIALIZATION OF FILE AND MEMORY LOCATION POINTERS/INDECES */
	/* 8.1. ARRAY MODE: INITIALISE indsh WITH THE SIGNATURE-BEGIN OF EACH DIMENSION */
	if ( tileMode == HIT_FILE_ARRAY ) {
		for( i=0; i<hit_shapeDims(var->shape); i++ ) 
			indsh[i] = hit_shapeSig(var->shape,i).begin;
	}
	/* 8.2. REAL OWNER VARIABLE: INITIALIZE POINTER AT BEGINNING OF DATA REGION */
	if ( var->memStatus == HIT_MS_OWNER ) ptr = var->data;
	/* 8.3. SELECTION VARIABLES: INITIALIZE INDECES TO LOCATE DATA IN MEMORY */
	else for( i=0; i<hit_shapeDims(var->shape); i++ ) ind[i] = 0;

	/* 9. FOR ALL ELEMENTS IN THE TILE */
	for( i=0; i<var->acumCard; i++ ) {
		/* 9.1. SELECTION VARIABLE: LOCATE NEXT ELEMENT IN MEMORY */
		if ( var->memStatus == HIT_MS_NOT_OWNER ) {
			offset = 0;
			for( j=0; j<hit_shapeDims(var->shape); j++) {
				offset += (size_t)ind[j] * (size_t)var->qstride[j] * (size_t)var->origAcumCard[j+1];
			}
			ptr = (char *)var->data + offset * (size_t)var->baseExtent;
		}

		if( io_filesystem == HIT_FILE_SHARED ) {
			/* 9.2. ARRAY MODE: COMPUTE OFFSET IN THE FILE AND RELOCATE FILE POINTER */
			if ( tileMode == HIT_FILE_ARRAY ) {
				file_offset = 0;
				for( j=0; j<hit_shapeDims(var->shape); j++) {
					// @arturo Aug 2023: Relative to root begin
					//file_offset += indsh[j] * rootAcumCard[j+1];
					file_offset += ( indsh[j] - hit_shapeSig(rootTile->shape,j).begin ) * rootAcumCard[j+1];
					/* Patch for BlockTiles (TO BE DEPRECATED) */
					//file_offset += indsh[j] * acumChildSizes[j+1] * rootAcumCard[j+1];
				}
				/* 9.2.1. BINARY FORMAT */
				if ( fileFormat == HIT_FILE_BINARY ) 
					ok = fseek( fichSh, (long)header_size + file_offset * (long)var->baseExtent, SEEK_SET );
				/* 9.2.2. TEXT FORMAT */
				else 
					ok = fseek( fichSh, (long)header_size + file_offset * (formatSize1 + 1), SEEK_SET );
					  
				/* 9.2.3. CHECK ERROR */
				if ( ok != 0 ) {
					fclose( fichSh ); 
					hit_errInternal(__FUNCTION__, "Positioning in file", fileName, debugCodeFile, debugCodeLine); 
				}
			}

			/* 9.3. TRY OPERATION: USE THE APROPRIATE DATA TYPE AND FORMAT */
			int result = 0;
			/* 9.3.1. BINARY FORMAT */
			if ( fileFormat == HIT_FILE_BINARY ) {
				switch ( fileMode ) {
				   case HIT_FILE_READ: result = (int)fread( ptr, var->baseExtent, 1, fichSh );
									   break;
				   case HIT_FILE_WRITE: result = (int)fwrite( ptr, var->baseExtent, 1, fichSh );
									   break;
				}
			}
			/* 9.3.2. TEXT FORMAT */
			else {
				switch ( fileMode ) {
				   case HIT_FILE_READ:
						switch ( type ) {
							case HIT_FILE_CHAR: result = fscanf( fichSh, patternRead, (char*)(ptr) );
										  break;
							case HIT_FILE_SHORT: result = fscanf( fichSh, patternRead, (short*)(ptr) );
										  break;
							case HIT_FILE_INT: result = fscanf( fichSh, patternRead, (int*)(ptr) );
										  break;
							case HIT_FILE_LONG: result = fscanf( fichSh, patternRead, (long*)(ptr) );
										  break;
							case HIT_FILE_FLOAT: result = fscanf( fichSh, patternRead, (float*)(ptr) );
										  break;
							case HIT_FILE_DOUBLE: result = fscanf( fichSh, patternRead, (double*)(ptr) );
										  break;
						}
						break;

				   case HIT_FILE_WRITE:
						switch ( type ) {
							case HIT_FILE_CHAR: result = fprintf( fichSh, patternWrite, *(char*)(ptr) );
										  break;
							case HIT_FILE_SHORT: result = fprintf( fichSh, patternWrite, *(short*)(ptr) );
										  break;
							case HIT_FILE_INT: result = fprintf( fichSh, patternWrite, *(int*)(ptr) );
										  break;
							case HIT_FILE_LONG: result = fprintf( fichSh, patternWrite, *(long*)(ptr) );
										  break;
							case HIT_FILE_FLOAT: result = fprintf( fichSh, patternWrite, *(float*)(ptr) );
										  break;
							case HIT_FILE_DOUBLE: result = fprintf( fichSh, patternWrite, *(double*)(ptr) );
										  break;
						}
						break;
				}
			}

			/* 9.4. CHECK OPERATION RESULT */
			if ( fileMode == HIT_FILE_READ ) {
				if ( result != 1 ) {
					fclose( fichSh ); 
					if ( fileFormat == HIT_FILE_BINARY ) {
						hit_errInternal(__FUNCTION__, "Reading binary data in file", fileName, debugCodeFile, debugCodeLine); 
					}
					else {
						hit_errInternal(__FUNCTION__, "Reading text data in file", fileName, debugCodeFile, debugCodeLine); 
					}
				}
			}
			else if ( fileFormat == HIT_FILE_BINARY ) {
				if ( result != 1 ) {
					fclose( fichSh ); 
					hit_errInternal(__FUNCTION__, "Writing binary data in file", fileName, debugCodeFile, debugCodeLine); 
				}
			}
			else if ( result != formatSize1+1 ) {
				fclose( fichSh ); 
				hit_errInternal(__FUNCTION__, "Writing text data in file", fileName, debugCodeFile, debugCodeLine); 
			}
		}
		// MPI MODE
		else {
			/* 9.2. ARRAY MODE: COMPUTE OFFSET IN THE FILE */
			if ( tileMode == HIT_FILE_ARRAY ) {
				file_offset = 0;
				for( j=0; j<hit_shapeDims(var->shape); j++) {
					// @arturo Aug 2023: Relative to root begin
					//file_offset += indsh[j] * rootAcumCard[j+1];
					file_offset += ( indsh[j] - hit_shapeSig(rootTile->shape,j).begin ) * rootAcumCard[j+1];
					/* Patch for BlockTiles (TO BE DEPRECATED) */
					//file_offset += indsh[j] * acumChildSizes[j+1] * rootAcumCard[j+1];
				}
			}

			/* 9.3. TRY OPERATION: USE THE APROPRIATE DATA TYPE AND FORMAT */
			int result = 0;
			/* 9.3.1. BINARY FORMAT */
			if ( fileFormat == HIT_FILE_BINARY ) {
				MPI_Status stat;
				switch ( fileMode ) {
				   case HIT_FILE_READ: 
					if ( tileMode == HIT_FILE_ARRAY )
						result = MPI_File_read_at( fich, (long)header_size + file_offset * (long)var->baseExtent, ptr, (int)(var->baseExtent), MPI_BYTE, &stat );
					else
						result = MPI_File_read( fich, ptr, (int)(var->baseExtent), MPI_BYTE, &stat );

					break;
				   case HIT_FILE_WRITE: 
					if ( tileMode == HIT_FILE_ARRAY )
						result = MPI_File_write_at( fich, (long)header_size + file_offset * (long)var->baseExtent, ptr, (int)(var->baseExtent), MPI_BYTE, &stat );
					else
						result = MPI_File_write( fich, ptr, (int)(var->baseExtent), MPI_BYTE, &stat );
					break;
				}
			}
			/* 9.3.2. TEXT FORMAT */
			else {
				MPI_Status stat;
				char buff[ formatSize1 + 1 ];

				switch ( fileMode ) {
				   case HIT_FILE_READ:
						if ( tileMode == HIT_FILE_ARRAY )
							result = MPI_File_read_at( fich, (long)header_size + file_offset * (formatSize1 + 1), buff, formatSize1, MPI_CHAR, &stat );
						else
							result = MPI_File_read( fich, buff, formatSize1, MPI_CHAR, &stat );

						if ( result == MPI_SUCCESS ) {
							buff[ formatSize1 ] = '\0';
							switch ( type ) {
								case HIT_FILE_CHAR: sscanf( buff, patternRead, (char*)(ptr) );
											  break;
								case HIT_FILE_SHORT: sscanf( buff, patternRead, (short*)(ptr) );
											  break;
								case HIT_FILE_INT: sscanf( buff, patternRead, (int*)(ptr) );
											  break;
								case HIT_FILE_LONG: sscanf( buff, patternRead, (long*)(ptr) );
											  break;
								case HIT_FILE_FLOAT: sscanf( buff, patternRead, (float*)(ptr) );
											  break;
								case HIT_FILE_DOUBLE: sscanf( buff, patternRead, (double*)(ptr) );
											  break;
							}
						}
						break;

				   case HIT_FILE_WRITE:
						switch ( type ) {
							case HIT_FILE_CHAR: sprintf( buff, patternWrite, *(char*)(ptr) );
										  break;
							case HIT_FILE_SHORT: sprintf( buff, patternWrite, *(short*)(ptr) );
										  break;
							case HIT_FILE_INT: sprintf( buff, patternWrite, *(int*)(ptr) );
										  break;
							case HIT_FILE_LONG: sprintf( buff, patternWrite, *(long*)(ptr) );
										  break;
							case HIT_FILE_FLOAT: sprintf( buff, patternWrite, *(float*)(ptr) );
										  break;
							case HIT_FILE_DOUBLE: sprintf( buff, patternWrite, *(double*)(ptr) );
										  break;
						}
						if ( tileMode == HIT_FILE_ARRAY )
							result = MPI_File_write_at( fich, (long)header_size + file_offset * (formatSize1 + 1), buff, formatSize1+1, MPI_CHAR, &stat );
						else
							result = MPI_File_write( fich, buff, formatSize1+1, MPI_CHAR, &stat );
						break;
				}
			}


			/* 9.4. CHECK OPERATION RESULT */
			if ( result != MPI_SUCCESS ) {
				MPI_File_close( &fich );
				if ( fileMode == HIT_FILE_READ ) {
					hit_errInternal(__FUNCTION__, "Reading data in file", fileName, debugCodeFile, debugCodeLine); 
				}
				else if ( fileFormat == HIT_FILE_BINARY ) {
					hit_errInternal(__FUNCTION__, "Writing binary data in file", fileName, debugCodeFile, debugCodeLine); 
				}
				else hit_errInternal(__FUNCTION__, "Writing text data in file", fileName, debugCodeFile, debugCodeLine); 
			}
		}

		/* 9.5. REAL OWNER: ADVANCE MEMORY POINTER TO THE NEXT ELEMENT */
		if ( var->memStatus == HIT_MS_OWNER ) ptr = (char *)ptr + var->baseExtent;

		/* 9.6. SELECTION VARIABLES: ADVANCE TILE COORDINATES */
		if ( var->memStatus == HIT_MS_NOT_OWNER ) {
			j = hit_shapeDims(var->shape)-1;
			do {
				ind[j] = (ind[j]+1) % (var->card[j]);					

				if ( ind[j] != 0 ) break;
				j--;
			} while( j>=0 );
		}

		/* 9.7. ARRAY MODE: ADVANCE ARRAY COORDINATES */
		if ( tileMode == HIT_FILE_ARRAY ) {
			j = hit_shapeDims(var->shape)-1;
			do {
				if ( indsh[j] == hit_shapeSig(var->shape,j).end ) {
					indsh[j] = hit_shapeSig(var->shape,j).begin;
				}
				else { 
					indsh[j] += hit_shapeSig(var->shape,j).stride; 
					break; 
				}
				j--;
			} while(j>=0);
		}
	}

	/* 10. END */
	if ( io_filesystem == HIT_FILE_SHARED ) {
		fclose( fichSh );
	}
	else {
		ok = MPI_File_close( &fich );
		if ( ok != MPI_SUCCESS ) hit_errInternal(__FUNCTION__, "Closing file", fileName, debugCodeFile, debugCodeLine); 
	}
	return 0;
}

/* Print usage of environment variables to control file options */
void hit_filePrintUsage() {
	fprintf(stderr, "\nHITMAP files, environment variables:\n");
	fprintf(stderr, "\tHIT_FILESYSTEM=shared|mpi Use a shared filesystem with standard file API, or MPI_File\n");
	fprintf(stderr, "\tHIT_FILE_TEXT=no|yes      Default output files format: Text or binary\n");
	fprintf(stderr, "\tHIT_FILE_COORD=array|tile Default coordinate system for files: Single array file or multiple tile files\n");
	fprintf(stderr, "\tHIT_FILE_HEADER=yes|no    Default presence of metadata header in files\n");
	fprintf(stderr, "\tHIT_FILE_TXT_SIZE=<n>     Default size of text numbers\n");
	fprintf(stderr, "\tHIT_FILE_TXT_DECIMALS=<n> Default number of decimal digits of floating point text numbers\n");
}

