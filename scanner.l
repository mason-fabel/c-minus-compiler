%{
#include <stdio.h>
#include "parser.tab.h"

extern void yyerror(const char*);
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
