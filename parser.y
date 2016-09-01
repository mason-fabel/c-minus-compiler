%{
#include <stdio.h>
#include <stdlib.h>

extern void scanner_use_file(char*);
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

int main(int argc, char** argv) {
	int i;

	if (argc == 1) {
		/* read from stdin - do nothing */
	} else if (argc == 2) {
		/* read from the file named in the first argument */
		scanner_use_file(argv[1]);
	} else {
		for (i = 2; i < argc; i++) {
			fprintf(stderr, "%s: invalid argument: %s\n", argv[0], argv[i]);
		}
		exit(1);
	}

	yyparse();

	exit(0);
}
