%{
#include <stdio.h>
#include "parser.h"

extern void yyerror(const char*);

void scanner_use_file(char* fname);
%}

%option noyywrap

%%

[aA] { return A; }
[bB] { return B; }
[ \t\n]+ { /* whitespace - do nothing */ }
. {
	char msg[100];

	sprintf(msg, "unknown char: '%c'", yytext[0]);
	yyerror(msg);

	return yytext[0];
}

%%

void scanner_use_file(char* fname) {
	FILE* fin;

	fin = fopen(fname, "r");
	if (fin == NULL) {
		fprintf(stderr, "unable to open file: %s\n", fname);
		exit(1);
	}

	yy_switch_to_buffer(yy_create_buffer(fin, YY_BUF_SIZE));
}
