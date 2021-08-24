/*
 * <license>
 * 
 * Controller v2.1
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
 * Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    char* option = argv[1];

    if(argc < 4)
        return -1;

    if(!strcmp(option, "host") && !strcmp(option, "kernel"))
        return -2;

    FILE *kfile = fopen(argv[2], "r");

    size_t kfile_size;

    fseek(kfile, 0, SEEK_END);
    kfile_size =  ftell(kfile);
    rewind(kfile);

    /* @Gabriel Rodriguez-Canal
     *   Extraction of CTRL_KERNEL primitive. It is written into a new file.
     */
    char *kfile_str = (char*)malloc(kfile_size * sizeof(char));
    fread(kfile_str, kfile_size * sizeof(char), 1, kfile);

    char *ctrl_kernel = NULL;
    ctrl_kernel = strstr(kfile_str, "CTRL_KERNEL");

    char *ctrl_kernel_start = ctrl_kernel;


    /* Find first parentheses */
    int n_parens = 0;

    int n_chars = 0; // Number of characters CTRL_KERNEL comprises

    for(; *ctrl_kernel != '(' && *ctrl_kernel != 0; ctrl_kernel++, n_chars++);
    ctrl_kernel++;
    n_chars++;
    n_parens++;

    for(; n_parens != 0 && *ctrl_kernel != 0; ctrl_kernel++) {
        if(*ctrl_kernel == '(')
            n_parens++;
        else if(*ctrl_kernel == ')')
            n_parens--;
    
        n_chars++;
    }



    /* @Gabriel Rodriguez-Canal
     * Extraction of includes. Both #include <.> and #include "." cases are considered.
     */
    FILE *ctrl_kernel_file = fopen(argv[3], "w");
    char *includes = (char*)malloc(1000 * sizeof(char));
    char *includes_head = includes;

    size_t header_len;
    char *header;
    if(strcmp(option, "kernel") == 0) {
        //header = "#define CTRL_FPGA_KERNEL_FILE\n\n#define CTRL_FPGA_BIN_NAME\n\n#include <stdio.h>\n#include \"/home/u46403/cmake-pruebas/macros_gen_name.h\"\n\n";
        header = "#define CTRL_FPGA_KERNEL_FILE\n\n#define CTRL_FPGA_BIN_NAME\n\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n\n";
    }
    else if(strcmp(option, "host") == 0) {
        header = "#define CTRL_FPGA_KERNEL_FILE\n\n";
    }
    strcpy(includes, header);
    includes += strlen(header);


    while((kfile_str = strstr(kfile_str, "#include")) != NULL) {
        while(*kfile_str != '<' && *kfile_str != '\"') {
            *includes = *kfile_str;
            includes++;
            kfile_str++;
        }

        if(*kfile_str == '<') {
            while(*kfile_str != '>') {
                *includes = *kfile_str;
                includes++;
                kfile_str++;
            }
        }
        else if(*kfile_str == '\"') {
            do {
                *includes = *kfile_str;
                includes++;
                kfile_str++;
            } while(*kfile_str != '\"');
        }
        *includes = *kfile_str;
        includes++;
        strcat(includes, "\n");
        includes++;
    }
    *includes = 0;

    fwrite(includes_head, strlen(includes_head), 1, ctrl_kernel_file);

    
    fwrite(ctrl_kernel_start, sizeof(char), n_chars, ctrl_kernel_file);

    if(strcmp(option, "kernel") == 0) {
        char *program = "\n\nint main() {\n\n \
            printf(\"%s\", cpar_kname);\n\n \
            if(cpar_simd)\n\n \
                printf(\"%s\", cpar_simd);\n\n \
            if(cpar_cu)\n\n \
                printf(\"%s\", cpar_cu);\n\n \
            #ifdef FPGA_PROFILING\n\n \
                printf(\"_profiling\");\n\n \
            #elif FPGA_EMULATION\n\n \
                printf(\"_emu\");\n\n \
            #endif\n\n \
            printf(\"_Ctrl\");\n\n \
        }";

        fwrite(program, strlen(program), 1, ctrl_kernel_file);
    }

    fclose(kfile);
    fclose(ctrl_kernel_file);
}
