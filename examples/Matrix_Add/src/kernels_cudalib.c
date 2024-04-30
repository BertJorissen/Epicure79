/**
 * @file kernels_cudalib.c
 * @author Trasgo Group
 * @brief MatrixAdd: Using CUDALIB interface for using third-party BLAS library: cuBLAS
 * @version 5.0
 * @date 2023-07-04
 *
 * @copyright This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @copyright Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * @copyright More information on http://trasgo.infor.uva.es/
 */

#include "Ctrl.h"
#include "kernels_params.h"

/* MAGMA */
CTRL_KERNEL(Add, CUDALIB, MAGMA, CTRL_KPARAMS(madd_params), {
	for (int k = 0; k < n_iter; k++) {
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, A.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
		magmablas_sgeadd(hit_tileDimCard(A, 0), hit_tileDimCard(A, 1), 1, B.data, hit_tileDimCard(A, 0), C.data, hit_tileDimCard(C, 0), queue);
	}
});

/* cuBLAS */
CTRL_KERNEL(Add, CUDALIB, CUBLAS, CTRL_KPARAMS(madd_params), {
	const float alpha = 1.0f;
	for (int k = 0; k < n_iter; k++) {
		cublasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, A.data, 1, C.data, 1);
		cublasSaxpy(handle, hit_tileDimCard(A, 0) * hit_tileDimCard(A, 1), &alpha, B.data, 1, C.data, 1);
	}
});
