
/***********************/
/* function prototypes */
/***********************/
void full_verify( void );

double	randlc( double *X, double *A );

void c_print_results( const char   *name,
						const char   class,
						int    n1, 
						int    n2,
						int    n3,
						int    niter,
						int    nprocs_compiled,
						int    nprocs_total,
						double t,
						double mops,
						const char   *optype,
						int    passed_verification,
						const char   *npbversion,
						const char   *compiletime,
						const char   *mpicc,
						const char   *clink,
						const char   *cmpi_lib,
						const char   *cmpi_inc,
						const char   *cflags,
						const char   *clinkflags );


