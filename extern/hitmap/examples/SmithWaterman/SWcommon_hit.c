/**
 * @file SWcommon_hit.c
 * Common hitmap definitions and utilities for the Smith-Waterman algorihtm.
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

// Include the header
#include "SWcommon_hit.h"



/**
 * Read the protein from a file.
 */
void readProtein(ProteinFile * fprotein, HitTile_aa_t * protein, int psize){

	hit_tileElemAt(*protein,1,0) = GAP_AA;

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
			hit_tileElemAt(*protein,1,i++) = aa;

		}
	}

}




void printProtein(HitTile_aa_t p, int offset, int psize){

	// Print
	int i;
	for(i=1+offset; i<psize+offset+1; i++){
		printf("%c",AA2char( hit_tileElemAt(p,1,i) ));
	}
	printf("\n");

}


void printProteinMatch(HitTile_aa_t p, int begin, int end){

	// Print
	int i;
	for(i=1; i< hit_tileDimCard(p,0); i++){

		if(i>begin && i<end+1)
			printf("%c", (char) toupper(AA2char(hit_tileElemAt(p,1,i))));
		else
			printf("%c",AA2char(hit_tileElemAt(p,1,i)));
	}
	printf("\n");


}


