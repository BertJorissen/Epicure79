#ifndef _CTRL_BLAS_ENUM_H_
#define _CTRL_BLAS_ENUM_H_

/**
 * Numbering is consistent with CBLAS. For cublas translation is handled internally.
 */
typedef enum {
	CTRL_BLAS_NOTRANS   = 111,
	CTRL_BLAS_TRANS     = 112,
	CTRL_BLAS_CONJTRANS = 113
} Ctrl_Blas_Trans;

#endif // _CTRL_BLAS_ENUM_H_