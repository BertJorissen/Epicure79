/* @author: Gabriel Rodriguez-Canal                                                          */
/* This lex program extracts the necessary statements from the Controllers code to generate  */
/* the kernel source file that can be compiled with aoc. These statements, sorted, are:      */
/* Ctrl_NewType( ... )                                                                       */
/* CTRL_KERNEL_CHAR( ... )                                                                   */
/* CTRL_KERNEL_PROTO( ... )                                                                  */
/* CTRL_KERNEL( ... )                                                                        */
/* Three buffers are necessary to guarantee the statements are produced in the proper order  */

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #define CTRL_KERNEL_BUF 100000
    #define CTRL_KERNEL_PROTO_BUF 1000

    int unmatched_parens = 0;
    char ctrl_kernel_buf[CTRL_KERNEL_BUF];
    char ctrl_kernel_proto_buf[CTRL_KERNEL_PROTO_BUF];
    char ctrl_kernel_char_buf[CTRL_KERNEL_PROTO_BUF];
%}

%option noyywrap

spaces [\t\n ]
sep ;
arg [ A-Za-z0-9_*-]+{spaces}*
new_t Ctrl_NewType{spaces}*"("([^\)]|{spaces}])*")"{spaces}*{sep}*{spaces}*
k_char_1 CTRL_KERNEL_CHAR{spaces}*"("({arg},{spaces}*)+KERNEL_PIPELINE"("
k_proto CTRL_KERNEL_PROTO{spaces}*"("(({arg}|,){spaces}*)+")"{spaces}*{sep}*{spaces}*
ctrl_k_signature CTRL_KERNEL{spaces}*"("{spaces}*({arg},)+[\t\n ]"("


/* extracts argument inside parenthesis */
%x kernel_body k_char_args
%%
{new_t} {
    strcat(ctrl_kernel_char_buf, yytext);
}
{k_char_1} {
    strcat(ctrl_kernel_char_buf, yytext);
    unmatched_parens = 1;
    BEGIN k_char_args;
}
{k_proto} {
    strcat(ctrl_kernel_proto_buf, yytext);
}
{ctrl_k_signature} {
    strcat(ctrl_kernel_buf, yytext);
    unmatched_parens = 1;
    BEGIN kernel_body;
}


.|\n ;

<kernel_body>{spaces}*[^\(^\)]+"(" {   
    unmatched_parens++; 
    strcat(ctrl_kernel_buf, yytext);
}
<kernel_body>[^\(^\)]*")"{spaces}*{sep}*{spaces}* {
    strcat(ctrl_kernel_buf, yytext);

    if (!unmatched_parens)
        BEGIN INITIAL;

    unmatched_parens--;
}
<kernel_body>(.|\n) ;

<k_char_args>{spaces}*[^\(^\)]+"(" {   
    unmatched_parens++; 
    strcat(ctrl_kernel_char_buf, yytext);
}
<k_char_args>[^\(^\)]*")"{spaces}*{sep}*{spaces}* {
    strcat(ctrl_kernel_char_buf, yytext);

    if (!unmatched_parens)
        BEGIN INITIAL;

    unmatched_parens--;
}
<k_char_args>(.|\n) ;


<<EOF>>   { return 0 ; } 

%%
int main() {
    yylex();

    printf("#define CTRL_FPGA_KERNEL_FILE\n");
    printf("#include \"Ctrl.h\"\n\n");
    printf("%s\n", ctrl_kernel_char_buf);
    printf("%s\n", ctrl_kernel_proto_buf);
    printf("%s\n", ctrl_kernel_buf);
}