/**
 * Hierarchical tiling of multi-dimensional dense array.
 * Reading/Writing dense tiles with MPI File functionalities
 *
 * @file hit_tileFile.c
 * @version 1.3.1
 * @author Arturo Gonzalez-Escribano
 * @date Nov 2012, May 2018
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
#include <hit_com.h>

/* Hit FUNCTION TO READ/WRITE TILES FROM/TO BINARY OR TEXT FILES USING A GIVEN FORMAT */
int hit_tileFileInternal(
		void * varP, 
		const int fileFormat, 
		const int fileMode, 
		const int tileMode, 
		const int type, 
		const int formatSize1, const int formatSize2, 
		const char *fileName, 
		const char *debugVarName, 
		const char *debugCodeFile, 
		int debugCodeLine
		) {
  	
	HitTile *var = (HitTile *)varP;

	// SELECT THE FILE BACKEND WITH AN ENVIRONMENT VARIABLE (MPI vs. SHARED FILE SYSTEM USING C STANDARD INTERFACE)
#define HIT_FILE_MPI	1
#define HIT_FILE_SHARED	2
	int mode = HIT_FILE_SHARED;	// Default mode
	char *modeStr = getenv("HITMAP_FILE");
	if ( modeStr != NULL ) {
		if ( !strcmp( modeStr, "MPI" ) ) mode = HIT_FILE_MPI;
		else if ( strcmp( modeStr, "SHARED" ) ) {
			fprintf( stderr, "Warning: Unknown HITMAP_FILE mode: %s\n", modeStr );
		}
	}
printf("File mode: %d, string: %s\n", mode, modeStr ); fflush(stdout);
	

	/* 1. CHECK FILE MODE PARAMETER.
	 * 		WRITE MODE: ENSURE THAT THE FILE IS CREATED 
	 * 		(IT AVOIDS CONCURRENCY PROBLEMS WHEN OPENING IN OVER-WRITE MODE) */
if ( mode == HIT_FILE_SHARED ) {
	int ok;
	switch ( fileMode ) {
		case HIT_FILE_READ: break;

		case HIT_FILE_WRITE: 
					ok = open( fileName, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR );
					if (ok == -1) hit_errInternal(__FUNCTION__, "Creating file", fileName, debugCodeFile, debugCodeLine); 
					// TODO: Change COMM_WORLD by the communicator in the tile layout
					//MPI_Barrier( MPI_COMM_WORLD );
					close( ok );
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
	long file_offset;
	void *ptr = NULL;
	char patternRead[32];
	char patternWrite[32];

	/* 4. OPEN FILE FOR READ/WRITE */
	int ok;
	FILE *fichSh = NULL;
	MPI_File fich;

if ( mode == HIT_FILE_SHARED ) {
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
					break;

		default:
			hit_errInternal(__FUNCTION__, "Unknown file mode", debugVarName, debugCodeFile, debugCodeLine);
			break;
	}
}

	/* 5. BUILD FORMAT */
	if ( fileFormat != HIT_FILE_BINARY ) {
		switch ( type ) {
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
				hit_errInternal(__FUNCTION__, "Unknown file type for variable", debugVarName, debugCodeFile, debugCodeLine); 
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

	/* 7. ARRAY MODE: INITIALISE indsh WITH THE SIGNATURE-BEGIN OF THE CORRESPONDING DIMENSION */
	if ( tileMode == HIT_FILE_ARRAY )
		for( i=0; i<hit_shapeDims(var->shape); i++ ) 
			indsh[i] = hit_shapeSig(var->shape,i).begin;

	/* 8. INITIALIZATION OF MEMORY LOCATION POINTERS/INDECES */
	/* 8.1. REAL OWNER VARIABLE: INITIALIZE POINTER AT BEGINNING OF DATA REGION */
	if ( var->memStatus == HIT_MS_OWNER ) ptr = var->data;
	/* 8.2. SELECTION VARIABLES: INITIALIZE INDECES TO LOCATE DATA IN MEMORY */
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

if( mode == HIT_FILE_SHARED ) {
		/* 9.2. ARRAY MODE: COMPUTE OFFSET IN THE FILE AND RELOCATE FILE POINTER */
		if ( tileMode == HIT_FILE_ARRAY ) {
			file_offset = 0;
			for( j=0; j<hit_shapeDims(var->shape); j++) {
				file_offset += indsh[j] * rootAcumCard[j+1];
				/* Patch for BlockTiles (TO BE DEPRECATED) */
				//file_offset += indsh[j] * acumChildSizes[j+1] * rootAcumCard[j+1];
			}
			/* 9.2.1. BINARY FORMAT */
			if ( fileFormat == HIT_FILE_BINARY ) 
				ok = fseek( fichSh, file_offset * (long)var->baseExtent, SEEK_SET );
			/* 9.2.2. TEXT FORMAT */
			else 
				ok = fseek( fichSh, file_offset * (formatSize1 + 1), SEEK_SET );
				  
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
				hit_errInternal(__FUNCTION__, "Reading data in file", fileName, debugCodeFile, debugCodeLine); 
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
				file_offset += indsh[j] * rootAcumCard[j+1];
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
					result = MPI_File_read_at( fich, file_offset * (long)var->baseExtent, ptr, (int)(var->baseExtent), MPI_BYTE, &stat );
				else
					result = MPI_File_read( fich, ptr, (int)(var->baseExtent), MPI_BYTE, &stat );

			   	break;
			   case HIT_FILE_WRITE: 
				if ( tileMode == HIT_FILE_ARRAY )
					result = MPI_File_write_at( fich, file_offset * (long)var->baseExtent, ptr, (int)(var->baseExtent), MPI_BYTE, &stat );
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
						result = MPI_File_read_at( fich, file_offset * (formatSize1 + 1), buff, formatSize1, MPI_CHAR, &stat );
					else
						result = MPI_File_read( fich, buff, formatSize1, MPI_CHAR, &stat );

					if ( result == MPI_SUCCESS ) {
						buff[ formatSize1 ] = '\0';
						switch ( type ) {
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
						result = MPI_File_write_at( fich, file_offset * (formatSize1 + 1), buff, formatSize1+1, MPI_CHAR, &stat );
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
/*
		if ( fileMode == HIT_FILE_READ ) {
			if ( result != 1 ) {
				MPI_File_close( &fich );
				hit_errInternal(__FUNCTION__, "Reading data in file", fileName, debugCodeFile, debugCodeLine); 
			}
		}
		else if ( fileFormat == HIT_FILE_BINARY ) {
			if ( result != 1 ) {
				MPI_File_close( &fich );
				hit_errInternal(__FUNCTION__, "Writing binary data in file", fileName, debugCodeFile, debugCodeLine); 
			}
		}
		else if ( result != formatSize1+1 ) {
			MPI_File_close( &fich );
			hit_errInternal(__FUNCTION__, "Writing text data in file", fileName, debugCodeFile, debugCodeLine); 
		}
*/
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
if ( mode == HIT_FILE_SHARED ) {
	fclose( fichSh );
}
else {
	ok = MPI_File_close( &fich );
	if ( ok != MPI_SUCCESS ) hit_errInternal(__FUNCTION__, "Closing file", fileName, debugCodeFile, debugCodeLine); 
}
	return 0;
}

