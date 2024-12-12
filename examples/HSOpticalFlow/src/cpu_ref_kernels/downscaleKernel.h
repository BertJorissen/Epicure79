/**
 * @file downscaleKernel.h
 * @brief Cpu reference downscale kernel for HSOpticalflow.
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
/// \brief resize image
/// \param[in]  src         image to downscale
/// \param[in]  width       image width
/// \param[in]  height      image height
/// \param[in]  stride      image stride
/// \param[in]  newWidth    image new width
/// \param[in]  newHeight   image new height
/// \param[in]  newStride   image new stride
/// \param[out] out         downscaled image data
///////////////////////////////////////////////////////////////////////////////
static void Downscale(const float *src, int width, int height, int stride,
					  int newWidth, int newHeight, int newStride, float *out) {
	#pragma omp parallel for
	for (int i = 0; i < newHeight; ++i) {
		for (int j = 0; j < newWidth; ++j) {
			const int srcX = j * 2;
			const int srcY = i * 2;
			// average 4 neighbouring pixels
			float sum;
			sum = Tex2Di(src, width, height, stride, srcX + 0, srcY + 0);
			sum += Tex2Di(src, width, height, stride, srcX + 0, srcY + 1);
			sum += Tex2Di(src, width, height, stride, srcX + 1, srcY + 0);
			sum += Tex2Di(src, width, height, stride, srcX + 1, srcY + 1);
			// normalize
			sum *= 0.25f;
			out[j + i * newStride] = sum;
		}
	}
}