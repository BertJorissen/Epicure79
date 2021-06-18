/*
* refMPIJacobi2D.v1.c
*	Simple 2D Cellular automata. MPI reference code manually developed and optimized
*
* v1.0
* (c) 2007, Arturo Gonzalez-Escribano
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
 * Copyright (c) 2007-2019, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<mpi.h>


#define	mpiTestError(ok,cad)	\
    if ( ok != MPI_SUCCESS ) {	\
        fprintf(stderr,"RunTime-Error, Rank %d: %s - %d\n", mpiRank, cad, ok);\
        exit(-1);				\
    }


int mpiRank;
int mpiNProc;
double clock, clockReduce;


/* 
 * UTILITY: DECOMPOSE A NUMBER IN 2 PRIME FACTORS AS NEAR AS POSSIBLE TO THE SQUARE ROOT 
 */
int * factors2D(int numProc) {
	int *result;
	int low, high;
	int product;

	/* 1. ALLOCATE MEMORY FOR result */
	result = (int *)calloc(2, sizeof(int));
	if ( result == NULL )
        fprintf(stderr,"RunTime-Error, Rank %d: Allocating factors2D result\n", mpiRank);

	/* 2. START AS NEAR AS POSSIBLE TO THE SQUARE ROOT */
	low = high = (int)sqrt((double)numProc);
	product = low*high;

	/* 3. MOVE UP/DOWN TO SEARCH THE NEAREST FACTORS */
	while ( product != numProc ) {
		if (product < numProc) {
			high++;
			product = product + low;
		}
		else {
			low--;
			if (low==1) { high=numProc; product=numProc; }
			else product = product - high;
		}
	}

	/* 4. RETURN RESULT */
	result[0] = high;
	result[1] = low;
	return result;
}


/*
 * MAIN PROGRAM
 */
int main(int argc, char *argv[]) {
	int ok;
	int i,j;
#ifdef DEBUG
	setbuf(stderr, NULL);
	setbuf(stdout, NULL);
#endif

	/* 0. READ PARAMETERS */
	int rows, columns, numIter;

	if ( argc != 4 ) {
		fprintf(stderr, "\nUsage: %s <numRows> <numColumns> <numIterations>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	rows = atoi( argv[1] );
	columns = atoi( argv[2] );
	numIter = atoi( argv[3] );


    /* 1. INITIALIZE MPI */
    ok = MPI_Init( &argc, &argv);
	mpiTestError( ok, "Initializing MPI" );

    /* 2. GET BASIC GROUP PARAMETERS */
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiNProc);

    /* 3. DEFINE MAIN DATA STRUCTURES */
	double *matrix;
	double *matrixCopy;
	matrix = (double *)calloc( (size_t)(rows * columns), sizeof(double) );
	matrixCopy = (double *)calloc( (size_t)(rows * columns), sizeof(double) );

	if ( matrix==NULL || matrixCopy==NULL) {
        fprintf(stderr,"RunTime-Error, Rank %d: Allocating data structures\n", mpiRank);
		exit(2);
	}

#define	element(mat, idx1, idx2)	(mat[(idx1)*columns+(idx2)])

	/* 4. INITIAL MATRIX VALUES */
	for (i=0; i<rows; i++) {
		for (j=0; j<columns; j++) {
			element(matrix, i, j) = 0.0;
		}
	}
	/* 5. FIXED BOUNDARY VALUES */
	for (i=0; i<rows; i++) {
		element(matrix, i, 0) = 3.0;
		element(matrix, i, columns-1) = 4.0;
		// INITIALIZE FIXED BORDERS ALSO ON THE COPY
		element(matrixCopy, i, 0) = 3.0;
		element(matrixCopy, i, columns-1) = 4.0;
	}
	for (i=0; i<columns; i++) {
		element(matrix, 0, i) = 1.0;
		element(matrix, rows-1, i) = 2.0;
		// INITIALIZE FIXED BORDERS ALSO ON THE COPY
		element(matrixCopy, 0, i) = 1.0;
		element(matrixCopy, rows-1, i) = 2.0;
	}

	/* 6. START CLOCK */
	MPI_Barrier( MPI_COMM_WORLD );
	clock = MPI_Wtime();

	/* 7. CARTESIAN TOPOLOGY */
	MPI_Comm cartesianComm;
	int *procs = factors2D( mpiNProc );

	/* 7.1. CHECK: NO SUPPORT FOR PARTITIONING LESS ELEMENTS THAN PROCS IN A DIMENSION */
	if ( procs[0] > rows-2 || procs[1] > columns-2 ) {
		if ( mpiRank == 0 ) fprintf(stderr, "Error: This code does not support partitioning less rows/columns than processors in that axis\n");
		MPI_Finalize();
	}

	int periods[2] = { 0, 0 };
	ok = MPI_Cart_create(MPI_COMM_WORLD, 2, procs, periods, 0, &cartesianComm);
	mpiTestError( ok, "Creating cartesian toology" );

	int myCoords[2];
	ok = MPI_Cart_coords(cartesianComm, mpiRank, 2, myCoords);
	mpiTestError( ok, "Getting cartesian coordinates" );

	int rankUp=-1000, rankDown=-1000, rankLeft=-1000, rankRight=-1000;
	int foo;

	if ( myCoords[0] != 0 ) {
		ok = MPI_Cart_shift(cartesianComm, 0, -1, &foo, &rankUp);
		mpiTestError( ok, "Shifting cartesian coordinates up" );
	}

	if ( myCoords[0] != procs[0]-1 ) {
		ok = MPI_Cart_shift(cartesianComm, 0, +1, &foo, &rankDown);
		mpiTestError( ok, "Shifting cartesian coordinates down" );
	}

	if ( myCoords[1] != 0 ) {
		ok = MPI_Cart_shift(cartesianComm, 1, -1, &foo, &rankLeft);
		mpiTestError( ok, "Shifting cartesian coordinates left" );
	}

	if ( myCoords[1] != procs[1]-1 ) {
		ok = MPI_Cart_shift(cartesianComm, 1, +1, &foo, &rankRight);
		mpiTestError( ok, "Shifting cartesian coordinates right" );
	}

#ifdef DEBUG
fprintf(stderr,"CTRL RANKS: %d (%d,%d,%d,%d)\n", mpiRank, rankUp, rankDown, rankLeft, rankRight);
#endif

	/* 8. COMPUTE THE LOCAL PART OF THE MATRIX */
	double procRatio[2];
	procRatio[0] = (double)(rows-2)/procs[0];
	procRatio[1] = (double)(columns-2)/procs[1];

	int begin[2];
	int end[2];

#ifdef DEBUG
fprintf(stderr,"CTRL %d, myCoords %d,%d\n", mpiRank, myCoords[0], myCoords[1]);
#endif

	begin[0] = 1 + (int)floor( myCoords[0]*procRatio[0] ); 
	end[0] = 1 + (int)floor( (myCoords[0]+((rows > procs[0]) ? 1 : 0))*procRatio[0] - ((rows > procs[0]) ? 1 : 0) );
	begin[1] = 1 + (int)floor( myCoords[1]*procRatio[1] ); 
	end[1] = 1 + (int)floor( (myCoords[1]+((columns > procs[1]) ? 1 : 0))*procRatio[1] - ((columns > procs[1]) ? 1 : 0) );

#ifdef DEBUG
fprintf(stderr,"CTRL %d, matrix[%d:%d][%d:%d]\n", mpiRank, begin[0], end[0], begin[1], end[1]);
#endif

	/* 9. COMMIT MPI TYPES */
	MPI_Datatype verticalBorderType;
	ok = MPI_Type_hvector( end[0]-begin[0]+1, 1, (int)sizeof(double)*columns, MPI_DOUBLE, &verticalBorderType );
	mpiTestError( ok, "Creating the columns type" );
	ok = MPI_Type_commit( &verticalBorderType );
	mpiTestError( ok, "Commiting the columns type" );

	MPI_Request request[4];
	MPI_Status status;

	/* 10. LOOP: UNROLLED ONE ITERATION TO AVOID EXTRA COMM. STAGE */
	int loopIndex;
	for (loopIndex = 0; loopIndex < numIter-1; loopIndex++) {

		/* 11. COMPUTE LOCAL PART */
		for (i=begin[0]; i<=end[0]; i++) {
			for (j=begin[1]; j<=end[1]; j++) {
				element( matrix,i,j ) = ( element( matrixCopy,i-1,j )  + element( matrixCopy,i+1,j ) + element( matrixCopy,i,j-1 ) + element( matrixCopy,i,j+1 ) ) / 4;
			}
		}

		/* 12. COMMUNICATE BORDERS: SEND MATRIX VALUES */
		if ( myCoords[0] != 0 ) {
			ok = MPI_Isend( &element(matrix, begin[0], begin[1]), end[1]-begin[1]+1, MPI_DOUBLE, rankUp, 0, MPI_COMM_WORLD, &request[0] );
			mpiTestError( ok, "Send Up" );
		}
		if ( myCoords[0] != procs[0]-1 ) {
			ok = MPI_Isend( &element(matrix, end[0], begin[1]), end[1]-begin[1]+1, MPI_DOUBLE, rankDown, 1, MPI_COMM_WORLD, &request[1] );
			mpiTestError( ok, "Send Down" );
		}
		if ( myCoords[1] != 0 ) {
			ok = MPI_Isend( &element(matrix,begin[0], begin[1]), 1, verticalBorderType, rankLeft, 2, MPI_COMM_WORLD, &request[2] );
			mpiTestError( ok, "Send Left" );
		}
		if ( myCoords[1] != procs[1]-1 ) {
			ok = MPI_Isend( &element(matrix,begin[0],end[1]), 1, verticalBorderType, rankRight, 3, MPI_COMM_WORLD, &request[3] );
			mpiTestError( ok, "Send Right" );
		}

		/* 13. LOCAL SHADOW COPY (WITHOUT BORDERS) */
		for (i=begin[0]; i<=end[0]; i++) {
			for (j=begin[1]; j<=end[1]; j++) {
				element( matrixCopy,i,j ) = element( matrix,i,j );
			}
		}

		/* 14. COMMUNICATE BORDERS: RECV IN MATRIX COPY */
		if ( myCoords[0] != procs[0]-1 ) {
			ok = MPI_Recv( &element(matrixCopy, end[0]+1, begin[1]), end[1]-begin[1]+1, MPI_DOUBLE, rankDown, 0, MPI_COMM_WORLD, &status );
			mpiTestError( ok, "Recv from Down" );
		}
		if ( myCoords[0] != 0 ) {
			ok = MPI_Recv( &element(matrixCopy, begin[0]-1, begin[1]), end[1]-begin[1]+1, MPI_DOUBLE, rankUp, 1, MPI_COMM_WORLD, &status );
			mpiTestError( ok, "Recv from Up" );
		}
		if ( myCoords[1] != procs[1]-1 ) {
			ok = MPI_Recv( &element(matrixCopy, begin[0], end[1]+1), 1, verticalBorderType, rankRight, 2, MPI_COMM_WORLD, &status );
			mpiTestError( ok, "Recv from Right" );
		}
		if ( myCoords[1] != 0 ) {
			ok = MPI_Recv( &element(matrixCopy, begin[0], begin[1]-1), 1, verticalBorderType, rankLeft, 3, MPI_COMM_WORLD, &status );
			mpiTestError( ok, "Recv from Left" );
		}

		/* 15. END Isends */
		if ( myCoords[0] != 0 ) {
			MPI_Wait( &request[0], &status );
		}
		if ( myCoords[0] != procs[0]-1 ) {
			MPI_Wait( &request[1], &status );
		}
		if ( myCoords[1] != 0 ) {
			MPI_Wait( &request[2], &status );
		}
		if ( myCoords[1] != procs[1]-1 ) {
			MPI_Wait( &request[3], &status );
		}

	} /* END LOOP */

	// UNROLLED LOOP: LAST ITERATION WITHOUT COMMUNICATION
	/* 16. COMPUTE LOCAL PART */
	for (i=begin[0]; i<=end[0]; i++) {
		for (j=begin[1]; j<=end[1]; j++) {
			element( matrix,i,j ) = ( element( matrixCopy,i-1,j )  + element( matrixCopy,i+1,j ) + element( matrixCopy,i,j-1 ) + element( matrixCopy,i,j+1 ) ) / 4;
		}
	}

	/* 17. FREE COMMUNICATION PATTERN RESOURCES */
	ok = MPI_Type_free( &verticalBorderType );
	mpiTestError( ok, "Freeing vertical border type" );

#ifdef DEBUG
for (i=begin[0]; i<=end[0]; i++) {
	for (j=begin[1]; j<=end[1]; j++) {
		fprintf(stderr,"M[%d][%d] %lf\n", i,j, element(matrix,i,j));
	}
}
#endif

	/* 18. COMMUNICATE ALL LOCAL PIECES */
	if ( mpiRank == 0 ) {
		/* 18.1. RANK 0 RECEIVES ALL */
		int rank;
		for ( rank=1; rank<mpiNProc; rank++ ) {
			/* COMPUTE THE POSITION OF THE REMOTE LOCAL PART TO BE RECEIVED */
			int remoteCoords[2];
			ok = MPI_Cart_coords(cartesianComm, rank, 2, remoteCoords);
			mpiTestError( ok, "Getting remote cartesian coordinates in end comm." );

			int remoteBegin[2];
			int remoteEnd[2];

			remoteBegin[0] = 1 + (int)floor( remoteCoords[0]*procRatio[0] ); 
			remoteEnd[0] = 1 + (int)floor( (remoteCoords[0]+((rows > procs[0]) ? 1 : 0))*procRatio[0] - ((rows > procs[0]) ? 1 : 0) );
			remoteBegin[1] = 1 + (int)floor( remoteCoords[1]*procRatio[1] ); 
			remoteEnd[1] = 1 + (int)floor( (remoteCoords[1]+((columns > procs[1]) ? 1 : 0))*procRatio[1] - ((columns > procs[1]) ? 1 : 0) );

#ifdef DEBUG
fprintf(stderr,"CTRL RemoteMatrix %d, [%d:%d][%d:%d]\n", rank, remoteBegin[0], remoteEnd[0], remoteBegin[1], remoteEnd[1]);
#endif
			MPI_Datatype remoteType;
			ok = MPI_Type_hvector( remoteEnd[0]-remoteBegin[0]+1, remoteEnd[1]-remoteBegin[1]+1, (MPI_Aint)((int)sizeof(double)*columns), MPI_DOUBLE, &remoteType );
			mpiTestError( ok, "Getting remote type in end comm." );
			ok = MPI_Type_commit( &remoteType );
			mpiTestError( ok, "Commiting the remote type in end comm." );

			ok = MPI_Recv( &element( matrix,remoteBegin[0],remoteBegin[1] ), 1, remoteType, rank, rank, MPI_COMM_WORLD, &status );
			mpiTestError( ok, "Receiving remote parts in end comm." );

			ok = MPI_Type_free( &remoteType );
			mpiTestError( ok, "Freeing remote type in end comm." );
		}
	}
	else {
		/* 18.2. OTHERS SEND THE LOCAL PART */
#ifdef DEBUG
fprintf(stderr,"CTRL LocalMatrix %d, [%d:%d][%d:%d]\n", mpiRank, begin[0], end[0], begin[1], end[1]);
#endif
		MPI_Datatype localType;
		ok = MPI_Type_hvector( end[0]-begin[0]+1, end[1]-begin[1]+1, (MPI_Aint)((int)sizeof(double)*columns), MPI_DOUBLE, &localType );
		mpiTestError( ok, "Getting local type in end comm." );
		ok = MPI_Type_commit( &localType );
		mpiTestError( ok, "Commiting the local type in end comm." );

		ok = MPI_Send( &element( matrix,begin[0],begin[1] ), 1, localType, 0, mpiRank, MPI_COMM_WORLD );
		mpiTestError( ok, "Sending local part in end comm." );

		ok = MPI_Type_free( &localType );
		mpiTestError( ok, "Freeing local type in end comm." );
	}

	/* 19. STOP AND PRINT CLOCK */	
	clock = MPI_Wtime() - clock;
	MPI_Reduce( &clock, &clockReduce, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD ); 
	if (mpiRank==0) printf("Clock_%s: %14.6lf\n", "mainClock", clockReduce);

	/* 20. WRITE RESULT IN FILE */
#ifdef WRITE_RESULT
	if ( mpiRank == 0 ) {
		FILE *f;
		if ((f=fopen("Result.out.dtxt","w")) == NULL) {
			fprintf(stderr,"Error: Imposible to open output file\n");
			exit(-1);
		}
		for (i=0; i<rows; i++) {
			for (j=0; j<rows; j++) {
				fprintf(f,"%014.4lf", element(matrix,i,j) );
				fprintf(f,"\n");
			}
		}
		fclose(f);
	}
#endif

    /* 21. END PROGRAM */
	free( procs );
	free( matrix );
	free( matrixCopy );
	MPI_Comm_free( &cartesianComm );
    MPI_Finalize();

	/* 22. RETURN CORRECT */
	return 0;
}

