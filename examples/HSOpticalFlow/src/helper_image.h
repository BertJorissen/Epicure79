/**
 * @file helper_image.h
 * @brief HSOpticalFlow: Functions to load ppm images.
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

// These are helper functions for the SDK samples (image,bitmap)
#ifndef COMMON_HELPER_IMAGE_H_
#define COMMON_HELPER_IMAGE_H_

#include <stdlib.h>

#include <stdio.h>
#include <string.h>

//! size of PGM file header
const unsigned int PGMHeaderSize = 0x40;

#ifndef FOPEN
#define FOPEN(fHandle, filename, mode) (fHandle = fopen(filename, mode))
#endif
#ifndef FOPEN_FAIL
#define FOPEN_FAIL(result) (result == NULL)
#endif

static inline bool __loadPPM(const char *file, unsigned char **data, unsigned int *w, unsigned int *h, unsigned int *channels) {
	FILE *fp = NULL;

	if (FOPEN_FAIL(FOPEN(fp, file, "rb"))) {
		printf("__LoadPPM() : Failed to open file: %s\n", file);
		return false;
	}

	// check header
	char header[PGMHeaderSize];

	if (fgets(header, PGMHeaderSize, fp) == NULL) {
		printf("_LoadPPM() : reading PGM header returned NULL\n");
		return false;
	}

	if (strncmp(header, "P5", 2) == 0) {
		*channels = 1;
	} else if (strncmp(header, "P6", 2) == 0) {
		*channels = 3;
	} else {
		printf("__LoadPPM() : File is not a PPM or PGM image\n");
		*channels = 0;
		return false;
	}

	// parse header, read maxval, width and height
	unsigned int width  = 0;
	unsigned int height = 0;
	unsigned int maxval = 0;
	unsigned int i      = 0;

	while (i < 3) {
		if (fgets(header, PGMHeaderSize, fp) == NULL) {
			printf("__LoadPPM() : reading PGM header returned NULL\n");
			return false;
		}

		if (header[0] == '#') {
			continue;
		}

		if (i == 0) {
			i += sscanf(header, "%u %u %u", &width, &height, &maxval);
		} else if (i == 1) {
			i += sscanf(header, "%u %u", &height, &maxval);
		} else if (i == 2) {
			i += sscanf(header, "%u", &maxval);
		}
	}

	// check if given handle for the data is initialized
	if (NULL != *data) {
		if (*w != width || *h != height) {
			printf("__LoadPPM() : Invalid image dimensions.\n");
		}
	} else {
		*data = (unsigned char *)malloc(sizeof(unsigned char) * width * height * *channels);
		*w    = width;
		*h    = height;
	}

	// read and close file
	if (fread(*data, sizeof(unsigned char), width * height * *channels, fp) == 0) {
		printf("__LoadPPM() read data returned error.\n");
	}

	fclose(fp);

	return true;
}

static inline bool sdkLoadPPM4ub(const char *file, unsigned char **data, unsigned int *w, unsigned int *h) {
	unsigned char *idata = 0;
	unsigned int   channels;

	if (__loadPPM(file, &idata, w, h, &channels)) {
		// pad 4th component
		int size = *w * *h;
		// keep the original pointer
		unsigned char *idata_orig = idata;
		*data                     = (unsigned char *)malloc(sizeof(unsigned char) * size * 4);
		unsigned char *ptr        = *data;

		for (int i = 0; i < size; i++) {
			*ptr++ = *idata++;
			*ptr++ = *idata++;
			*ptr++ = *idata++;
			*ptr++ = 0;
		}

		free(idata_orig);
		return true;
	} else {
		free(idata);
		return false;
	}
}

#endif  // COMMON_HELPER_IMAGE_H_
