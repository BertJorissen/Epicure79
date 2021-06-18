/*
LICENSE TERMS

Copyright (c)2008-2014 University of Virginia
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted without royalty fees or other restrictions, provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the University of Virginia, the Dept. of Computer Science, nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF VIRGINIA OR THE SOFTWARE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#define AMB_TEMP (80.0f)

// Block size
#ifndef BSIZE
	#ifdef AMD
		#define BSIZE 16
	#else
		#define BSIZE 512
	#endif
#endif

#ifndef BLOCK_X
	#define BLOCK_X 16
#endif

#ifndef BLOCK_Y
	#define BLOCK_Y 16
#endif

// Vector size
#ifndef SSIZE
	#ifdef AOCL_BOARD_de5net_a7
		#define SSIZE 8
	#elif AOCL_BOARD_p385a_sch_ax115
		#define SSIZE 8
	#elif AMD
		#define SSIZE 4
	#endif
#endif

// Radius of stencil, e.g 5-point stencil => 1
#ifndef RAD
  #define RAD  1
#endif

// Number of parallel time steps
#ifndef TIME
	#ifdef AOCL_BOARD_de5net_a7
		#define TIME 6
	#elif AOCL_BOARD_p385a_sch_ax115
		#define TIME 21
	#elif AMD
		#define TIME 1
	#endif
#endif

// Padding to fix alignment for time steps that are not a multiple of 8
#ifndef PAD
  #define PAD TIME % 16
#endif

#define HALO_SIZE		TIME * RAD			// halo size
#define BACK_OFF		2 * HALO_SIZE			// back off for going to next block
