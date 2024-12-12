/**
 * @file derivativesKernel.h
 * @brief Cpu reference derivatives kernel for HSOpticalflow.
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

#include "cpu_tex.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief computes image derivatives for a pair of images
/// \param[in]  I0  source image
/// \param[in]  I1  tracked image
/// \param[in]  w   images width
/// \param[in]  h   images height
/// \param[in]  s   images stride
/// \param[out] Ix  x derivative
/// \param[out] Iy  y derivative
/// \param[out] Iz  temporal derivative
///////////////////////////////////////////////////////////////////////////////
static void ComputeDerivatives(const float *I0, const float *I1, int w, int h,
							   int s, float *Ix, float *Iy, float *Iz) {
	#pragma omp parallel for
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			const int pos = j + i * s;
			float     t0, t1;
			// derivative filter is (1, -8, 0, 8, -1)/12
			// x derivative
			t0 = Tex2Di(I0, w, h, s, j - 2, i);
			t0 -= Tex2Di(I0, w, h, s, j - 1, i) * 8.0f;
			t0 += Tex2Di(I0, w, h, s, j + 1, i) * 8.0f;
			t0 -= Tex2Di(I0, w, h, s, j + 2, i);
			t0 /= 12.0f;

			t1 = Tex2Di(I1, w, h, s, j - 2, i);
			t1 -= Tex2Di(I1, w, h, s, j - 1, i) * 8.0f;
			t1 += Tex2Di(I1, w, h, s, j + 1, i) * 8.0f;
			t1 -= Tex2Di(I1, w, h, s, j + 2, i);
			t1 /= 12.0f;

			// spatial derivatives are averaged
			Ix[pos] = (t0 + t1) * 0.5f;

			// t derivative
			Iz[pos] = I1[pos] - I0[pos];

			// y derivative
			t0 = Tex2Di(I0, w, h, s, j, i - 2);
			t0 -= Tex2Di(I0, w, h, s, j, i - 1) * 8.0f;
			t0 += Tex2Di(I0, w, h, s, j, i + 1) * 8.0f;
			t0 -= Tex2Di(I0, w, h, s, j, i + 2);
			t0 /= 12.0f;

			t1 = Tex2Di(I1, w, h, s, j, i - 2);
			t1 -= Tex2Di(I1, w, h, s, j, i - 1) * 8.0f;
			t1 += Tex2Di(I1, w, h, s, j, i + 1) * 8.0f;
			t1 -= Tex2Di(I1, w, h, s, j, i + 2);
			t1 /= 12.0f;

			Iy[pos] = (t0 + t1) * 0.5f;
		}
	}
}
