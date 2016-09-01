%{
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

extern void scanner_use_file(char*);
extern int yylex(void);

void yyerror(const char* msg);
%}

%union {
	token_t token;
}

%token ADDASS AND BOOL BOOLCONST BREAK CHAR CHARCONST DEC DIVASS ELSE EQ GRTEQ ID IF INC INT LESSEQ MULASS NOT NOTEQ NUMCONST OR RECORD RETURN STATIC SUBASS WHILE

%type<token> ADDASS AND BOOL BOOLCONST BREAK CHAR CHARCONST DEC DIVASS ELSE EQ GRTEQ ID IF INC INT LESSEQ MULASS NOT NOTEQ NUMCONST OR RECORD RETURN STATIC SUBASS WHILE



%start token_list

%%

token_list
	: token 
	| token_list token
	;

token
	: AND {
		fprintf(stdout, "Line %i Token: AND\n", $1.lineno);
	}
	| BOOL {
		fprintf(stdout, "Line %i Token: BOOL\n", $1.lineno);
	}
	| BOOLCONST {
		fprintf(stdout, "Line %i Token: BOOLCONST Value: %i  Input: %s\n",
			$1.lineno, $1.value.int_val, $1.input);
	}
	| BREAK {
		fprintf(stdout, "Line %i Token: BREAK\n", $1.lineno);
	}
	| CHAR {
		fprintf(stdout, "Line %i Token: CHAR\n", $1.lineno);
	}
	| CHARCONST {
		fprintf(stdout, "Line %i Token: CHARCONST Value: %c Input: %s\n",
			$1.lineno, $1.value.char_val, $1.input);
	}
	| ELSE {
		fprintf(stdout, "Line %i Token: ELSE\n", $1.lineno);
	}
	| ID {
		fprintf(stdout, "Line %i Token: ID Value: %s\n",
			$1.lineno, $1.value.str_val);
	}
	| INT {
		fprintf(stdout, "Line %i Token: INT\n", $1.lineno);
	}
	| NOT {
		fprintf(stdout, "Line %i Token: NOT\n", $1.lineno);
	}
	| NUMCONST {
		fprintf(stdout, "Line %i Token: NUMCONST Value: %i  Input: %s\n",
			$1.lineno, $1.value.int_val, $1.input);
	}
	| OR {
		fprintf(stdout, "Line %i Token: OR\n", $1.lineno);
	}
	| RECORD {
		fprintf(stdout, "Line %i Token: RECORD\n", $1.lineno);
	}
	| RETURN {
		fprintf(stdout, "Line %i Token: RETURN\n", $1.lineno);
	}
	| STATIC {
		fprintf(stdout, "Line %i Token: STATIC\n", $1.lineno);
	}
	| WHILE {
		fprintf(stdout, "Line %i Token: WHILE\n", $1.lineno);
	}
	;

%%

void yyerror(const char* msg) {
	fprintf(stdout, "parser error: %s\n", msg);
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
