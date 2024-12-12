/**
 * @file solverKernel.h
 * @brief Cpu reference solver kernel for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 *
 * @copyright This file is part of a modified version of a CUDA sample. Thus the following applies:
 * @copyright Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
 *
 * @copyright Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

///////////////////////////////////////////////////////////////////////////////
/// \brief one iteration of classical Horn-Schunck method
///
/// It is one iteration of Jacobi method for a corresponding linear system
/// \param[in]  du0     current horizontal displacement approximation
/// \param[in]  dv0     current vertical displacement approximation
/// \param[in]  Ix      image x derivative
/// \param[in]  Iy      image y derivative
/// \param[in]  Iz      temporal derivative
/// \param[in]  w       width
/// \param[in]  h       height
/// \param[in]  s       stride
/// \param[in]  alpha   degree of smoothness
/// \param[out] du1     new horizontal displacement approximation
/// \param[out] dv1     new vertical displacement approximation
///////////////////////////////////////////////////////////////////////////////
static void SolveForUpdate(const float *du0, const float *dv0, const float *Ix,
						   const float *Iy, const float *Iz, int w, int h,
						   int s, float alpha, float *du1, float *dv1) {
	#pragma omp parallel for
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			const int pos = j + i * s;
			int       left, right, up, down;

			// handle borders
			if (j != 0)
				left = pos - 1;
			else
				left = pos;

			if (j != w - 1)
				right = pos + 1;
			else
				right = pos;

			if (i != 0)
				down = pos - s;
			else
				down = pos;

			if (i != h - 1)
				up = pos + s;
			else
				up = pos;

			float sumU = (du0[left] + du0[right] + du0[up] + du0[down]) * 0.25f;
			float sumV = (dv0[left] + dv0[right] + dv0[up] + dv0[down]) * 0.25f;

			float frac = (Ix[pos] * sumU + Iy[pos] * sumV + Iz[pos]) /
						 (Ix[pos] * Ix[pos] + Iy[pos] * Iy[pos] + alpha);

			du1[pos] = sumU - Ix[pos] * frac;
			dv1[pos] = sumV - Iy[pos] * frac;
		}
	}
}