%{
#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "parser.h"

void scanner_error(void);
void scanner_use_file(char* fname);
%}

%option yylineno
%option noyywrap

%%

and {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return AND;
}

bool {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return BOOL;
}

break {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return BREAK;
}

char {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return CHAR;
}

else {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return ELSE;
}

int {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return INT;
}

not {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return NOT;
}

or {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return OR;
}

record {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return RECORD;
}

return {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return RETURN;
}

static {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return STATIC;
}

while {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);

	return WHILE;
}

true {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);
	yylval.token.value.int_val = 1;

	return BOOLCONST;
}

false {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);
	yylval.token.value.int_val = 0;

	return BOOLCONST;
}

[a-zA-Z][a-zA-Z0-9]* {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);
	yylval.token.value.str_val = strdup(yytext);

	return ID;
}

'\.' {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);
	yylval.token.value.char_val = yytext[1];

	return CHARCONST;
}

[0-9]+ {
	yylval.token.lineno = yylineno;
	yylval.token.input = strdup(yytext);
	yylval.token.value.int_val = atoi(yytext);

	return NUMCONST;
}

[ \t\n]+ { /* whitespace - do nothing */ }

\/\/.* { /* comment - do nothing */ }

. { scanner_error(); }

%%

void scanner_error(void) {
	fprintf(stdout, "ERROR(%i): Invalid or misplaced input character: \"%c\"\n",
		yylineno, yytext[0]);
}

void scanner_use_file(char* fname) {
	FILE* fin;

	fin = fopen(fname, "r");
	if (fin == NULL) {
		fprintf(stderr, "unable to open file: %s\n", fname);
		exit(1);
	}

	yy_switch_to_buffer(yy_create_buffer(fin, YY_BUF_SIZE));
}
