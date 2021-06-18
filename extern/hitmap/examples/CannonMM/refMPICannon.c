/*
* refMPICannon.c
* 	MPI reference code manually developed and optimized
*	Cannon's algorithm for Matrix Multiplication. 
*
*	Simplified version where the matrix is always divisible in square pieces
*	for the given number of processors.
*
*	Asynchronous sends, synchronous receives
*
* v1.0
* (c) 2007-2009, Arturo Gonzalez-Escribano
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

#define MATRIX_ROWS		10
#define MATRIX_COLUMNS	20

#define	mpiTestError(ok,cad)	\
    if ( ok != MPI_SUCCESS ) {	\
        fprintf(stderr,"RunTime-Error, Rank %d: %s - %d\n", mpiRank, cad, ok);\
        exit(-1);				\
    }


#define elemA(i,j)	A[(i)*MATRIX_COLUMNS+(j)]
#define elemCopyA(i,j)	copyA[(i)*MATRIX_COLUMNS+(j)]
#define elemB(i,j)	B[(i)*MATRIX_ROWS+(j)]
#define elemCopyB(i,j)	copyB[(i)*MATRIX_ROWS+(j)]
#define elemC(i,j)	C[(i)*MATRIX_ROWS+(j)]


int mpiRank;
int mpiNProc;
double mainClock, clockReduce;


int main(int argc, char *argv[]) {
	int ok;
	int i,j,k;
#ifdef DEBUG
	setbuf(stderr, NULL);
	setbuf(stdout, NULL);
#endif

    /* 1. INITIALIZE MPI */
    ok = MPI_Init( &argc, &argv);
	mpiTestError( ok, "Initializing MPI" );

    /* 2. GET BASIC GROUP PARAMETERS */
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiNProc);

	/* 3. CHECK APPROPRIATE NUMBER OF PROCS/SIZES FOR THE SIMPLIFIED VERSION */
	double sqProc = sqrt( mpiNProc );
	if ( sqProc != floor(sqProc) ) {
		if ( mpiRank == 0 ) {
			fprintf(stderr,"Error: This code does not support a number of procs. without a perfect square root: %d\n", mpiNProc);
		}
		MPI_Finalize();
		exit(-1);
	}
	if ( MATRIX_COLUMNS % (int)sqProc != 0 || MATRIX_ROWS % (int)sqProc != 0 ) {
		if ( mpiRank == 0 ) {
			fprintf(stderr,"Error: This code does not support matrix sizes which are not integer divisible by the square root of the number of processors\n");
		}
		MPI_Finalize();
		exit(-1);
	}

    /* 4. DEFINE MAIN DATA STRUCTURES */
	double *A = (double *)calloc( (size_t)(MATRIX_ROWS*MATRIX_COLUMNS), sizeof(double));
	double *B = (double *)calloc( (size_t)(MATRIX_ROWS*MATRIX_COLUMNS), sizeof(double));
	double *C = (double *)calloc( (size_t)(MATRIX_ROWS*MATRIX_ROWS), sizeof(double));
	double *copyA = (double *)calloc( (size_t)(MATRIX_ROWS*MATRIX_COLUMNS), sizeof(double));
	double *copyB = (double *)calloc( (size_t)(MATRIX_ROWS*MATRIX_COLUMNS), sizeof(double));
	if ( A==NULL || B==NULL || C==NULL ) {
		fprintf(stderr,"Error, Rank %d: Allocating data structures\n", mpiRank);
		exit(3);
	}

	/* 5. INITIAL MATRIX VALUES */
	/* INIT A(i,j) = i*j + j */
	int cont=0;
	for (i=0; i<MATRIX_ROWS; i++) {
		for (j=0; j<MATRIX_COLUMNS; j++) {
			elemA(i,j) = cont++;
		}
	}
#ifdef B_IDENTITY
	/* INIT B = Identity */
	for (i=0; i<MATRIX_COLUMNS; i++) {
		for (j=0; j<MATRIX_ROWS; j++) {
			elemB(i,j) = (i==j) ?  1.0 : 0.0;
		}
	}
#else
	/* INIT B = Random with seed */
	srand48( 364543 );
	for (i=0; i<MATRIX_COLUMNS; i++) {
		for (j=0; j<MATRIX_ROWS; j++) {
			elemB(i,j) = drand48();
		}
	}
#endif
	/* INIT result = 0 */
	for (i=0; i<MATRIX_ROWS; i++) {
		for (j=0; j<MATRIX_ROWS; j++) {
			elemC(i,j) = 0.0;
		}
	}

	/* 6. START CLOCK */
	MPI_Barrier( MPI_COMM_WORLD );
	mainClock = MPI_Wtime();

	/* 7. CARTESIAN TOPOLOGY */
	MPI_Comm cartesianComm;
	int procs[2];
	procs[0] = (int)sqrt( mpiNProc );
	procs[1] = procs[0];
	int periods[2] = { 1, 1 };
	ok = MPI_Cart_create(MPI_COMM_WORLD, 2, procs, periods, 0, &cartesianComm);
	mpiTestError( ok, "Creating cartesian toology" );

	int myCoords[2];
	ok = MPI_Cart_coords(cartesianComm, mpiRank, 2, myCoords);
	mpiTestError( ok, "Getting cartesian coordinates" );

	int rankUp=-1000, rankDown=-1000, rankLeft=-1000, rankRight=-1000;
	int rankSendIniA=-1000, rankSendIniB=-1000;
	int rankRecvIniA=-1000, rankRecvIniB=-1000;

	ok = MPI_Cart_shift(cartesianComm, 0, +1, &rankUp, &rankDown);
	mpiTestError( ok, "Shifting cartesian coordinates vertical" );

	ok = MPI_Cart_shift(cartesianComm, 1, +1, &rankLeft, &rankRight);
	mpiTestError( ok, "Shifting cartesian coordinates horizontal" );

	ok = MPI_Cart_shift(cartesianComm, 1, -myCoords[0], &rankRecvIniA, &rankSendIniA);
	mpiTestError( ok, "Shifting cartesian coordinates ini A" );

	ok = MPI_Cart_shift(cartesianComm, 0, -myCoords[1], &rankRecvIniB, &rankSendIniB);
	mpiTestError( ok, "Shifting cartesian coordinates ini B" );

#ifdef DEBUG
fprintf(stderr,"CTRL %d RANKS: (%d,%d,%d,%d)\n", mpiRank, rankUp, rankDown, rankLeft, rankRight);
fprintf(stderr,"CTRL %d INI RANKS: A(send %d, recv %d) B(send %d, recv %d)\n", mpiRank, rankSendIniA, rankRecvIniA, rankSendIniB, rankRecvIniB);
#endif


	/* 8. COMPUTE THE INDECES OF THE LOCAL PART OF THE MATRICES */
	#define procRatioRows0		MATRIX_ROWS/procs[0]
	#define procRatioRows1		MATRIX_ROWS/procs[1]
	#define procRatioColumns0		MATRIX_COLUMNS/procs[0]
	#define procRatioColumns1		MATRIX_COLUMNS/procs[1]

	int beginA[2];
	int endA[2];
	int beginB[2];
	int endB[2];
	int beginC[2];
	int endC[2];

#ifdef DEBUG
fprintf(stderr,"CTRL %d, myCoords %d,%d\n", mpiRank, myCoords[0], myCoords[1]);
#endif

	beginA[0] = (int)floor( (double)myCoords[0]*procRatioRows0 ); 
	endA[0] = (int)floor( ((double)myCoords[0]+((MATRIX_ROWS > procs[0]) ? 1 : 0))*procRatioRows0 - ((MATRIX_ROWS > procs[0]) ? 1 : 0) );
	beginA[1] = (int)floor( (double)myCoords[1]*procRatioColumns1 ); 
	endA[1] = (int)floor( ((double)myCoords[1]+((MATRIX_COLUMNS > procs[1]) ? 1 : 0))*procRatioColumns1 - ((MATRIX_COLUMNS > procs[1]) ? 1 : 0) );

	beginB[0] = (int)floor( (double)myCoords[0]*procRatioColumns0 ); 
	endB[0] = (int)floor( ((double)myCoords[0]+((MATRIX_COLUMNS > procs[0]) ? 1 : 0))*procRatioColumns0 - ((MATRIX_COLUMNS > procs[0]) ? 1 : 0) );
	beginB[1] = (int)floor( (double)myCoords[1]*procRatioRows1 ); 
	endB[1] = (int)floor( ((double)myCoords[1]+((MATRIX_ROWS > procs[1]) ? 1 : 0))*procRatioRows1 - ((MATRIX_ROWS > procs[1]) ? 1 : 0) );

	beginC[0] = (int)floor( (double)myCoords[0]*procRatioRows0 ); 
	endC[0] = (int)floor( ((double)myCoords[0]+((MATRIX_ROWS > procs[0]) ? 1 : 0))*procRatioRows0 - ((MATRIX_ROWS > procs[0]) ? 1 : 0) );
	beginC[1] = (int)floor( (double)myCoords[1]*procRatioRows1 ); 
	endC[1] = (int)floor( ((double)myCoords[1]+((MATRIX_ROWS > procs[1]) ? 1 : 0))*procRatioRows1 - ((MATRIX_ROWS > procs[1]) ? 1 : 0) );

#ifdef DEBUG
fprintf(stderr,"CTRL %d, A[%d:%d][%d:%d]\n", mpiRank, beginA[0], endA[0], beginA[1], endA[1]);
fprintf(stderr,"CTRL %d, B[%d:%d][%d:%d]\n", mpiRank, beginB[0], endB[0], beginB[1], endB[1]);
fprintf(stderr,"CTRL %d, C[%d:%d][%d:%d]\n", mpiRank, beginC[0], endC[0], beginC[1], endC[1]);
#endif

	/* 9. COMMIT MPI TYPES */
	MPI_Datatype dataBlockA;
	MPI_Datatype dataBlockB;
	MPI_Datatype dataBlockC;

	ok = MPI_Type_hvector( endA[0]-beginA[0]+1, endA[1]-beginA[1]+1, sizeof(double)*(size_t)MATRIX_COLUMNS, MPI_DOUBLE, &dataBlockA );
	mpiTestError( ok, "Creating the A block type" );
	ok = MPI_Type_commit( &dataBlockA );
	mpiTestError( ok, "Commiting the A block type" );

	ok = MPI_Type_hvector( endB[0]-beginB[0]+1, endB[1]-beginB[1]+1, sizeof(double)*(size_t)MATRIX_ROWS, MPI_DOUBLE, &dataBlockB );
	mpiTestError( ok, "Creating the B block type" );
	ok = MPI_Type_commit( &dataBlockB );
	mpiTestError( ok, "Commiting the B block type" );

	ok = MPI_Type_hvector( endC[0]-beginC[0]+1, endC[1]-beginC[1]+1, sizeof(double)*(size_t)MATRIX_ROWS, MPI_DOUBLE, &dataBlockC );
	mpiTestError( ok, "Creating the C block type" );
	ok = MPI_Type_commit( &dataBlockC );
	mpiTestError( ok, "Commiting the C block type" );

	MPI_Request request[2];
	MPI_Status status;

#ifdef DEBUG_V
{
int i,j;
for (i=beginA[0]; i<=endA[0]; i++) {
	for (j=beginA[1]; j<=endA[1]; j++) {
		fprintf(stderr,"CTRL %d Before shift - A[%d][%d]=%lf\n", mpiRank, i,j,elemA(i,j) );
	}
}
for (i=beginB[0]; i<=endB[0]; i++) {
	for (j=beginB[1]; j<=endB[1]; j++) {
		fprintf(stderr,"CTRL %d Before shift - B[%d][%d]=%lf\n", mpiRank, i,j,elemB(i,j) );
	}
}
}
#endif

	/* 10. INITIAL DATA DISTRIBUTION */
	ok = MPI_Isend( &(elemA(beginA[0],beginA[1])), 1, dataBlockA, rankSendIniA, 0, MPI_COMM_WORLD, &request[0] );
	mpiTestError( ok, "Send Ini A" );

	ok = MPI_Isend( &(elemB(beginB[0],beginB[1])), 1, dataBlockB, rankSendIniB, 1, MPI_COMM_WORLD, &request[1] );
	mpiTestError( ok, "Send Ini B" );

	ok = MPI_Recv( &(elemCopyA(beginA[0],beginA[1])), 1, dataBlockA, rankRecvIniA, 0, MPI_COMM_WORLD, &status );
	mpiTestError( ok, "Recv Ini A" );

	ok = MPI_Recv( &(elemCopyB(beginB[0],beginB[1])), 1, dataBlockB, rankRecvIniB, 1, MPI_COMM_WORLD, &status );
	mpiTestError( ok, "Recv Ini B" );

	MPI_Wait( &request[0], &status );
	MPI_Wait( &request[1], &status );

	for (i=beginA[0]; i<=endA[0]; i++) {
		for (j=beginA[1]; j<=endA[1]; j++) {
			elemA(i,j) = elemCopyA(i,j);
		}
	}
	for (i=beginB[0]; i<=endB[0]; i++) {
		for (j=beginB[1]; j<=endB[1]; j++) {
			elemB(i,j) = elemCopyB(i,j);
		}
	}


#ifdef DEBUG_V
{
int i,j;
for (i=beginA[0]; i<=endA[0]; i++) {
	for (j=beginA[1]; j<=endA[1]; j++) {
		fprintf(stderr,"CTRL %d After shift - A[%d][%d]=%lf\n", mpiRank, i,j, elemA(i,j) );
	}
}
for (i=beginB[0]; i<=endB[0]; i++) {
	for (j=beginB[1]; j<=endB[1]; j++) {
		fprintf(stderr,"CTRL %d After shift - B[%d][%d]=%lf\n", mpiRank, i,j,elemB(i,j) );
	}
}
}
#endif

	/* 11. LOOP, ONE ITERATION LESS TO AVOID EXTRA COMUNICATION 
					AFTER THE LAST ITERATION */
	int loopIndex;
	for (loopIndex = 0; loopIndex < procs[0]-1; loopIndex++) {

		/* 12. COMPUTE LOCAL PRODUCT */
		for (i=beginC[0]; i<=endC[0]; i++) {
			for (j=beginC[1]; j<=endC[1]; j++) {
				for (k=0; k<=endA[1]-beginA[1]; k++) {
#ifdef DEBUG
fprintf(stderr,"CTRL %d, i,j,k:(%d,%d,%d) %lf x %lf Stage 1\n",mpiRank, i,j,k,elemA(i,k+begin[1]), elemB(k+beginB[0],j));
#endif
					elemC(i,j) = elemC(i,j) + elemA(i,k+beginA[1]) * elemB(k+beginB[0],j);
				}
			}
		}

		/* 13. COMMUNICATE A,B BLOCKS */
		ok = MPI_Isend( &elemA(beginA[0],beginA[1]), 1, dataBlockA, rankLeft, 0, MPI_COMM_WORLD, &request[0] );
		mpiTestError( ok, "Send Block A" );

		ok = MPI_Isend( &elemB(beginB[0],beginB[1]), 1, dataBlockB, rankUp, 1, MPI_COMM_WORLD, &request[1] );
		mpiTestError( ok, "Send Block B" );

		ok = MPI_Recv( &elemCopyA(beginA[0],beginA[1]), 1, dataBlockA, rankRight, 0, MPI_COMM_WORLD, &status );
		mpiTestError( ok, "Recv Block A" );

		ok = MPI_Recv( &elemCopyB(beginB[0],beginB[1]), 1, dataBlockB, rankDown, 1, MPI_COMM_WORLD, &status );
		mpiTestError( ok, "Recv Block B" );

#ifdef DEBUG_V
for (i=beginA[0]; i<=endA[0]; i++) {
	for (j=beginA[1]; j<=endA[1]; j++) {
		fprintf(stderr,"CTRL %d  After repeated comm. A[%d][%d] %lf\n", mpiRank, i,j, elemA(i,j));
	}
}
#endif
		/* 14. END Isends */
		MPI_Wait( &request[0], &status );
		MPI_Wait( &request[1], &status );

		for (i=beginA[0]; i<=endA[0]; i++) {
			for (j=beginA[1]; j<=endA[1]; j++) {
				elemA(i,j) = elemCopyA(i,j);
			}
		}
		for (i=beginB[0]; i<=endB[0]; i++) {
			for (j=beginB[1]; j<=endB[1]; j++) {
#ifdef DEBUG
fprintf(stderr,"CTRL %d B->%lf\n", mpiRank, elemCopyB(i,j));
#endif
				elemB(i,j) = elemCopyB(i,j);
			}
		}

	} /* END LOOP */

	ok = MPI_Type_free( &dataBlockA );
	mpiTestError( ok, "Freeing data block A type" );
	ok = MPI_Type_free( &dataBlockB );
	mpiTestError( ok, "Freeing data block B type" );

	/* 15. COMPUTE LOCAL PRODUCT, LAST ITERATION */
	for (i=beginC[0]; i<=endC[0]; i++) {
		for (j=beginC[1]; j<=endC[1]; j++) {
			for (k=0; k<=endA[1]-beginA[1]; k++) {
#ifdef DEBUG
fprintf(stderr,"CTRL %d, i,j,k:(%d,%d,%d) %lf x %lf Stage 1\n",mpiRank, i,j,k,elemA(i,k+begin[1]), elemB(k+beginB[0],j));
#endif
					elemC(i,j) = elemC(i,j) + elemA(i,k+beginA[1]) * elemB(k+beginB[0],j);
			}
		}
	}

#ifdef DEBUG
for (i=beginC[0]; i<=endC[0]; i++) {
	for (j=beginC[1]; j<=endC[1]; j++) {
		fprintf(stderr,"CTRL %d  Final C[%d][%d] %lf\n", mpiRank, i,j, elemC(i,j));
	}
}
#endif

	/* 16. COMMUNICATE ALL LOCAL PIECES */
	if ( mpiRank == 0 ) {
		/* 16.1. RANK 0 RECEIVES ALL */
		int rank;
		for ( rank=1; rank<mpiNProc; rank++ ) {
			/* COMPUTE THE REMOTE LOCAL PART */
			int remoteCoords[2];
			ok = MPI_Cart_coords(cartesianComm, rank, 2, remoteCoords);
			mpiTestError( ok, "Getting remote cartesian coordinates in end comm." );

			int remoteBeginC[2];
			remoteBeginC[0] = (int)floor( remoteCoords[0]*procRatioRows0 ); 
			remoteBeginC[1] = (int)floor( remoteCoords[1]*procRatioRows1 ); 

#ifdef DEBUG
			int remoteEndC[2];
			remoteEndC[0] = (int)floor( (remoteCoords[0]+((MATRIX_ROWS > procs[0]) ? 1 : 0))*procRatioRows0 - ((MATRIX_ROWS > procs[0]) ? 1 : 0) );
			remoteEndC[1] = (int)floor( (remoteCoords[1]+((MATRIX_ROWS > procs[1]) ? 1 : 0))*procRatioRows1 - ((MATRIX_ROWS > procs[1]) ? 1 : 0) );

fprintf(stderr,"CTRL RemoteC %d, [%d:%d][%d:%d]\n", rank, remoteBeginC[0], remoteEndC[0], remoteBeginC[1], remoteEndC[1]);
#endif

			ok = MPI_Recv( &(elemC(remoteBeginC[0],remoteBeginC[1])), 1, dataBlockC, rank, rank, MPI_COMM_WORLD, &status );
			mpiTestError( ok, "Receiving remote parts in end comm." );
		}
	}
	else {
		/* 16.2. OTHERS SEND THE LOCAL PART */
#ifdef DEBUG
fprintf(stderr,"CTRL LocalC %d, [%d:%d][%d:%d]\n", mpiRank, beginC[0], endC[0], beginC[1], endC[1]);
#endif
		ok = MPI_Send( &(elemC(beginC[0],beginC[1])), 1, dataBlockC, 0, mpiRank, MPI_COMM_WORLD );
		mpiTestError( ok, "Sending local part in end comm." );
	}
	/* FREE TYPE */
	ok = MPI_Type_free( &dataBlockC );
	mpiTestError( ok, "Freeing remote type in end comm." );

	/* 17. STOP AND PRINT CLOCK */	
	mainClock = MPI_Wtime() - mainClock;
	MPI_Reduce( &mainClock, &clockReduce, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD ); 
	if (mpiRank==0) printf("Clock_%s: %14.6lf\n", "mainClock", clockReduce);

	/* 18. WRITE RESULT IN FILE */
#ifdef WRITE_RESULT
	if ( mpiRank == 0 ) {
		FILE *f;
		if ((f=fopen("C.dtxt","w")) == NULL) {
			fprintf(stderr,"Error: Imposible to open output file\n");
			exit(-1);
		}
		for (i=0; i<MATRIX_ROWS; i++) {
			for (j=0; j<MATRIX_ROWS; j++) {
				fprintf(f,"%014.4lf", elemC(i,j));
				fprintf(f,"\n");
			}
		}
		fclose(f);
	}
#endif

    /* 19. END PROGRAM */
    MPI_Finalize();

	/* 20. RETURN CORRECT */
	return 0;
}
