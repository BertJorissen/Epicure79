/**
 * @file SWcommon_ref.c
 * Common C reference definitions and utilities for the Smith-Waterman algorihtm.
 *
 * @author Javier Fresno
 * @date Jun 2013
 */

/*
 * <license>
 * 
 * Hitmap v1.3
 * 
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Copyright (c) 2007-2019, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

// Headers
#include "SWcommon.h"
#include "SWcommon_ref.h"



void readProtein(ProteinFile * fprotein, aa_t * protein, int psize){


	protein[0] = GAP_AA;

	int i = 1;
	char ch;
	while(i<=psize){

		int res = fscanf(fprotein->file,"%c",&ch);

		if(res == EOF){
			fprotein->rewind++;
			rewind(fprotein->file);
			continue;
		}

		aa_t aa = char2AA(ch);
		if( aa != NOTFOUND_AA ){
			protein[i++] = aa;
		}

	}

}


void printProtein(aa_t * p, int psize){

	// Print
	int i;
	for(i=1; i<psize+1; i++){
		printf("%c",AA2char(p[i]));
	}
	printf("\n");


}

/**
 * Print the protein to stdout
 */
void printProteinMatch(aa_t * p, int psize, int begin, int end){

	// Print
	int i;
	for(i=1; i<psize+1; i++){

		if(i>begin && i<end+1)
			printf("%c", (char) toupper(AA2char(p[i])));
		else
			printf("%c",AA2char(p[i]));
	}
	printf("\n");
}





double currentTime(){
    struct timeval tim;
    gettimeofday(&tim, NULL);
    return (double) tim.tv_sec + ((double)tim.tv_usec / (1000*1000) );
};

