/**
**
** Routines to read/write matrix.
**
** ejim  Wed Mar  4 15:16:14 PST 1998
**
**/

#include "my_util.h"


void *my_malloc (int sz)
{
  void *ptr = malloc(sz);

  if (!ptr){
    printf ("Error: can't allocate %d bytes\n", sz);
    exit (0);
  }
#ifdef DEBUG
  printf("allocating %d bytes to %p\n", sz, ptr);
#endif

  return ptr;
}

/* generates random integer in [low, high) */
int random_integer (int low, int high)
{
  int r;
  r = ((high-low)*drand48()) + low;
  if (r==high) r--;

  return r;
}

/* generates random double in [low, high) */
double random_double (double low, double high)
{
    return  ((high-low)*drand48()) + low;
}
/*
MT-LEVEL
     Safe

DESCRIPTION
     This family of  functions  generates  pseudo-random  numbers
     using  the  well-known linear congruential algorithm and 48-
     bit integer arithmetic.

     Functions  drand48()  and  erand48()   return   non-negative
     double-precision floating-point values uniformly distributed
     over the interval [0.0, 1.0).
*/


int str_to_mem_unit(char *str){
  int v = 0;

  while (*str >= '0' && *str <='9'){
    v = v*10 + *str - '0'; str++;
  }

  if (*str == 'm' || *str == 'M'){
    v *= 1024*1024;
  }
  else if (*str == 'k' || *str == 'K'){
    v *= 1024;
  }

  return v;
}


#include "mmio.h"


int mm_is_valid(MM_typecode matcode)
{
    if (!mm_is_matrix(matcode)) return 0;
    if (mm_is_dense(matcode) && mm_is_pattern(matcode)) return 0;
    if (mm_is_real(matcode) && mm_is_hermitian(matcode)) return 0;
    if (mm_is_pattern(matcode) && (mm_is_hermitian(matcode) ||
                mm_is_skew(matcode))) return 0;
    return 1;
}

int mm_read_banner(FILE *f, MM_typecode *matcode)
{
    char line[MM_MAX_LINE_LENGTH];
    char banner[MM_MAX_TOKEN_LENGTH];
    char mtx[MM_MAX_TOKEN_LENGTH];
    char crd[MM_MAX_TOKEN_LENGTH];
    char data_type[MM_MAX_TOKEN_LENGTH];
    char storage_scheme[MM_MAX_TOKEN_LENGTH];
    char *p;


    mm_clear_typecode(matcode);

    if (fgets(line, MM_MAX_LINE_LENGTH, f) == NULL)
        return MM_PREMATURE_EOF;

    if (sscanf(line, "%s %s %s %s %s", banner, mtx, crd, data_type,
        storage_scheme) != 5)
        return MM_PREMATURE_EOF;

    for (p=mtx; *p!='\0'; *p=tolower(*p),p++);  /* convert to lower case */
    for (p=crd; *p!='\0'; *p=tolower(*p),p++);
    for (p=data_type; *p!='\0'; *p=tolower(*p),p++);
    for (p=storage_scheme; *p!='\0'; *p=tolower(*p),p++);

    /* check for banner */
    if (strncmp(banner, MatrixMarketBanner, strlen(MatrixMarketBanner)) != 0)
        return MM_NO_HEADER;

    /* first field should be "mtx" */
    if (strcmp(mtx, MM_MTX_STR) != 0)
        return  MM_UNSUPPORTED_TYPE;
    mm_set_matrix(matcode);


    /* second field describes whether this is a sparse matrix (in coordinate
            storgae) or a dense array */


    if (strcmp(crd, MM_SPARSEROW_STR) == 0)
        mm_set_sparserow(matcode);
    else
    if (strcmp(crd, MM_COORDINATE_STR) == 0)
        mm_set_coordinate(matcode);
    else
    if (strcmp(crd, MM_DENSE_STR) == 0)
            mm_set_dense(matcode);
    else
        return MM_UNSUPPORTED_TYPE;


    /* third field */

    if (strcmp(data_type, MM_REAL_STR) == 0)
        mm_set_real(matcode);
    else
    if (strcmp(data_type, MM_COMPLEX_STR) == 0)
        mm_set_complex(matcode);
    else
    if (strcmp(data_type, MM_PATTERN_STR) == 0)
        mm_set_pattern(matcode);
    else
    if (strcmp(data_type, MM_INT_STR) == 0)
        mm_set_integer(matcode);
    else
        return MM_UNSUPPORTED_TYPE;


    /* fourth field */

    if (strcmp(storage_scheme, MM_GENERAL_STR) == 0)
        mm_set_general(matcode);
    else
    if (strcmp(storage_scheme, MM_SYMM_STR) == 0)
        mm_set_symmetric(matcode);
    else
    if (strcmp(storage_scheme, MM_HERM_STR) == 0)
        mm_set_hermitian(matcode);
    else
    if (strcmp(storage_scheme, MM_SKEW_STR) == 0)
        mm_set_skew(matcode);
    else
        return MM_UNSUPPORTED_TYPE;


    return 0;
}

int mm_write_mtx_crd_size(FILE *f, int M, int N, int nz)
{
    if (fprintf(f, "%d %d %d\n", M, N, nz) != 3)
        return MM_COULD_NOT_WRITE_FILE;
    else
        return 0;
}

int mm_read_mtx_crd_size(FILE *f, int *M, int *N, int *nz )
{
    char line[MM_MAX_LINE_LENGTH];
    int num_items_read;

    /* set return null parameter values, in case we exit with errors */
    *M = *N = *nz = 0;

    /* now continue scanning until you reach the end-of-comments */
    do
    {
        if (fgets(line,MM_MAX_LINE_LENGTH,f) == NULL)
            return MM_PREMATURE_EOF;
    }while (line[0] == '%');

    /* line[] is either blank or has M,N, nz */
    if (sscanf(line, "%d %d %d", M, N, nz) == 3)
        return 0;

    else
    do
    {
        num_items_read = fscanf(f, "%d %d %d", M, N, nz);
        if (num_items_read == EOF) return MM_PREMATURE_EOF;
    }
    while (num_items_read != 3);

    return 0;
}


int mm_read_mtx_array_size(FILE *f, int *M, int *N)
{
    char line[MM_MAX_LINE_LENGTH];
    int num_items_read;
    int nz;

    /* set return null parameter values, in case we exit with errors */
    *M = *N = 0;

    /* now continue scanning until you reach the end-of-comments */
    do
    {
        if (fgets(line,MM_MAX_LINE_LENGTH,f) == NULL)
            return MM_PREMATURE_EOF;
    }while (line[0] == '%');

    /* line[] is either blank or has M,N, nz */
    if (sscanf(line, "%d %d %*d", M, N) == 2)
        return 0;

    else /* we have a blank line */
    do
    {
        num_items_read = fscanf(f, "%d %d %*d", M, N);
        if (num_items_read == EOF) return MM_PREMATURE_EOF;
    }
    while (num_items_read != 2);

    return 0;
}

int mm_write_mtx_array_size(FILE *f, int M, int N)
{
    if (fprintf(f, "%d %d\n", M, N) != 2)
        return MM_COULD_NOT_WRITE_FILE;
    else
        return 0;
}



/*-------------------------------------------------------------------------*/

/******************************************************************/
/* use when I[], J[], and val[]J, and val[] are already allocated */
/******************************************************************/

int mm_read_mtx_crd_data(FILE *f, int M, int N, int nz, int I[], int J[],
        double val[], MM_typecode matcode)
{
    int i;
    if (mm_is_complex(matcode))
    {
        for (i=0; i<nz; i++)
            if (fscanf(f, "%d %d %lg %lg", &I[i], &J[i], &val[2*i], &val[2*i+1])
                != 4) return MM_PREMATURE_EOF;
    }
    else if (mm_is_real(matcode))
    {
        for (i=0; i<nz; i++)
        {
            if (fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i])
                != 3) return MM_PREMATURE_EOF;

        }
    }

    else if (mm_is_pattern(matcode))
    {
        for (i=0; i<nz; i++)
            if (fscanf(f, "%d %d", &I[i], &J[i])
                != 2) return MM_PREMATURE_EOF;
    }
    else
        return MM_UNSUPPORTED_TYPE;

    return 0;

}

int mm_read_mtx_crd_entry(FILE *f, int *I, int *J,
        double *real, double *imag, MM_typecode matcode)
{
    if (mm_is_complex(matcode))
    {
            if (fscanf(f, "%d %d %lg %lg", I, J, real, imag)
                != 4) return MM_PREMATURE_EOF;
    }
    else if (mm_is_real(matcode))
    {
            if (fscanf(f, "%d %d %lg\n", I, J, real)
                != 3) return MM_PREMATURE_EOF;

    }

    else if (mm_is_pattern(matcode))
    {
            if (fscanf(f, "%d %d", I, J) != 2) return MM_PREMATURE_EOF;
    }
    else
        return MM_UNSUPPORTED_TYPE;

    return 0;

}


/************************************************************************
    mm_read_mtx_crd()  fills M, N, nz, array of values, and return
                        type code, e.g. 'MCRS'

                        if matrix is complex, values[] is of size 2*nz,
                            (nz pairs of real/imaginary values)
************************************************************************/

int mm_read_mtx_crd(char *fname, int *M, int *N, int *nz, int **I, int **J,
        double **val, MM_typecode *matcode)
{
    int ret_code;
    FILE *f;

    if (strcmp(fname, "stdin") == 0) f=stdin;
    else
    if ((f = fopen(fname, "r")) == NULL)
        return MM_COULD_NOT_READ_FILE;


    if ((ret_code = mm_read_banner(f, matcode)) != 0)
        return ret_code;

    if (!(mm_is_valid(*matcode) && mm_is_sparse(*matcode) &&
            mm_is_matrix(*matcode)))
        return MM_UNSUPPORTED_TYPE;

    if ((ret_code = mm_read_mtx_crd_size(f, M, N, nz)) != 0)
        return ret_code;


    *I = (int *)  malloc(*nz * sizeof(int));
    *J = (int *)  malloc(*nz * sizeof(int));
    *val = NULL;

    if (mm_is_complex(*matcode))
    {
        *val = (double *) malloc(*nz * 2 * sizeof(double));
        ret_code = mm_read_mtx_crd_data(f, *M, *N, *nz, *I, *J, *val,
                *matcode);
        if (ret_code != 0) return ret_code;
    }
    else if (mm_is_real(*matcode))
    {
        *val = (double *) malloc(*nz * sizeof(double));
        ret_code = mm_read_mtx_crd_data(f, *M, *N, *nz, *I, *J, *val,
                *matcode);
        if (ret_code != 0) return ret_code;
    }

    else if (mm_is_pattern(*matcode))
    {
        ret_code = mm_read_mtx_crd_data(f, *M, *N, *nz, *I, *J, *val,
                *matcode);
        if (ret_code != 0) return ret_code;
    }

    if (f != stdin) fclose(f);
    return 0;
}

int mm_write_banner(FILE *f, MM_typecode matcode)
{
    char *str = mm_typecode_to_str(matcode);
    int ret_code;

    ret_code = fprintf(f, "%s %s\n", MatrixMarketBanner, str);
    free(str);
    if (ret_code !=2 )
        return MM_COULD_NOT_WRITE_FILE;
    else
        return 0;
}

int mm_write_mtx_crd(char fname[], int M, int N, int nz, int I[], int J[],
        double val[], MM_typecode matcode)
{
    FILE *f;
    int i;

    if (strcmp(fname, "stdout") == 0)
        f = stdout;
    else
    if ((f = fopen(fname, "w")) == NULL)
        return MM_COULD_NOT_WRITE_FILE;

    /* print banner followed by typecode */
    fprintf(f, "%s ", MatrixMarketBanner);
    fprintf(f, "%s\n", mm_typecode_to_str(matcode));

    /* print matrix sizes and nonzeros */
    fprintf(f, "%d %d %d\n", M, N, nz);

    /* print values */
    if (mm_is_pattern(matcode))
        for (i=0; i<nz; i++)
            fprintf(f, "%d %d\n", I[i], J[i]);
    else
    if (mm_is_real(matcode))
        for (i=0; i<nz; i++)
            fprintf(f, "%d %d %20.16g\n", I[i], J[i], val[i]);
    else
    if (mm_is_complex(matcode))
        for (i=0; i<nz; i++)
            fprintf(f, "%d %d %20.16g %20.16g\n", I[i], J[i], val[2*i],
                        val[2*i+1]);
    else
    {
        if (f != stdout) fclose(f);
        return MM_UNSUPPORTED_TYPE;
    }

    if (f !=stdout) fclose(f);

    return 0;
}


char  *mm_typecode_to_str(MM_typecode matcode)
{
    char buffer[MM_MAX_LINE_LENGTH];
    char *types[4];
    int error =0;

    /* check for MTX type */
    if (mm_is_matrix(matcode))
        types[0] = MM_MTX_STR;
    else
        error=1;

    /* check for CRD or ARR matrix */
    if (mm_is_sparserow(matcode))
        types[1] = MM_SPARSEROW_STR;
    else
    if (mm_is_coordinate(matcode))
        types[1] = MM_COORDINATE_STR;
    else
    if (mm_is_dense(matcode))
        types[1] = MM_DENSE_STR;
    else
        return NULL;

    /* check for element data type */
    if (mm_is_real(matcode))
        types[2] = MM_REAL_STR;
    else
    if (mm_is_complex(matcode))
        types[2] = MM_COMPLEX_STR;
    else
    if (mm_is_pattern(matcode))
        types[2] = MM_PATTERN_STR;
    else
    if (mm_is_integer(matcode))
        types[2] = MM_INT_STR;
    else
        return NULL;


    /* check for symmetry type */
    if (mm_is_general(matcode))
        types[3] = MM_GENERAL_STR;
    else
    if (mm_is_symmetric(matcode))
        types[3] = MM_SYMM_STR;
    else
    if (mm_is_hermitian(matcode))
        types[3] = MM_HERM_STR;
    else
    if (mm_is_skew(matcode))
        types[3] = MM_SKEW_STR;
    else
        return NULL;

    sprintf(buffer,"%s %s %s %s", types[0], types[1], types[2], types[3]);
    return strdup(buffer);

}


#include "matrix_io.h"

void csr2csc(int n, int m, int nz, double *a, int *col_idx, int *row_start,
             double *csc_a, int *row_idx, int *col_start);
void coo2csr_in(int n, int nz, double *a, int *i_idx, int *j_idx);
void coo2csr(int n, int nz, double *a, int *i_idx, int *j_idx,
	     double *csr_a, int *col_idx, int *row_start);


/* write CSR format */
/* 1st line : % number_of_rows number_of_columns number_of_nonzeros
   2nd line : % base of index
   3rd line : row_number  nz_r(=number_of_nonzeros_in_the_row)
   next nz_r lines : column_index value(when a != NULL)
   next line : row_number  nz_r(=number_of_nonzeros_in_the_row)
   next nz_r lines : column_index value(when a != NULL)
   ...
   */

void write_csr (char *fn, int m, int n, int nz,
		int *row_start, int *col_idx, double *a)
{
  FILE *f;
  int i, j;

  if ((f = fopen(fn, "w")) == NULL){
    printf ("can't open file <%s> \n", fn);
    exit(1);
  }

  fprintf (f, "%s %d %d %d\n", "%", m, n, nz);
  fprintf (f, "%s 0\n", "%");

  for (i=0; i<m; i++){
    fprintf(f, "%d %d\n", i, row_start[i+1]-row_start[i]);

    for (j=row_start[i]; j<row_start[i+1]; j++){
      if (a)
	fprintf(f, "%d %20.19g\n", col_idx[j], a[j]);
      else
	fprintf(f, "%d\n", col_idx[j]);
    }
  }

  fclose (f);

}


/* reads matrix market format (coordinate) and returns
   csr format */

void read_mm_matrix (char *fn, int *m, int *n, int *nz,
		  int **i_idx, int **j_idx, double **a)
{
  MM_typecode matcode;
  FILE *f;
  int i,k;
  int base=1, base2;

  if ((f = fopen(fn, "r")) == NULL) {
    printf ("can't open file <%s> \n", fn);
    exit(1);
  }
  if (mm_read_banner(f, &matcode) != 0){
    printf("Could not process Matrix Market banner.\n");
    exit(1);
  }

  /*  This is how one can screen matrix types if their application */
  /*  only supports a subset of the Matrix Market data types.      */

  if (! (mm_is_matrix(matcode) && mm_is_sparse(matcode)) ){
    printf("Sorry, this application does not support ");
    printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
    exit(1);
  }

  /* find out size of sparse matrix .... */

  fscanf(f, "%*s %d %d %d", m, n, nz);

//  fscanf(f, "%d %d",&base2, &base);     /* indentifies starting index */

  /* reserve memory for matrices */
 /* if (mm_is_symmetric(matcode)){
    *i_idx = (int *) my_malloc(*nz *2 * sizeof(int));
    *j_idx = (int *) my_malloc(*nz *2 * sizeof(int));
    *a = (double *) my_malloc(*nz *2 * sizeof(double));
  }
  else {*/
    *i_idx = (int *) my_malloc(*nz * sizeof(int));
    *j_idx = (int *) my_malloc(*nz * sizeof(int));
    *a = (double *) my_malloc(*nz * sizeof(double));

  for (i=0; i<*nz; i++)  {
	(*i_idx)[i]=-1;
	(*j_idx)[i]=-1;
	(*a)[i] =-1;
  }
 // }

  if (!(*i_idx) || !(*j_idx) || !(*a)){
    printf ("cannot allocate memory for %d, %d, %d sparse matrix\n", *m, *n, *nz);
    exit(1);
  }

//printf("\n n:%d m: %d nz: %d base:%d\n", *m,*n,*nz, base);
  k=0;

//(*i_idx)[0]=base2-base;
//(*j_idx)[0]=base-base;
  for (i=0; i<*nz; i++)  {
    //if (mm_is_pattern(matcode)){
      fscanf(f, "%d %d", &(*i_idx)[i], &(*j_idx)[i]);
      (*i_idx)[i] -= base;  /* adjust from 1-based to 0-based */
      (*j_idx)[i] -= base;
	//printf("\n %d %d", (*i_idx)[i],(*j_idx)[i]);
      (*a)[i] = random_double(-1, 1);
   // }
  /*  else if (mm_is_real(matcode)){
      fscanf(f, "%d %d %lg", &(*i_idx)[i], &(*j_idx)[i], &(*a)[i]);
      (*i_idx)[i] -= base;  // adjust from 1-based to 0-based
      (*j_idx)[i] -= base;
    }*/

   /* if (mm_is_symmetric(matcode)){
      if ( (*i_idx)[i] != (*j_idx)[i] ){
	(*i_idx)[*nz+k] = (*j_idx)[i];
	(*j_idx)[*nz+k] = (*i_idx)[i];
	(*a)[*nz+k] = (*a)[i];
	k++;
      }
    }*/
  }
  *nz += k;

  fclose(f);
  for (i=0; i<*nz; i++)  {
	//printf("\n %d %d ",(*i_idx)[i], (*j_idx)[i] );
	}
  coo2csr_in (*m, *nz, *a, *i_idx, *j_idx);
}

/* reads Harwell-Boeing format matrix and returns CSR format */

#define LINE_LEN     90
#define INPUT_WIDTH  80

void read_hb_matrix (char *fn, int *m, int *n, int *nz,
		     int **row_start, int **col_idx, double **a)
{
  MM_typecode matcode;
  FILE *f;
  int i, field, j, k;
  int row, rhs;
  char buffer[LINE_LEN];
  char mat_format[4];
  int *i_idx, *j_idx;
  int *col_start, *row_idx;
  double *coo_a;
  char    start_format[INPUT_WIDTH];
  char    idx_format[INPUT_WIDTH];
  int start_input_repeat, start_input_width;
  int idx_input_repeat, idx_input_width;
  int start_lines, idx_lines, l;

  if ((f = fopen(fn, "r")) == NULL) {
    printf ("can't open file <%s> \n", fn);
    exit(1);
  }
  fgets (buffer, LINE_LEN, f); /* title line */
  fgets (buffer, LINE_LEN, f); /* line counts */
  sscanf(buffer, "%*d %d %d %*d %d", &start_lines, &idx_lines, &rhs);
  fgets (buffer, LINE_LEN, f); /* elements counts */
  sscanf(buffer, "%s %d %d %d", mat_format, m, n, nz);
  printf("type=%s m=%d n=%d nz=%d rhs=%d\n", mat_format, *m, *n, *nz, rhs);
  fgets (buffer, LINE_LEN, f); /* formats */
  sscanf(buffer, "%s %s", start_format, idx_format);
  sscanf(start_format, "(%d%*c%d)", &start_input_repeat, &start_input_width);
  sscanf(idx_format, "(%d%*c%d)", &idx_input_repeat, &idx_input_width);
  printf("%d start_input lines %d repeats %d: %d col_input lines %d repeats %d\n",
	 start_lines, start_input_width, start_input_repeat,
	 idx_lines, idx_input_width, idx_input_repeat);

  if (rhs)
    fgets (buffer, LINE_LEN, f); /* rhs header */

  mm_clear_typecode(&matcode);
  mm_set_matrix(&matcode);
  mm_set_sparserow(&matcode);

  switch (mat_format[0]){
  case 'r':
  case 'R':
    mm_set_real(&matcode);
    break;
  case 'c':
  case 'C':
    mm_set_complex(&matcode);
    break;
  case 'p':
  case 'P':
    mm_set_pattern(&matcode);
    break;
  default:
    printf ("<%s> is not Harwell Boeing matrix format %s\n", fn, mat_format);
    exit (0);
  }

  switch (mat_format[1]){
  case 'u':
  case 'U':
  case 'r':
  case 'R':
    mm_set_general(&matcode);
    break;
  case 's':
  case 'S':
    mm_set_symmetric(&matcode);
    break;
  case 'h':
  case 'H':
    mm_set_hermitian(&matcode);
    break;
  case 'z':
  case 'Z':
    mm_set_skew(&matcode);
    break;
  default:
    printf ("<%s> is not Harwell Boeing matrix format %s\n", fn, mat_format);
    exit (0);
  }

  switch (mat_format[2]){
  case 'a':
  case 'A':
    break;
  case 'u':
  case 'U':
    printf ("<%s> is not unassembled Harwell Boeing matrix format %s\n",
	    fn, mat_format);
    exit (0);
  default:
    printf ("<%s> is not Harwell Boeing matrix format %s\n", fn, mat_format);
    exit (0);
  }


  *row_start = (int *) my_malloc((*m+1) * sizeof(int));

  /* reserve memory for matrices */
  if (mm_is_symmetric(matcode)){
    i_idx = (int *) my_malloc(*nz *2 * sizeof(int));
    j_idx = (int *) my_malloc(*nz *2 * sizeof(int));
    coo_a = (double *) my_malloc(*nz *2 * sizeof(double));
  }
  else {
    col_start = (int *) my_malloc((*n+1) * sizeof(int));
    row_idx = (int *) my_malloc(*nz * sizeof(int));

    *col_idx = (int *) my_malloc(*nz * sizeof(int));
    *a = (double *) my_malloc(*nz * sizeof(double));
  }


  if (mm_is_symmetric(matcode)){
    for (i=l=0; l<start_lines; l++){
      fgets (buffer, LINE_LEN, f);
      for (field=0; (field<start_input_repeat) && (i<=*m); field++){
	for(j=0; j<start_input_width; j++)
	  start_format[j] = buffer[field*start_input_width+j];
	start_format[j] =  0;

	(*row_start)[i++] = atoi(start_format)-1;
      }
    }

    for (i=l=k=row=0; l<idx_lines; l++){
      fgets (buffer, LINE_LEN, f);
      for (field=0; (field<idx_input_repeat) && (i<*nz); field++, i++){
	for(j=0; j<idx_input_width; j++)
	  idx_format[j] = buffer[field*idx_input_width+j];
	idx_format[j] =  0;

	if (i==(*row_start)[row+1]) row++;
	i_idx[i] = row;
	j_idx[i] = atoi(idx_format)-1;

	coo_a[i] = random_double(-1, 1);

	if ( i_idx[i] != j_idx[i] ){
	  i_idx[*nz+k] = j_idx[i];
	  j_idx[*nz+k] = i_idx[i];
	  coo_a[*nz+k] = coo_a[i];
	  k++;
	}
      }
    }
    *nz += k;

    *col_idx = (int *) my_malloc(*nz * sizeof(int));
    *a = (double *) my_malloc(*nz * sizeof(double));
    coo2csr (*m, *nz, coo_a, i_idx, j_idx, *a, *col_idx, *row_start);

    free (i_idx);
    free (j_idx);
    free (coo_a);

  }
  else {
    for (i=l=0; l<start_lines; l++){
      fgets (buffer, LINE_LEN, f);
      for (field=0; (field<start_input_repeat) && (i<=*n); field++){
	for(j=0; j<start_input_width; j++)
	  start_format[j] = buffer[field*start_input_width+j];
	start_format[j] =  0;

	col_start[i++] = atoi(start_format)-1;
      }
    }


    for (i=l=0; l<idx_lines; l++){
      fgets (buffer, LINE_LEN, f);
      for (field=0; (field<idx_input_repeat) && (i<*nz); field++, i++){
	for(j=0; j<idx_input_width; j++)
	  idx_format[j] = buffer[field*idx_input_width+j];
	idx_format[j] =  0;

	row_idx[i] = atoi(idx_format)-1;

	(*a)[i] = random_double(-1, 1);
      }
    }

    csr2csc(*n, *m, *nz, NULL, row_idx, col_start, NULL, *col_idx, *row_start);


    free (row_idx);
    free (col_start);
  }

  fclose(f);
}


void sort(int *col_idx, double *a, int start, int end)
{
  int i, j, it;
  double dt;

  for (i=end-1; i>start; i--)
    for(j=start; j<i; j++)
      if (col_idx[j] > col_idx[j+1]){

	if (a){
	  dt=a[j];
	  a[j]=a[j+1];
	  a[j+1]=dt;
        }
	it=col_idx[j];
	col_idx[j]=col_idx[j+1];
	col_idx[j+1]=it;

      }
}



/* converts COO format to CSR format, in-place,
   if SORT_IN_ROW is defined, each row is sorted in column index.
   On return, i_idx contains row_start position */

void coo2csr_in(int n, int nz, double *a, int *i_idx, int *j_idx)
{
  int *row_start;
  int i, j;
  int init, i_next, j_next, i_pos;
  double dt, a_next;

  row_start = (int *)malloc((n+1)*sizeof(int));
  if (!row_start){
    printf ("coo2csr_in: cannot allocate temporary memory\n");
    exit (1);
  }
  for (i=0; i<=n; i++) row_start[i] = 0;

#ifdef DEBUG
 printf("\n idx \n" );
 for (i=0; i<nz; i++){
	printf("\n %d", i_idx[i]);
 }

#endif
  /* determine row lengths */
  for (i=0; i<nz; i++) {
	if(i_idx[i]!=-1){
	row_start[i_idx[i]+1]++;
	}
  }


#ifdef DEBUG
 printf("\n row Start \n" );
 for (i=0; i<n; i++){
	printf("\n %d", row_start[i]);
 }
#endif

  for (i=0; i<n; i++) row_start[i+1] += row_start[i];


#ifdef DEBUG
 printf("\n row Start \n" );
 for (i=0; i<n; i++){

printf("\n %d", row_start[i]);
}
#endif


  for (init=0; init<nz; ){
    dt = a[init];
    i = i_idx[init];
    j = j_idx[init];
    i_idx[init] = -1;

    while (1){
      i_pos = row_start[i];
      a_next = a[i_pos];
      i_next = i_idx[i_pos];
      j_next = j_idx[i_pos];

      a[i_pos] = dt;
      j_idx[i_pos] = j;
      i_idx[i_pos] = -1;
      row_start[i]++;
      if (i_next < 0) break;
      dt = a_next;
      i = i_next;
      j = j_next;

    }
    init++;
    while ((i_idx[init] < 0) && (init < nz))  init++;
  }


  /* shift back row_start */
  for (i=0; i<n; i++) i_idx[i+1] = row_start[i];
  i_idx[0] = 0;

#ifdef DEBUG
 printf("\n row Start \n" );
 for (i=0; i<n; i++){

printf("\n %d", i_idx[i]);
}
#endif
  for (i=0; i<n; i++){
    sort (j_idx, a, i_idx[i], i_idx[i+1]);
  }


}

/* converts COO format to CSR format, not in-place,
   if SORT_IN_ROW is defined, each row is sorted in column index */


void coo2csr(int n, int nz, double *a, int *i_idx, int *j_idx,
	     double *csr_a, int *col_idx, int *row_start)
{
  int i, l;

  for (i=0; i<=n; i++) row_start[i] = 0;

  /* determine row lengths */
  for (i=0; i<nz; i++) row_start[i_idx[i]+1]++;


  for (i=0; i<n; i++) row_start[i+1] += row_start[i];


  /* go through the structure  once more. Fill in output matrix. */
  for (l=0; l<nz; l++){
    i = row_start[i_idx[l]];
    csr_a[i] = a[l];
    col_idx[i] = j_idx[l];
    row_start[i_idx[l]]++;
  }

  /* shift back row_start */
  for (i=n; i>0; i--) row_start[i] = row_start[i-1];

  row_start[0] = 0;

  for (i=0; i<n; i++){
    sort (col_idx, csr_a, row_start[i], row_start[i+1]);
  }

}


/*
   converts CSR format to CSC format, not in-place,
   if a == NULL, only pattern is reorganized.
   the size of matrix is n x m.
 */

void csr2csc(int n, int m, int nz, double *a, int *col_idx, int *row_start,
             double *csc_a, int *row_idx, int *col_start)
{
  int i, j, k, l;
  int *ptr;

  for (i=0; i<=m; i++) col_start[i] = 0;

  /* determine column lengths */
  for (i=0; i<nz; i++) col_start[col_idx[i]+1]++;

  for (i=0; i<m; i++) col_start[i+1] += col_start[i];


  /* go through the structure once more. Fill in output matrix. */

  for (i=0, ptr=row_start; i<n; i++, ptr++)
    for (j=*ptr; j<*(ptr+1); j++){
      k = col_idx[j];
      l = col_start[k]++;
      row_idx[l] = i;
      if (a) csc_a[l] = a[j];
    }

  /* shift back col_start */
  for (i=m; i>0; i--) col_start[i] = col_start[i-1];

  col_start[0] = 0;
}
