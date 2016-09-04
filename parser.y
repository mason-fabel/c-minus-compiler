%{
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

extern void scanner_use_file(char*);
extern int yylex(void);

void yyerror(const char* msg);
void print_value(token_t* tok);
void print_input(token_t* tok);
void print_token(token_t* tok);
const char* token_name(int token_class);
%}

%union {
	token_t* token;
}

%token ADDASS AND BOOL BOOLCONST BREAK CHAR CHARCONST DEC DIVASS ELSE EQ GRTEQ ID IF INC INT LESSEQ MULASS NOT NOTEQ NUMCONST OR RECORD RETURN STATIC SUBASS WHILE

%type<token> ADDASS AND BOOL BOOLCONST BREAK CHAR CHARCONST DEC DIVASS ELSE EQ GRTEQ ID IF INC INT LESSEQ MULASS NOT NOTEQ NUMCONST OR RECORD RETURN STATIC SUBASS WHILE

%token-table


%start token_list

%%

token_list
	: token 
	| token_list token
	;

token
	: BOOLCONST	{ print_token($1); }
	| CHARCONST	{ print_token($1); }
	| ID		{ print_token($1); }
	| NUMCONST	{ print_token($1); }
	;

%%

void yyerror(const char* msg) {
	fprintf(stdout, "parser error: %s\n", msg);

	return;
}

void print_value(token_t* tok) {
	switch (tok->value_mode) {
		case MODE_NONE:
			break;
		case MODE_INT:
			fprintf(stdout, " Value: %i", tok->value.int_val);
			break;
		case MODE_CHAR:
			fprintf(stdout, " Value: \'%c\'", tok->value.char_val);
			break;
		case MODE_STR:
			fprintf(stdout, " Value: %s", tok->value.str_val);
			break;
	}

	return;
}

void print_input(token_t* tok) {
	fprintf(stdout, " Input: %s", tok->input);

	return;
}

void print_token(token_t* tok) {
	fprintf(stdout, "Line %i Token: %s", tok->lineno, token_name(tok->type));

	switch (tok->type) {
		case BOOLCONST:
		case CHARCONST:
		case ID:
		case NUMCONST:
			print_value(tok);
	}

	switch (tok->type) {
		case BOOLCONST:
		case CHARCONST:
		case NUMCONST:
			print_input(tok);
	}

	fprintf(stdout, "\n");

	return;
}

const char* token_name(int token_class) {
	/* NOTE: We first undo an offset of 258 introduced by the flex token class
	 * numbering, and then add 3 as bison puts 3 tokens ("$end", "error", and
	 * "$undefined") at the begining of the toke name table.
	 */
	return yytname[token_class - 258 + 3];
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
