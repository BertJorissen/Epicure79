/*
* refMPIluback.c
* 	MPI reference code manually developed and optimized
* 	Solving an equation system: LU factorization + Back Substitution
* 	Both algorithms use parallel block-cyclic data distribution
*
* 	C implementation of the correspondig SCALAPACK algorithms
*
* v1.1
* (c) 2010, Carlos de Blas Carton
* (c) 2015, Arturo Gonzalez-Escribano
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>

#define BC_OK 0
#define BC_ERROR -1
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define TRUE 1
#define FALSE 0

typedef struct mat {
  int dim1;
  int dim2;
  int blocksize1;
  int blocksize2;
  int blocksize;
  int nblock1;
  int nblock2;
  int last1;
  int last2;
  int mydim1;
  int mydim2;
  int mynblock1;
  int mynblock2;
  int mylast1;
  int mylast2;
  double ** mat;
} MATRIX;

int initialize(int*, char ***);
int freeAll();
int initializeMat(MATRIX *, int, int, int, int, int*, int*);
int initializeVec(MATRIX *, int, int, int*);
int freeMat (MATRIX *);
int randomMat(MATRIX *, int, double, double);

int print(MATRIX *, const char *);
int printVec(MATRIX *, const char *);

int initializeClock();
int stopClock();
int printClock();

void factorizeMat(MATRIX *, int, int);
void solveMat(MATRIX *, MATRIX *, int);

#define lastsize(iproc,nproc,nblock,lastsize,blocksize) ((iproc==((nblock-1)%nproc))?lastsize:blocksize)
#define ismine(coordb,coordp,numproc) ((coordb%numproc)==coordp) 

#define localMatrixBlockAt(m,i,j) m->mat[((i)/mydata.numproccol)*m->mynblock2+((j)/mydata.numprocrow)]
#define matrixBlockAt(m,i,j) m->mat[(i)*m->mynblock2+(j)]

double mytime, ** buffer2, ** buffer;
int dimmat1,dimmat2,dimbloq1,dimbloq2,*sizes;
MPI_Aint *addresses; 

MPI_Status estado; 
MPI_Request recibo; 
MPI_Datatype TBLOQUEM,TBLOQUEV, *types; 
MPI_Op sumavec; 

//process information
typedef struct proc {
  int myrow;
  int mycolumn;
  int mynumber;
  int numproc;
  int numprocrow;
  int numproccol;
  MPI_Comm comfilas;
  MPI_Comm comcolumnas;
} PROC;

PROC mydata; 

//reduce operation
void vectorAdd (double *in, double *inout, int *len, MPI_Datatype* type) {
	(void)len;
	int i,size;  
	MPI_Type_size(*type,&size); 
	size/=(int)sizeof(double); 
	for(i=0;i<size;i++) inout[i]+=in[i]; 
} 

//clock utilities
int initializeClock() {
	mytime=MPI_Wtime();
	return BC_OK;
}

int stopClock() {
	mytime = MPI_Wtime() - mytime;
	return BC_OK;
}

int printClock() {
	double tpored;
	MPI_Reduce(&mytime,&tpored,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	// @arturo 2015: Clock output format similar to other examples
	// if (mydata.myrow==0 && mydata.mycolumn==0) {
	if ( mydata.mynumber == 0 ) {
		//printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\n",mydata.numproc,mydata.numprocrow,mydata.numproccol,dimmat1,dimmat2,dimbloq1,dimbloq2,tpored);
		printf("Clock_mainClock Max: %lf\n", tpored );
	}
	return BC_OK;
}

//initialization
int initialize (int * argc, char ***argv) {
int low,high,product;
  MPI_Init(argc,argv);  

  MPI_Comm_size(MPI_COMM_WORLD,&mydata.numproc); 
  MPI_Comm_rank(MPI_COMM_WORLD,&mydata.mynumber); 

  low = high = (int)sqrt((double)mydata.numproc); 
  product = low*high; 
  /* MOVE UP/DOWN TO SEARCH THE NEAREST FACTORS */
  while ( product != mydata.numproc ) { 
    if (product < mydata.numproc) { 
      high++; 
      product += low; 
    } 
    else { 
      low--; 
      if (low==1) high=product=mydata.numproc; 
      else product -= high; 
    } 
  } 
  mydata.numproccol=high; 
  mydata.numprocrow=low; 

  return BC_OK;
}

int numroc(int iproc, int nproc, int nblock, int blocksize, int last) {
//computes the number of rows or columns in the local part of a matrix from a processor
  int res,bloqextra;
  res=((nblock-1)/nproc)*blocksize;
  bloqextra=(nblock-1)%nproc;
  if(iproc<bloqextra){
    res+=blocksize;
  }
  else if(iproc==bloqextra){
    if (last==0) return res+blocksize;
    else res+=last;
  }
  return res;
}

int numbroc(int iproc, int nproc, int nblock) {
//computes the number of rows or columns of blocks in the local part of a matrix from a processor
  int res,bloqextra;
  res=(nblock/nproc);
  bloqextra=nblock%nproc;
  if(iproc<bloqextra){
    res++;
  }
  return res;
}

//resources release
int freeAll() {
  int i;
  MPI_Comm_free(&mydata.comfilas);  
  MPI_Comm_free(&mydata.comcolumnas); 
  MPI_Type_free(&TBLOQUEV); 
  MPI_Type_free(&TBLOQUEM); 
  MPI_Op_free(&sumavec); 
  for(i=0;i<numbroc(mydata.myrow,mydata.numproccol,(dimmat1-1)/dimbloq1+1);i++) free(buffer[i]); 
  for(i=0;i<numbroc(mydata.mycolumn,mydata.numprocrow,(dimmat2-1)/dimbloq2+1);i++) free(buffer2[i]); 
  return BC_OK;
}

//matrix resources release
int freeMat (MATRIX * mat) {
  int i,j;
  for(i=0;i<mat->mynblock1;i++) for(j=0;j<mat->mynblock2;j++) free(mat->mat[i*mat->mynblock2+j]);  
  free(mat->mat);
  return BC_OK;
}


int initializeMat(MATRIX * mat, int n1, int n2, int tb1, int tb2, int * nb1, int * nb2) {
	 //initializes matrix structure
  int i,j;
  dimmat1=n1;
  dimmat2=n2;
  dimbloq1=tb1;
  dimbloq2=tb2;
	
	MPI_Comm_split(MPI_COMM_WORLD,mydata.mynumber%mydata.numproccol,mydata.mynumber,&mydata.comfilas);
	MPI_Comm_split(MPI_COMM_WORLD,mydata.mynumber/mydata.numproccol,mydata.mynumber,&mydata.comcolumnas);
	
	MPI_Comm_rank(mydata.comfilas,&mydata.mycolumn);
	MPI_Comm_rank(mydata.comcolumnas,&mydata.myrow);

  mat->dim1=n1;
  mat->dim2=n2;
  mat->blocksize1=tb1;
  mat->blocksize2=tb2;
  mat->blocksize=tb1*tb2;
  mat->nblock1=(n1-1)/tb1 + 1; //ceil
  mat->nblock2=(n2-1)/tb2 + 1; //ceil
  if ((mat->last1 = (n1 % tb1))==0) mat->last1=tb1;
  if ((mat->last2 = (n2 % tb2))==0) mat->last2=tb2;
  if (nb1!=NULL) *nb1=mat->nblock1;
  if (nb2!=NULL) *nb2=mat->nblock2;

  mat->mydim1=numroc(mydata.myrow,mydata.numproccol,mat->nblock1,mat->blocksize1,mat->last1); 
  mat->mydim2=numroc(mydata.mycolumn,mydata.numprocrow,mat->nblock2,mat->blocksize2,mat->last2); 
  mat->mynblock1=numbroc(mydata.myrow,mydata.numproccol,mat->nblock1); 
  mat->mynblock2=numbroc(mydata.mycolumn,mydata.numprocrow,mat->nblock2); 
  mat->mylast1=lastsize(mydata.myrow,mydata.numproccol,mat->nblock1,mat->last1,mat->blocksize1); 
  mat->mylast2=lastsize(mydata.mycolumn,mydata.numprocrow,mat->nblock2,mat->last2,mat->blocksize2); 

  //we reserve space for entire blocks: padding with 0's
  mat->mat=calloc((size_t)(mat->mynblock1*mat->mynblock2),sizeof(double *)); 
  
  for(i=0;i<mat->mynblock1;i++) { 
    for(j=0;j<mat->mynblock2;j++) { 
      matrixBlockAt(mat,i,j)=calloc((size_t)(mat->blocksize1*mat->blocksize2),sizeof(double)); 
    } 
  } 

  buffer=calloc((size_t)mat->mynblock1,sizeof(double*)); 
  for(j=0;j<mat->mynblock1;j++) { 
    buffer[j]=calloc((size_t)(mat->blocksize1*mat->blocksize2),sizeof(double)); 
  } 


  buffer2=calloc((size_t)mat->mynblock2,sizeof(double*)); 
  for(j=0;j<mat->mynblock2;j++) { 
    buffer2[j]=calloc((size_t)(mat->blocksize1*mat->blocksize2),sizeof(double)); 
  } 

  MPI_Type_contiguous(tb1*tb2,MPI_DOUBLE,&TBLOQUEM); 
  MPI_Type_commit(&TBLOQUEM); 

  sizes = calloc((size_t)max(mat->mynblock1,mat->mynblock2),sizeof(int)); 
  addresses = calloc((size_t)max(mat->mynblock1,mat->mynblock2),sizeof(MPI_Aint)); 
  types = calloc((size_t)max(mat->mynblock1,mat->mynblock2),sizeof(MPI_Datatype)); 
  for(j=0;j<max(mat->mynblock1,mat->mynblock2);j++) {types[j]=TBLOQUEM; sizes[j]=1;} 

  return BC_OK;
}

int initializeVec (MATRIX * mat, int n, int tb, int * nb) {
	 //initializes vector structure
  int i,j;
  mat->dim1=1;
  mat->dim2=n;
  mat->blocksize1=1;
  mat->blocksize2=tb;
  mat->blocksize=tb;
  mat->nblock1=1; 
  mat->nblock2=(n-1)/tb + 1; //ceil
  mat->last1 = 1;
  if ((mat->last2 = (n % tb))==0) mat->last2=tb;
  if (nb!=NULL) *nb=mat->nblock1;

  mat->mydim1=numroc(mydata.myrow,mydata.numproccol,mat->nblock1,mat->blocksize1,mat->last1); 
  mat->mydim2=numroc(mydata.mycolumn,mydata.numprocrow,mat->nblock2,mat->blocksize2,mat->last2); 
  mat->mynblock1=numbroc(mydata.myrow,mydata.numproccol,mat->nblock1); 
  mat->mynblock2=numbroc(mydata.mycolumn,mydata.numprocrow,mat->nblock2); 
  mat->mylast1=lastsize(mydata.myrow,mydata.numproccol,mat->nblock1,mat->last1,mat->blocksize1); 
  mat->mylast2=lastsize(mydata.mycolumn,mydata.numprocrow,mat->nblock2,mat->last2,mat->blocksize2); 

  //we reserve space for entire blocks: padding with 0's
  mat->mat=calloc((size_t)(mat->mynblock1*mat->mynblock2),sizeof(double *)); 
  
  for(i=0;i<mat->mynblock1;i++) { 
    for(j=0;j<mat->mynblock2;j++) { 
      matrixBlockAt(mat,i,j)=calloc((size_t)(mat->blocksize1*mat->blocksize2),sizeof(double)); 
    } 
  } 

  MPI_Type_contiguous(tb,MPI_DOUBLE,&TBLOQUEV); 
  MPI_Type_commit(&TBLOQUEV); 

  MPI_Op_create((MPI_User_function*)vectorAdd,1,&sumavec); 

  return BC_OK;
}

int randomMat(MATRIX * mat, int seed, double min, double max) {
		//random initialization of a matrix using a seed
  int i,j,k,l,tam1,tam2,mio; 
  double * punteroM;

  if (min>=max) return BC_ERROR;
  srand48((long)seed);
  for(i=0;i<mat->nblock1;i++) {
    tam1=(i<mat->nblock1-1)?mat->blocksize1:mat->last1;
    mio=ismine(i,mydata.myrow,mydata.numproccol);
    for(k=0;k<tam1;k++) {
      for(j=0;j<mat->nblock2;j++) {
        tam2=(j<mat->nblock2-1)?mat->blocksize2:mat->last2;
        if (ismine(j,mydata.mycolumn,mydata.numprocrow) && mio) {
          punteroM=localMatrixBlockAt(mat,i,j);
          for (l=0;l<tam2;l++) punteroM[k*mat->blocksize2+l]=drand48()*(max-min)+min;
        }
        else {
          for (l=0;l<tam2;l++) drand48();
        }
      }
    }
  }
  return BC_OK;
}

int print(MATRIX * mat, const char * matrixName) {
	//prints a matrix
  int i,j,k,l,mio,tam1,tam2,offset1;
  // int offset2;
  double * buf,*punteroM;

  // @arturo 2015: Output compatible with check script
  MPI_Barrier( MPI_COMM_WORLD );
  FILE *f = fopen( matrixName, "w+" );
  if ( f == NULL ) {
	  fprintf(stderr,"Error: Opening matrix file %s\n", matrixName );
	  MPI_Finalize();
	  exit( EXIT_FAILURE );
  }
  MPI_Barrier( MPI_COMM_WORLD );

  if (mydata.myrow==0 && mydata.mycolumn==0) {
    // @arturo 2015: Output compatible with check script
	// putc(10,stdout);
    buf=calloc((size_t)mat->blocksize2,sizeof(double));

    for(i=0;i<mat->nblock1;i++) {
      tam1=(i<mat->nblock1-1)?mat->blocksize1:mat->last1;
      mio=ismine(i,0,mydata.numproccol);
      offset1=i*mat->blocksize1;
      for(k=0;k<tam1;k++) {
        for(j=0;j<mat->nblock2;j++) {
          tam2=(j<mat->nblock2-1)?mat->blocksize2:mat->last2;
          //offset2=j*mat->blocksize2;
          if (ismine(j,0,mydata.numprocrow) && mio) {
            punteroM=localMatrixBlockAt(mat,i,j);
            for (l=0;l<tam2;l++) 
                // @arturo 2015: Output compatible with check script
				//fprintf(f,"%s(%d,%d)=%f\n",matrixName,offset1,offset2+l,punteroM[l+k*mat->blocksize2]);
				fprintf(f,"%016.6lf\n",punteroM[l+k*mat->blocksize2]);
          }
          else {
            MPI_Recv(buf,tam2,MPI_DOUBLE,i%mydata.numproccol+(j%mydata.numprocrow)*mydata.numproccol,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);
            for (l=0;l<tam2;l++) 
                // @arturo 2015: Output compatible with check script
				//fprintf(f,"%s(%d,%d)=%f\n",matrixName,offset1,offset2+l,buf[l]);
				fprintf(f,"%016.6lf\n",buf[l]);
          }
        }
        offset1++;
        // @arturo 2015: Output compatible with check script
        //putc(10,stdout);
      }
    }
    free(buf);
    // @arturo 2015: Output compatible with check script
    //fflush(stdout);
	fclose(f);
  }
  else {
    for(i=0;i<mat->nblock1;i++) {
      tam1=(i<mat->nblock1-1)?mat->blocksize1:mat->last1;
      mio=ismine(i,mydata.myrow,mydata.numproccol);
      for(k=0;k<tam1;k++) {
        for(j=0;j<mat->nblock2;j++) {
          tam2=(j<mat->nblock2-1)?mat->blocksize2:mat->last2;
          if (ismine(j,mydata.mycolumn,mydata.numprocrow) && mio) {
            punteroM=localMatrixBlockAt(mat,i,j);
            MPI_Send(&punteroM[k*mat->blocksize2],tam2,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
          }
        }
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return BC_OK;
}


int printVec(MATRIX * mat, const char * matrixName) {
		//prints a vector
  int j,l,tam2;
  // int offset2;
  double * buf,*punteroM;

  // @arturo 2015: Output compatible with check script
  MPI_Barrier( MPI_COMM_WORLD );
  FILE *f = fopen( matrixName, "w+" );
  if ( f == NULL ) {
	  fprintf(stderr,"Error: Opening vector file %s\n", matrixName );
	  MPI_Finalize();
	  exit( EXIT_FAILURE );
  }
  MPI_Barrier( MPI_COMM_WORLD );

  if (mydata.myrow==0 && mydata.mycolumn==0) {
    // @arturo 2015: Output compatible with check script
    //putc(10,stdout);
    buf=calloc((size_t)mat->blocksize2,sizeof(double));

    for(j=0;j<mat->nblock2;j++) {
      tam2=(j<mat->nblock2-1)?mat->blocksize2:mat->last2;
      //offset2=j*mat->blocksize2;
      if (ismine(j,0,mydata.numprocrow)) {
        punteroM=localMatrixBlockAt(mat,0,j);
        for (l=0;l<tam2;l++) 
            // @arturo 2015: Output compatible with check script
			//fprintf(f,"%s(%d,%d)=%f\n\n",matrixName,offset2+l,0,punteroM[l]);
			fprintf(f,"%016.6lf\n",punteroM[l]);
      }
      else {
        MPI_Recv(buf,tam2,MPI_DOUBLE,(j%mydata.numprocrow)*mydata.numproccol,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);
        for (l=0;l<tam2;l++) 
            // @arturo 2015: Output compatible with check script
			//fprintf(f,"%s(%d,%d)=%f\n\n",matrixName,offset2+l,0,buf[l]);
			fprintf(f,"%016.6lf\n",buf[l]);
      }
    }
    free(buf);
    // @arturo 2015: Output compatible with check script
    //fflush(stdout);
	fclose(f);
  }
  else if (mydata.myrow==0) {
    for(j=0;j<mat->nblock2;j++) {
      tam2=(j<mat->nblock2-1)?mat->blocksize2:mat->last2;
      if (ismine(j,mydata.mycolumn,mydata.numprocrow)) {
        punteroM=localMatrixBlockAt(mat,0,j);
        MPI_Send(punteroM,tam2,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return BC_OK;
}

void strsmu(int m, int n, double * a, double * b, int lda, int ldb) {
//solves AX=B /  A MxM matrix, X and B MxN matrices
//A is upper triangular
//solution in B
//requires m,n>=0

  int j,k,i;
  double temp;

  if (m==0 || n==0) return;

  for(k=m-1;k>=0;k--) {
    for(i=m-1;i>k;i--) {
      if((temp=-a[k*lda+i])!=0.0) {
        for (j=0;j<n;j++) {
#ifdef DEBUG
printf("RUT: STRSMU, PROC %d;%d    ",mydata.myrow,mydata.mycolumn);
printf("%lf -= %lf*%lf = %lf\n",b[k*ldb+j],-temp,b[i*ldb+j],b[k*ldb+j]+temp*b[i*ldb+j]);
#endif
          b[k*ldb+j]+=temp*b[i*ldb+j];
        }
      }
    }
    temp=a[k*lda+k];
    for(j=0;j<n;j++) {
#ifdef DEBUG
printf("RUT: STRSMU, PROC %d;%d    ",mydata.myrow,mydata.mycolumn);
printf("%lf /= %lf = %lf\n",b[k*ldb+j],temp,b[k*ldb+j]/temp);
#endif
      b[k*ldb+j]/=temp;
    }
  }
}

void strsml(int m, int n, double * a, double * b, int lda, int ldb) {
//solves AX=B /  A MxM matrix, X and B MxN matrices
//A is lower triangular with 1's in the diagonal
//solution in B
//requires m,n>=0	
	
  int j,k,i;
  double temp;

  if (m==0 || n==0) return;

  for(k=0;k<m;k++) {
    for(i=0;i<k;i++) {
      if((temp=-a[k*lda+i])!=0.0) {
        for (j=0;j<n;j++) {
#ifdef DEBUG
printf("RUT: STRSML, PROC %d;%d    ",mydata.myrow,mydata.mycolumn);
printf("%lf -= %lf*%lf = %lf\n",b[k*ldb+j],-temp,b[i*ldb+j],b[k*ldb+j]+temp*b[i*ldb+j]);
#endif
          b[k*ldb+j]+=temp*b[i*ldb+j];
        }
      }
    }
  }
}

void strsml2(int m, int n, double * a, double * b, int lda, int ldb) {
//solves XA=B /  A NxN matrix, X and B MxN matrices
//A is lower triangular with 1's in the diagonal
//solution in B
//requires m,n>=0	

  int j,k,i;
  double temp;

  if (m==0 || n==0) return;

  for(k=0;k<n;k++) {
    for(j=0;j<m;j++) {
      if (b[j*ldb+k]!=0.0) {
#ifdef DEBUG
printf("RUT: STRSML2, PROC %d;%d    ",mydata.myrow,mydata.mycolumn);
printf("%lf /= %lf = %lf\n",b[j*ldb+k],a[k*lda+k],b[j*ldb+k]/a[k*lda+k]);
#endif
        b[j*ldb+k]/=a[k*lda+k];
        temp=-b[j*ldb+k];
        for(i=k+1;i<n;i++) {
#ifdef DEBUG
printf("RUT: STRSML2, PROC %d;%d    ",mydata.myrow,mydata.mycolumn);
printf("%lf -= %lf*%lf = %lf\n",b[j*ldb+i],-temp,a[k*lda+i],b[j*ldb+i]+temp*a[k*lda+i]);
#endif
          b[j*ldb+i]+=temp*a[k*lda+i];
        }
      }
    }
  }
}

void sgemm (int m, int n, int k, double * a, double * b, double * c, int lda, int ldb, int ldc) {
//solves C=C-AB /  A MxK matrix, B KxN matrix and C MxN matrix
//requires m,n,k>=0		
  double temp;
  int i,j,l;

  if (n==0 || m==0 || k==0) return;

  for(i=0;i<m;i++) {
    for(l=0;l<k;l++) {
      if((temp=-a[i*lda+l])!=0.0) {
        for(j=0;j<n;j++) {
#ifdef DEBUG
printf("RUT: SGEMM, PROC %d;%d    ",mydata.myrow,mydata.mycolumn);
printf("%lf -= %lf*%lf = %lf\n",c[i*ldc+j],-temp,b[l*ldb+j],c[i*ldc+j]+temp*b[l*ldb+j]);
#endif
          c[i*ldc+j]+=temp*b[l*ldb+j];
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////7

void factorizeMat(MATRIX * mat, int m, int n) {
	//factorizes A in L*U
	
  int i,j,k,ii,jj,min,tam1,tam2,tam3,p1,p2,b1,b2,bb2,bb1,bini;
  double *b, *c, temp, *buf;
  min=min(m,n);
  MPI_Datatype tbloqfil, tbloqfilrcv, tbloqcol, tbloqcolrcv, tcol; 

  p1=0;
  p2=0;
  b1=0;
  b2=0;

  buf=calloc((size_t)mat->blocksize1,sizeof(double));

  #define lda mat->blocksize2

  for(i=0;i<min;i++) {
    bb1=(mydata.myrow>p1)?b1:b1+1;
    bb2=(mydata.mycolumn>p2)?b2:b2+1;

    tam1=(i<mat->nblock1-1)?mat->blocksize1:mat->last1;
    if(bb2<mat->mynblock2) {
	  if (b1<mat->mynblock1) {
			for(j=bb2;j<mat->mynblock2;j++) {
				MPI_Address(matrixBlockAt(mat,b1,j),&addresses[j]); 
			}
			for(j=mat->mynblock2-1;j>=bb2;j--) {
				addresses[j]-=addresses[bb2]; 
			}
			MPI_Type_create_hindexed(mat->mynblock2-bb2,sizes,&addresses[bb2],TBLOQUEM,&tbloqfil); 
			MPI_Type_commit(&tbloqfil); 
	  }
      for(j=bb2;j<mat->mynblock2;j++) {
        MPI_Address(buffer2[j],&addresses[j]); 
      }
      for(j=mat->mynblock2-1;j>=bb2;j--) {
        addresses[j]-=addresses[bb2]; 
      }
      MPI_Type_create_hindexed(mat->mynblock2-bb2,sizes,&addresses[bb2],TBLOQUEM,&tbloqfilrcv); 
      MPI_Type_commit(&tbloqfilrcv); 
    }
    if(bb1<mat->mynblock1) {
	  if (b2<mat->mynblock2) {

		  for(j=bb1;j<mat->mynblock1;j++) {
			  MPI_Address(matrixBlockAt(mat,j,b2),&addresses[j]); 
		  }
		  for(j=mat->mynblock1-1;j>=bb1;j--) {
			  addresses[j]-=addresses[bb1]; 
		  }
		  MPI_Type_create_hindexed(mat->mynblock1-bb1,sizes,&addresses[bb1],TBLOQUEM,&tbloqcol); 
		  MPI_Type_commit(&tbloqcol); 
	  }
	  for(j=bb1;j<mat->mynblock1;j++) {
        MPI_Address(buffer[j],&addresses[j]); 
      }
      for(j=mat->mynblock1-1;j>=bb1;j--) {
        addresses[j]-=addresses[bb1]; 
      }
      MPI_Type_create_hindexed(mat->mynblock1-bb1,sizes,&addresses[bb1],TBLOQUEM,&tbloqcolrcv); 
      MPI_Type_commit(&tbloqcolrcv); 
    }
    ///////////////////////////////
    //process with diagonal block//
    ///////////////////////////////
    if (mydata.myrow==p1 && mydata.mycolumn==p2) {
      //factorize diagonal block and blocks on its right
      b=matrixBlockAt(mat,b1,b2);

      for(k=0;k<tam1-1;k++){
        if (b[k*lda+k]==0.0) {        
          fprintf(stderr,"0.0 in position %d,%d\n",k+i*mat->blocksize1,k+i*mat->blocksize1);
        }
        for(ii=k+1;ii<tam1;ii++) {
          //L multiplier
          if(b[ii*lda+k]!=0.0) {
#ifdef DEBUG
printf("RUT: FACT, PROC %d;%d  ",mydata.myrow,mydata.mycolumn);
printf("%lf /= %lf = %lf\n",b[ii*lda+k],b[k*lda+k],b[ii*lda+k]/b[k*lda+k]);
#endif
            b[ii*lda+k]/=b[k*lda+k];
          }
        }
		MPI_Type_vector(tam1-k-1,1,lda,MPI_DOUBLE,&tcol); 
		MPI_Type_commit(&tcol); 
		MPI_Bcast(&b[(k+1)*lda+k],1,tcol,p2,mydata.comfilas); 
        //update trailing matrix
        for(ii=k+1;ii<tam1;ii++) {
          if((temp=-b[ii*lda+k])!=0.0) {
            tam3=(b2<mat->nblock2-1)?mat->blocksize2:mat->last2;
            for(jj=k+1;jj<tam3;jj++) {
#ifdef DEBUG
printf("RUT: FACT, PROC %d;%d  ",mydata.myrow,mydata.mycolumn);
printf("%lf -= %lf*%lf = %lf\n",b[ii*lda+jj],-temp,b[k*lda+jj],b[ii*lda+jj]+temp*b[k*lda+jj]);
#endif
              b[ii*lda+jj]+=temp*b[k*lda+jj];
            }
            for(j=b2+1;j<mat->mynblock2;j++) {
              tam3=(j<mat->mynblock2-1)?mat->blocksize2:mat->mylast2;
              c=matrixBlockAt(mat,b1,j);
              for(jj=0;jj<tam3;jj++) {
#ifdef DEBUG
printf("RUT: FACT, PROC %d;%d  ",mydata.myrow,mydata.mycolumn);
printf("%lf -= %lf*%lf = %lf\n",c[ii*lda+jj],-temp,c[k*lda+jj],c[ii*lda+jj]+temp*c[k*lda+jj]);
#endif
                c[ii*lda+jj]+=temp*c[k*lda+jj];
              }
            }
          }
        }
		MPI_Type_free(&tcol); 
      }
	  if (b[(tam1-1)*lda+tam1-1]==0.0) {        
			fprintf(stderr,"Hay un 0.0 en la posicion %d,%d\n",tam1-1+i*mat->blocksize1,tam1-1+i*mat->blocksize1);
	  }

      /////////////////////////////////////////////////////////////

         //sending of diagonal block
      MPI_Bcast(b,1,TBLOQUEM,p1,mydata.comcolumnas); 

	    //column blocks behind diagonal block
      for(j=b1+1;j<mat->mynblock1;j++) {
        //a(j,i)/=a(i,i)
        tam2=(j<mat->mynblock1-1)?mat->blocksize1:mat->mylast1; 
        c=matrixBlockAt(mat,j,b2);  
        strsml2(tam2,tam1,b,c,mat->blocksize2,mat->blocksize2);
      }

      /////////////////////////////////////////////////////////////

      if(b2+1<mat->mynblock2) {
         //sending of row blocks
        MPI_Bcast(matrixBlockAt(mat,b1,b2+1),1,tbloqfil,p1,mydata.comcolumnas); 
      }
      if(b1+1<mat->mynblock1) {
         //sending of column blocks
        MPI_Bcast(matrixBlockAt(mat,b1+1,b2),1,tbloqcol,p2,mydata.comfilas); 
      }

      //rest of blocks
	  //column blocks
      for(j=b1+1;j<mat->mynblock1;j++) {
        tam2=(j<mat->mynblock1-1)?mat->blocksize1:mat->mylast1; 
        c=matrixBlockAt(mat,j,b2);  
	    //row blocks
        for(k=b2+1;k<mat->mynblock2;k++) {
	     //a(j,k)-=a(j,i)*a(i,k)
          tam3=(k<mat->mynblock2-1)?mat->blocksize2:mat->mylast2;
          sgemm(tam2,tam3,tam1,c,matrixBlockAt(mat,b1,k),matrixBlockAt(mat,j,k),mat->blocksize2,mat->blocksize2,mat->blocksize2);	  
        }
      }
    }
    //////////////////////////////////////////////
    //processes with blocks below diagonal block//
    //////////////////////////////////////////////
    else if (mydata.mycolumn==p2) {
      //reception of diagonal blocks
        MPI_Bcast(buffer2[b2],1,TBLOQUEM,p1,mydata.comcolumnas);  	                     

	    //column blocks below diagonal block
        b=buffer2[b2];
        for(j=bb1;j<mat->mynblock1;j++) {
          //a(j,i)/=a(i,i)
          tam2=(j<mat->mynblock1-1)?mat->blocksize1:mat->mylast1; 
          strsml2(tam2,tam1,b,matrixBlockAt(mat,j,b2),mat->blocksize2,mat->blocksize2);
        }

      /////////////////////////////////////////////////////////////
      //reception of row blocks
        if (b2+1<mat->mynblock2) {
          MPI_Bcast(buffer2[b2+1],1,tbloqfilrcv,p1,mydata.comcolumnas);  	                     
        }

         //sending of column blocks
        if(bb1<mat->mynblock1){
          MPI_Bcast(matrixBlockAt(mat,bb1,b2),1,tbloqcol,p2,mydata.comfilas); 
        }

        //rest of blocks
        //column blocks
        for(j=bb1;j<mat->mynblock1;j++) {
          tam2=(j<mat->mynblock1-1)?mat->blocksize1:mat->mylast1; 
          c=matrixBlockAt(mat,j,b2);       
	       //row blocks
          for(k=b2+1;k<mat->mynblock2;k++) {
            //a(j,k)-=a(j,i)*a(i,k)
            tam3=(k<mat->mynblock2-1)?mat->blocksize2:mat->mylast2;
            sgemm(tam2,tam3,tam1,c,buffer2[k],matrixBlockAt(mat,j,k),mat->blocksize2,mat->blocksize2,mat->blocksize2);	  
          }
        }
    }
    ////////////////////////////////////////////////////////
    //processes with blocks on the right of diagonal block//
    ////////////////////////////////////////////////////////
    else if (mydata.myrow==p1) {
        bini=(mydata.mycolumn>p2)?b2:b2+1;
         //factorize blocks on the right of diagonal block
        for(k=0;k<tam1-1;k++) {
         //reception of column
		  MPI_Type_contiguous(tam1-k-1,MPI_DOUBLE,&tcol); 
		  MPI_Type_commit(&tcol); 
		  MPI_Bcast(buf,1,tcol,p2,mydata.comfilas); 
          //update trailing matrix
          for(ii=k+1;ii<tam1;ii++) {
            //L multiplier
            if((temp=-buf[ii-k-1])!=0.0) {
              for(j=bini;j<mat->mynblock2;j++) {
	            c=matrixBlockAt(mat,b1,j);
                tam3=(j<mat->mynblock2-1)?mat->blocksize2:mat->mylast2;
                for(jj=0;jj<tam3;jj++) {
#ifdef DEBUG
printf("RUT: FACT, PROC %d;%d  ",mydata.myrow,mydata.mycolumn);
printf("%lf -= %lf*%lf = %lf\n",c[ii*lda+jj],-temp,c[k*lda+jj],c[ii*lda+jj]+temp*c[k*lda+jj]);
#endif
                  c[ii*lda+jj]+=temp*c[k*lda+jj];
                }
              }
            }
          }
		  MPI_Type_free(&tcol); 
        }

        ///////////////////////////////////////////////////////
        if(bb2<mat->mynblock2) {
         //sending of row blocks
          MPI_Bcast(matrixBlockAt(mat,b1,bb2),1,tbloqfil,p1,mydata.comcolumnas); 
        }
        if(b1+1<mat->mynblock1) {
         //reception of column blocks
          MPI_Bcast(buffer[b1+1],1,tbloqcolrcv,p2,mydata.comfilas); 
        }
        //rest of blocks
        //column blocks
        for(j=b1+1;j<mat->mynblock1;j++) {
          tam2=(j<mat->mynblock1-1)?mat->blocksize1:mat->mylast1;
          c=buffer[j];      
          //row blocks
          for(k=bb2;k<mat->mynblock2;k++) {
            //a(j,k)-=a(j,i)*a(i,k)
            tam3=(k<mat->mynblock2-1)?mat->blocksize2:mat->mylast2;
            sgemm(tam2,tam3,tam1,c,matrixBlockAt(mat,b1,k),matrixBlockAt(mat,j,k),mat->blocksize2,mat->blocksize2,mat->blocksize2);	  
          }
        }
    }
    /////////////////////
    //rest of processes//
    /////////////////////
    else {
        //reception of row blocks
        if(bb2<mat->mynblock2) {
          MPI_Bcast(buffer2[bb2],1,tbloqfilrcv,p1,mydata.comcolumnas);  	            
        }
        if(bb1<mat->mynblock1) {
          //reception of column blocks
          MPI_Bcast(buffer[bb1],1,tbloqcolrcv,p2,mydata.comfilas); 
        }
        //column blocks
        for(j=bb1;j<mat->mynblock1;j++) {
          tam2=(j<mat->mynblock1-1)?mat->blocksize1:mat->mylast1;
          c=buffer[j];                 
         //row blocks
          for(k=bb2;k<mat->mynblock2;k++) {
	        //a(j,k)-=a(j,i)*a(i,k)
            tam3=(k<mat->mynblock2-1)?mat->blocksize2:mat->mylast2;
            sgemm(tam2,tam3,tam1,c,buffer2[k],matrixBlockAt(mat,j,k),mat->blocksize2,mat->blocksize2,mat->blocksize2);	  
          }
        }
    }
    if(bb2<mat->mynblock2) {
      if (b1<mat->mynblock1) MPI_Type_free(&tbloqfil);
      MPI_Type_free(&tbloqfilrcv); 
    }
    if(bb1<mat->mynblock1) {
      if (b2<mat->mynblock2) MPI_Type_free(&tbloqcol);
      MPI_Type_free(&tbloqcolrcv); 
    }
    p1=(p1+1)%mydata.numproccol;
    p2=(p2+1)%mydata.numprocrow;
    if (p1==0) b1++;
    if (p2==0) b2++;
  }
  free(buf);
}

void solveMat(MATRIX * mat, MATRIX * matb, int n) {
	//solves the system Ax=B

  int p1,p2,b1,b2,bb2,i,j,tam1,tam2;
  double *b,*c,*temp;
  MPI_Datatype tbloqfil, tbloqfilrcv; 

  p1=0;
  p2=0;
  b1=0;
  b2=0;

  temp=calloc((size_t)matb->blocksize2,sizeof(double)); 

  //solves Ly=b
  for(i=0;i<n;i++) {
  	//MPI type creation
    tam1=(i<mat->nblock1-1)?mat->blocksize1:mat->last1;
    bb2=(mydata.mycolumn>p2)?b2-1:b2;
    if(bb2>=0) {
	  if (b1<mat->mynblock1) {
        for(j=0;j<=bb2;j++) {
          MPI_Address(matrixBlockAt(mat,b1,j),&addresses[j]);  
        }
        for(j=bb2;j>=0;j--) {
          addresses[j]-=addresses[0];  
        }
        MPI_Type_create_hindexed(bb2+1,sizes,addresses,TBLOQUEM,&tbloqfil); 
        MPI_Type_commit(&tbloqfil); 
	  }
      for(j=0;j<=bb2;j++) {
        MPI_Address(buffer2[j],&addresses[j]); 
      }
      for(j=bb2;j>=0;j--) {
        addresses[j]-=addresses[0]; 
      }
      MPI_Type_create_hindexed(bb2+1,sizes,addresses,TBLOQUEM,&tbloqfilrcv); 
      MPI_Type_commit(&tbloqfilrcv); 
    }

    /////////////////////////////////////////////
    //process with the i-th block of the vector//
    /////////////////////////////////////////////
    if (mydata.myrow==0 && mydata.mycolumn==p2) {
      b=matrixBlockAt(matb,0,b2);
      if (p1==0) { //blocks of matrix are also mine
        c=matrixBlockAt(mat,b1,b2);
        for(j=b2-1;j>=0;j--) {
          //b(i)-=b(j)*a(i,j)
          sgemm(tam1,1,matb->blocksize2,matrixBlockAt(mat,b1,j),matrixBlockAt(matb,0,j),b,mat->blocksize2,1,1);
        }
      }
      else {
        //matrix blocks reception  
        if(b2>=0) {
          MPI_Recv(buffer2[0],1,tbloqfilrcv,p1,i,mydata.comcolumnas,&estado);  
        }
        c=buffer2[b2];
        for(j=b2-1;j>=0;j--) {
          //b(i)-=b(j)*a(i,j)
          tam2=(j<matb->mynblock1-1)?matb->blocksize1:matb->mylast1;     
          sgemm(tam1,1,matb->blocksize2,buffer2[j],matrixBlockAt(matb,0,j),b,mat->blocksize2,1,1);
        }
      }
      //Reduction of partial results
      for(j=0;j<matb->blocksize2;j++) temp[j]=b[j]; 
      MPI_Reduce(temp,b,1,TBLOQUEV,sumavec,p2,mydata.comfilas); 
      strsml(tam1,1,c,b,mat->blocksize2,1);
    }
    /////////////////////////////////////////////
    //processes with other blocks of the vector//
    /////////////////////////////////////////////
    else if (mydata.myrow==0) { //mydata.mycolumn/=p2
        //accumulator block initialized to 0
        for(j=0;j<matb->blocksize2;j++) temp[j]=0.0;
        if (p1==0) { //blocks of matrix are also mine
          for(j=bb2;j>=0;j--) {
            //b(i)-=b(j)*a(i,j)
            sgemm(tam1,1,matb->blocksize2,matrixBlockAt(mat,b1,j),matrixBlockAt(matb,0,j),temp,mat->blocksize2,1,1);
          }
        }
        else {
        //matrix blocks reception  
          if(bb2>=0) {
            MPI_Recv(buffer2[0],1,tbloqfilrcv,p1,i,mydata.comcolumnas,&estado);  
          }
          for(j=bb2;j>=0;j--) {
            //b(i)-=b(j)*a(i,j) 
            sgemm(tam1,1,matb->blocksize2,buffer2[j],matrixBlockAt(matb,0,j),temp,mat->blocksize2,1,1);
          }
        }
        //sending of partial result
	    MPI_Reduce(temp,NULL,1,TBLOQUEV,sumavec,p2,mydata.comfilas); 
    }
    ////////////////////////////////////////////////////////////
    //processs with blocks in the i-th block row of the matrix//
    ////////////////////////////////////////////////////////////
    else if (mydata.myrow==p1) {
      //p2!=0
      if(bb2>=0) {
        //block sending
        MPI_Send(matrixBlockAt(mat,b1,0),1,tbloqfil,0,i,mydata.comcolumnas); 
      }
    }
    if(bb2>=0) {
      if (b1<mat->mynblock1) MPI_Type_free(&tbloqfil); 
      MPI_Type_free(&tbloqfilrcv); 
    }
    p1=(p1+1)%mydata.numproccol;
    p2=(p2+1)%mydata.numprocrow;
    if (p1==0) b1++;
    if (p2==0) b2++;
  }
  p1=(p1==0)?(mydata.numproccol-1):(p1-1);
  p2=(p2==0)?(mydata.numprocrow-1):(p2-1);
  if (p1==mydata.numproccol-1) b1--;
  if (p2==mydata.numprocrow-1) b2--;
  //solves Ux=y
  for(i=n-1;i>=0;i--) {
  	//MPI Type creaction
    bb2=(mydata.mycolumn>=p2)?b2:b2+1;
    tam1=(i<mat->nblock1-1)?mat->blocksize1:mat->last1;
    if(bb2<mat->mynblock2) {
	  if (b1<mat->mynblock1) {
        for(j=bb2;j<mat->mynblock2;j++) {
          MPI_Address(matrixBlockAt(mat,b1,j),&addresses[j]); 
        }
        for(j=mat->mynblock2-1;j>=bb2;j--) {
          addresses[j]-=addresses[bb2]; 
        }
        MPI_Type_create_hindexed(mat->mynblock2-bb2,sizes,&addresses[bb2],TBLOQUEM,&tbloqfil); 
        MPI_Type_commit(&tbloqfil); 
	  }
      for(j=bb2;j<mat->mynblock2;j++) {
        MPI_Address(buffer2[j],&addresses[j]); 
      }
      for(j=mat->mynblock2-1;j>=bb2;j--) {
        addresses[j]-=addresses[bb2]; 
      }
      MPI_Type_create_hindexed(mat->mynblock2-bb2,sizes,&addresses[bb2],TBLOQUEM,&tbloqfilrcv); 
      MPI_Type_commit(&tbloqfilrcv); 
    }

    /////////////////////////////////////////////
    //process with the i-th block of the vector//
    /////////////////////////////////////////////
    if (mydata.myrow==0 && mydata.mycolumn==p2) {
      b=matrixBlockAt(matb,0,b2);
      if (p1==0) { //blocks of matrix are also mine
        for(j=b2+1;j<matb->mynblock2;j++) {
          //b(i)-=b(j)*a(i,j)
          tam2=(j<matb->mynblock2-1)?matb->blocksize2:matb->mylast2;      
          sgemm(tam1,1,tam2,matrixBlockAt(mat,b1,j),matrixBlockAt(matb,0,j),b,mat->blocksize2,1,1);
        }
        c=matrixBlockAt(mat,b1,b2);     
      }
      else {
        //matrix blocks reception  
        if(b2<matb->mynblock2) {
          MPI_Recv(buffer2[b2],1,tbloqfilrcv,p1,i,mydata.comcolumnas,&estado);  
        } 
        c=buffer2[b2];
        for(j=b2+1;j<matb->mynblock2;j++) {
          //b(i)-=b(j)*a(i,j)
          tam2=(j<matb->mynblock2-1)?matb->blocksize2:matb->mylast2;      
          sgemm(tam1,1,tam2,buffer2[j],matrixBlockAt(matb,0,j),b,mat->blocksize2,1,1);
        }
      }
      //Reduction of partial results
      for(j=0;j<matb->blocksize2;j++) temp[j]=b[j]; 
      MPI_Reduce(temp,b,1,TBLOQUEV,sumavec,p2,mydata.comfilas); 
      strsmu(tam1,1,c,b,mat->blocksize2,1);
    }
    /////////////////////////////////////////////
    //processes with other blocks of the vector//
    /////////////////////////////////////////////
    else if (mydata.myrow==0) { //mydata.mycolumn/=p2
        //accumulator block initialized to 0
        for(j=0;j<matb->blocksize2;j++) temp[j]=0.0;
        if (p1==0) { //blocks of matrix are also mine
          for(j=bb2;j<matb->mynblock2;j++) {
            //b(i)-=b(j)*a(i,j)
            tam2=(j<matb->mynblock2-1)?matb->blocksize2:matb->mylast2;     
            sgemm(tam1,1,tam2,matrixBlockAt(mat,b1,j),matrixBlockAt(matb,0,j),temp,mat->blocksize2,1,1);
          }
        }
        else {
         //matrix blocks reception  
          if(bb2<matb->mynblock2) {
            MPI_Recv(buffer2[bb2],1,tbloqfilrcv,p1,i,mydata.comcolumnas,&estado);  
          } 
         
          for(j=bb2;j<matb->mynblock2;j++) {
            //b(i)-=b(j)*a(i,j)
            tam2=(j<matb->mynblock2-1)?matb->blocksize2:matb->mylast2;     
            sgemm(tam1,1,tam2,buffer2[j],matrixBlockAt(matb,0,j),temp,mat->blocksize2,1,1);
          }
        }
        //sending of partial result
		MPI_Reduce(temp,NULL,1,TBLOQUEV,sumavec,p2,mydata.comfilas); 
    }
    ////////////////////////////////////////////////////////////
    //processs with blocks in the i-th block row of the matrix//
    ////////////////////////////////////////////////////////////
    else if (mydata.myrow==p1) {
      //p2!=0
        //block sending
      if(bb2<mat->mynblock2) {
        MPI_Send(matrixBlockAt(mat,b1,bb2),1,tbloqfil,0,i,mydata.comcolumnas); 
      }
    }
    if(bb2<mat->mynblock2) {
	  if (b1<mat->mynblock1) MPI_Type_free(&tbloqfil); 
      MPI_Type_free(&tbloqfilrcv); 
    }
    p1=(p1==0)?(mydata.numproccol-1):(p1-1);
    p2=(p2==0)?(mydata.numprocrow-1):(p2-1);
    if (p1==mydata.numproccol-1) b1--;
    if (p2==mydata.numprocrow-1) b2--;
  }
  free(temp); 
}

// Main
int main(int argc, char** argv) {

  int n,bs1,nblocks,mblocks;
  MATRIX a,b;

  initialize(&argc,&argv);

  // @arturo 2015: Check parameters
  if ( argc < 3 ) {
	  fprintf(stderr, "Usage: %s <matrixSize> <blockSize>\n", argv[0]);
	  MPI_Finalize();
	  exit( EXIT_FAILURE );
  }

  n=atoi(argv[1]);
  bs1=atoi(argv[2]);

  initializeMat(&a,n,n,bs1,bs1,&mblocks,&nblocks);
  initializeVec(&b,n,bs1,NULL);
  randomMat(&a,0xCAFE,-50.0,50.0);
  randomMat(&b,0xBECA,-50.0,50.0);
	
#ifdef WRITE_DATA
  print(&a,"A.dtxt");
  printVec(&b,"B.dtxt");
#endif

  initializeClock();

  factorizeMat(&a,mblocks,nblocks);
  solveMat(&a,&b,min(nblocks,mblocks));

  stopClock();

// @arturo 2015: All output should be before or after the measured
#ifdef WRITE_DATA
  print(&a,"LU.dtxt");
#endif
#ifdef WRITE_DATA
  printVec(&b,"X.dtxt");
#endif
  freeMat(&a);
  freeMat(&b);
  freeAll();

  printClock();

  MPI_Finalize();
  return 0;
}
