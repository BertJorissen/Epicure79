#include "Ctrl.h"
#include "Ctrl_Blas_Enum.h"
/**
 * @file Ctrl_Blas.c
 * @brief Collection of Blas kernels for Controllers.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

Ctrl_NewType(float);

// SCOPY
CTRL_KERNEL_CHAR(ctrl_scopy, MANUAL, 16);
CTRL_KERNEL(ctrl_scopy, CUDALIB, CUBLAS, int n, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	cublasScopy(handle, n, x.data, incx, y.data, incy);
});

CTRL_KERNEL(ctrl_scopy, HIPLIB, HIPBLAS, int n, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	hipblasScopy(handle, n, x.data, incx, y.data, incy);
});

CTRL_KERNEL(ctrl_scopy, CUDALIB, MAGMA, int n, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	magma_scopy(n, x.data, incx, y.data, incy, queue);
});

CTRL_KERNEL(ctrl_scopy, CPULIB, MKL, int n, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	cblas_scopy(n, x.data, incx, y.data, incy);
});

CTRL_KERNEL_PROTO(ctrl_scopy,
				  4,
				  CUDALIB, CUBLAS,
				  HIPLIB, HIPBLAS,
				  CUDALIB, MAGMA,
				  CPULIB, MKL,
				  5,
				  INVAL, int, n,
				  IN, HitTile_float, x,
				  INVAL, int, incx,
				  OUT, HitTile_float, y,
				  INVAL, int, incy);

// SGEMV
CTRL_KERNEL_CHAR(ctrl_sgemv, MANUAL, 16);
CTRL_KERNEL(ctrl_sgemv, CUDALIB, CUBLAS, Ctrl_Blas_Trans trans, int m, int n, float alpha, KHitTile_float a, int lda, KHitTile_float x, int incx, float beta, KHitTile_float y, int incy, {
	cublasOperation_t cublasTrans;
	switch (trans) {
		case CTRL_BLAS_NOTRANS:
			cublasTrans = CUBLAS_OP_N;
			break;
		case CTRL_BLAS_TRANS:
			cublasTrans = CUBLAS_OP_T;
			break;
		case CTRL_BLAS_CONJTRANS:
			cublasTrans = CUBLAS_OP_C;
			break;
	}
	cublasSgemv(handle, cublasTrans, m, n, &alpha, a.data, lda, x.data, incx, &beta, y.data, incy); // y = αA'x + βy (matrix A' * vector x)
});

CTRL_KERNEL(ctrl_sgemv, HIPLIB, HIPBLAS, Ctrl_Blas_Trans trans, int m, int n, float alpha, KHitTile_float a, int lda, KHitTile_float x, int incx, float beta, KHitTile_float y, int incy, {
	hipblasOperation_t hipblasTrans;
	switch (trans) {
		case CTRL_BLAS_NOTRANS:
			hipblasTrans = HIPBLAS_OP_N;
			break;
		case CTRL_BLAS_TRANS:
			hipblasTrans = HIPBLAS_OP_T;
			break;
		case CTRL_BLAS_CONJTRANS:
			hipblasTrans = HIPBLAS_OP_C;
			break;
	}
	hipblasSgemv(handle, hipblasTrans, m, n, &alpha, a.data, lda, x.data, incx, &beta, y.data, incy); // y = αA'x + βy (matrix A' * vector x)
});

CTRL_KERNEL(ctrl_sgemv, CUDALIB, MAGMA, Ctrl_Blas_Trans trans, int m, int n, float alpha, KHitTile_float a, int lda, KHitTile_float x, int incx, float beta, KHitTile_float y, int incy, {
	magma_sgemv((magma_trans_t)trans, m, n, alpha, a.data, lda, x.data, incx, beta, y.data, incy, queue); // y = αA'x + βy (matrix A' * vector x)
});

CTRL_KERNEL(ctrl_sgemv, CPULIB, MKL, Ctrl_Blas_Trans trans, int m, int n, float alpha, KHitTile_float a, int lda, KHitTile_float x, int incx, float beta, KHitTile_float y, int incy, {
	cblas_sgemv(CblasRowMajor, (CBLAS_TRANSPOSE)trans, m, n, alpha, a.data, lda, x.data, incx, beta, y.data, incy); // y = αA'x + βy (matrix A' * vector x)
});

CTRL_KERNEL_PROTO(ctrl_sgemv,
				  4,
				  CUDALIB, CUBLAS,
				  HIPLIB, HIPBLAS,
				  CUDALIB, MAGMA,
				  CPULIB, MKL,
				  11,
				  INVAL, Ctrl_Blas_Trans, trans,
				  INVAL, int, m,
				  INVAL, int, n,
				  INVAL, float, alpha,
				  IN, HitTile_float, a,
				  INVAL, int, lda,
				  IN, HitTile_float, x,
				  INVAL, int, incx,
				  INVAL, float, beta,
				  IO, HitTile_float, y,
				  INVAL, int, incy);

// SGER
CTRL_KERNEL_CHAR(ctrl_sger, MANUAL, 16);

CTRL_KERNEL(ctrl_sger, CUDALIB, CUBLAS, int m, int n, float alpha, KHitTile_float x, int incx, KHitTile_float y, int incy, KHitTile_float a, int lda, {
	cublasSger(handle, m, n, &alpha, x.data, incx, y.data, incy, a.data, lda); // A = α*x*y' + A
});

CTRL_KERNEL(ctrl_sger, HIPLIB, HIPBLAS, int m, int n, float alpha, KHitTile_float x, int incx, KHitTile_float y, int incy, KHitTile_float a, int lda, {
	hipblasSger(handle, m, n, &alpha, x.data, incx, y.data, incy, a.data, lda); // A = α*x*y' + A
});

CTRL_KERNEL(ctrl_sger, CUDALIB, MAGMA, int m, int n, float alpha, KHitTile_float x, int incx, KHitTile_float y, int incy, KHitTile_float a, int lda, {
	magma_sger(m, n, alpha, x.data, incx, y.data, incy, a.data, lda, queue); // A = α*x*y' + A
});

CTRL_KERNEL(ctrl_sger, CPULIB, MKL, int m, int n, float alpha, KHitTile_float x, int incx, KHitTile_float y, int incy, KHitTile_float a, int lda, {
	cblas_sger(CblasRowMajor, m, n, alpha, x.data, incx, y.data, incy, a.data, lda); // A = α*x*y' + A
});

CTRL_KERNEL_PROTO(ctrl_sger,
				  4,
				  CUDALIB, CUBLAS,
				  HIPLIB, HIPBLAS,
				  CUDALIB, MAGMA,
				  CPULIB, MKL,
				  9,
				  INVAL, int, m,
				  INVAL, int, n,
				  INVAL, float, alpha,
				  IN, HitTile_float, x,
				  INVAL, int, incx,
				  IN, HitTile_float, y,
				  INVAL, int, incy,
				  IO, HitTile_float, a,
				  INVAL, int, lda);

// SGEMM
CTRL_KERNEL_CHAR(ctrl_sgemm, MANUAL, 16);

CTRL_KERNEL(ctrl_sgemm, CUDALIB, CUBLAS, Ctrl_Blas_Trans transa, Ctrl_Blas_Trans transb, int m, int n, int k, float alpha, KHitTile_float a, int lda, KHitTile_float b, int ldb, float beta, KHitTile_float c, int ldc, {
	cublasOperation_t cublasTransa;
	switch (transa) {
		case CTRL_BLAS_NOTRANS:
			cublasTransa = CUBLAS_OP_N;
			break;
		case CTRL_BLAS_TRANS:
			cublasTransa = CUBLAS_OP_T;
			break;
		case CTRL_BLAS_CONJTRANS:
			cublasTransa = CUBLAS_OP_C;
			break;
	}
	cublasOperation_t cublasTransb;
	switch (transb) {
		case CTRL_BLAS_NOTRANS:
			cublasTransb = CUBLAS_OP_N;
			break;
		case CTRL_BLAS_TRANS:
			cublasTransb = CUBLAS_OP_T;
			break;
		case CTRL_BLAS_CONJTRANS:
			cublasTransb = CUBLAS_OP_C;
			break;
	}
	cublasSgemm(handle, cublasTransa, cublasTransb, m, n, k, &alpha, a.data, lda, b.data, ldb, &beta, c.data, ldc); // C = αA'*B' + βC,
});

CTRL_KERNEL(ctrl_sgemm, HIPLIB, HIPBLAS, Ctrl_Blas_Trans transa, Ctrl_Blas_Trans transb, int m, int n, int k, float alpha, KHitTile_float a, int lda, KHitTile_float b, int ldb, float beta, KHitTile_float c, int ldc, {
	hipblasOperation_t hipblasTransa;
	switch (transa) {
		case CTRL_BLAS_NOTRANS:
			hipblasTransa = hipBLAS_OP_N;
			break;
		case CTRL_BLAS_TRANS:
			hipblasTransa = hipBLAS_OP_T;
			break;
		case CTRL_BLAS_CONJTRANS:
			hipblasTransa = hipBLAS_OP_C;
			break;
	}
	hipblasOperation_t hipblasTransb;
	switch (transb) {
		case CTRL_BLAS_NOTRANS:
			hipblasTransb = HIPBLAS_OP_N;
			break;
		case CTRL_BLAS_TRANS:
			hipblasTransb = HIPBLAS_OP_T;
			break;
		case CTRL_BLAS_CONJTRANS:
			hipblasTransb = HIPBLAS_OP_C;
			break;
	}
	hipblasSgemm(handle, hipblasTransa, hipblasTransb, m, n, k, &alpha, a.data, lda, b.data, ldb, &beta, c.data, ldc); // C = αA'*B' + βC,
});

CTRL_KERNEL(ctrl_sgemm, CUDALIB, MAGMA, Ctrl_Blas_Trans transa, Ctrl_Blas_Trans transb, int m, int n, int k, float alpha, KHitTile_float a, int lda, KHitTile_float b, int ldb, float beta, KHitTile_float c, int ldc, {
	magma_sgemm((magma_trans_t)transa, (magma_trans_t)transb, m, n, k, alpha, a.data, lda, b.data, ldb, beta, c.data, ldc, queue); // C = αA'*B' + βC,
});

CTRL_KERNEL(ctrl_sgemm, CPULIB, MKL, Ctrl_Blas_Trans transa, Ctrl_Blas_Trans transb, int m, int n, int k, float alpha, KHitTile_float a, int lda, KHitTile_float b, int ldb, float beta, KHitTile_float c, int ldc, {
	cblas_sgemm(CblasRowMajor, (CBLAS_TRANSPOSE)transa, (CBLAS_TRANSPOSE)transb, m, n, k, alpha, a.data, lda, b.data, ldb, beta, c.data, ldc); // C = αA'*B' + βC,
});

CTRL_KERNEL_PROTO(ctrl_sgemm,
				  4,
				  CUDALIB, CUBLAS,
				  HIPLIB, HIPBLAS,
				  CUDALIB, MAGMA,
				  CPULIB, MKL,
				  13,
				  INVAL, Ctrl_Blas_Trans, transa,
				  INVAL, Ctrl_Blas_Trans, transb,
				  INVAL, int, m,
				  INVAL, int, n,
				  INVAL, int, k,
				  INVAL, float, alpha,
				  IN, HitTile_float, a,
				  INVAL, int, lda,
				  IN, HitTile_float, b,
				  INVAL, int, ldb,
				  INVAL, float, beta,
				  IO, HitTile_float, c,
				  INVAL, int, ldc);

// SAXPY
CTRL_KERNEL_CHAR(ctrl_saxpy, MANUAL, 16);

CTRL_KERNEL(ctrl_saxpy, CUDALIB, CUBLAS, int n, float a, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	cublasSaxpy(handle, n, &a, x.data, incx, y.data, incy);
});

CTRL_KERNEL(ctrl_saxpy, HIPLIB, HIPBLAS, int n, float a, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	hipblasSaxpy(handle, n, &a, x.data, incx, y.data, incy);
});

CTRL_KERNEL(ctrl_saxpy, CUDALIB, MAGMA, int n, float a, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	magma_saxpy(n, a, x.data, incx, y.data, incy, queue);
});

CTRL_KERNEL(ctrl_saxpy, CPULIB, MKL, int n, float a, KHitTile_float x, int incx, KHitTile_float y, int incy, {
	cblas_saxpy(n, a, x.data, incx, y.data, incy);
});

CTRL_KERNEL_PROTO(ctrl_saxpy,
				  4,
				  CUDALIB, CUBLAS,
				  HIPLIB, HIPBLAS,
				  CUDALIB, MAGMA,
				  CPULIB, MKL,
				  6,
				  INVAL, int, n,
				  INVAL, float, a,
				  IN, HitTile_float, x,
				  INVAL, int, incx,
				  IO, HitTile_float, y,
				  INVAL, int, incy);

// SSBMV
CTRL_KERNEL_CHAR(ctrl_ssbmv, MANUAL, 16);

CTRL_KERNEL(ctrl_ssbmv, CUDALIB, CUBLAS, int uplo, int n, int k, float alpha, KHitTile_float a, int lda, KHitTile_float x, int incx, float beta, KHitTile_float y, int incy, {
	cublasSsbmv(handle, (cublasFillMode_t)uplo, n, k, &alpha, a.data, lda, x.data, incx, &beta, y.data, incy); // y := αAx + βy
});

CTRL_KERNEL(ctrl_ssbmv, HIPLIB, HIPBLAS, int uplo, int n, int k, float alpha, KHitTile_float a, int lda, KHitTile_float x, int incx, float beta, KHitTile_float y, int incy, {
	hipblasSsbmv(handle, (hipblasFillMode_t)uplo, n, k, &alpha, a.data, lda, x.data, incx, &beta, y.data, incy); // y := αAx + βy
});

CTRL_KERNEL(ctrl_ssbmv, CPULIB, MKL, int uplo, int n, int k, float alpha, KHitTile_float a, int lda, KHitTile_float x, int incx, float beta, KHitTile_float y, int incy, {
	cblas_ssbmv(CblasRowMajor, (CBLAS_UPLO)uplo, n, k, alpha, a.data, lda, x.data, incx, beta, y.data, incy); // y := αAx + βy
});

CTRL_KERNEL_PROTO(ctrl_ssbmv,
				  3,
				  CUDALIB, CUBLAS,
				  HIPLIB, HIPBLAS,
				  CPULIB, MKL,
				  11,
				  INVAL, int, uplo,
				  INVAL, int, n,
				  INVAL, int, k,
				  INVAL, float, alpha,
				  IN, HitTile_float, a,
				  INVAL, int, lda, IN, HitTile_float, x,
				  INVAL, int, incx,
				  INVAL, float, beta,
				  IO, HitTile_float, y,
				  INVAL, int, incy);
