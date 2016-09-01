%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex(void);

void yyerror(const char* msg);
%}

%token A
%token B
%token LPAREN
%token RPAREN

%start expr

%%

expr : expr expr | A | B | /* empty */;

%%


void yyerror(const char* msg) {
	fprintf(stderr, "bison error: %s\n", msg);
}

int main() {
	yyparse();

	exit(0);
}
