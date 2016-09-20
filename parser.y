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
%token <token> ADDASS
%token <token> AND
%token <token> BOOL
%token <token> BOOLCONST
%token <token> BREAK
%token <token> CHAR
%token <token> CHARCONST
%token <token> DEC
%token <token> DIVASS
%token <token> ELSE
%token <token> EQ
%token <token> GRTEQ
%token <token> ID
%token <token> IF
%token <token> INC
%token <token> INT
%token <token> LESSEQ
%token <token> MULASS
%token <token> NOT
%token <token> NOTEQ
%token <token> NUMCONST
%token <token> OR
%token <token> RECORD
%token <token> RETURN
%token <token> STATIC
%token <token> SUBASS
%token <token> WHILE

%type<token> '+'
%type<token> '-'
%type<token> '*'
%type<token> '/'
%type<token> '%'
%type<token> '?'
%type<token> '='
%type<token> '<'
%type<token> '>'
%type<token> '('
%type<token> ')'
%type<token> '['
%type<token> ']'
%type<token> '{'
%type<token> '}'
%type<token> '.'
%type<token> ','
%type<token> ':'
%type<token> ';'

%token-table


%start token_list

%%

token_list
	: token 
	| token_list token
	;

token
	: '+'		{ print_token($1); }
	| '-'		{ print_token($1); }
	| '*'		{ print_token($1); }
	| '/'		{ print_token($1); }
	| '%'		{ print_token($1); }
	| '?'		{ print_token($1); }
	| '='		{ print_token($1); }
	| '<'		{ print_token($1); }
	| '>'		{ print_token($1); }
	| '('		{ print_token($1); }
	| ')'		{ print_token($1); }
	| '['		{ print_token($1); }
	| ']'		{ print_token($1); }
	| '{'		{ print_token($1); }
	| '}'		{ print_token($1); }
	| '.'		{ print_token($1); }
	| ','		{ print_token($1); }
	| ':'		{ print_token($1); }
	| ';'		{ print_token($1); }
	| AND		{ print_token($1); }
	| ADDASS	{ print_token($1); }
	| BREAK		{ print_token($1); }
	| BOOL		{ print_token($1); }
	| BOOLCONST	{ print_token($1); }
	| CHAR		{ print_token($1); }
	| CHARCONST	{ print_token($1); }
	| DEC		{ print_token($1); }
	| DIVASS	{ print_token($1); }
	| ELSE		{ print_token($1); }
	| EQ		{ print_token($1); }
	| GRTEQ		{ print_token($1); }
	| ID		{ print_token($1); }
	| IF		{ print_token($1); }
	| INC		{ print_token($1); }
	| INT		{ print_token($1); }
	| LESSEQ	{ print_token($1); }
	| MULASS	{ print_token($1); }
	| NOT		{ print_token($1); }
	| NOTEQ		{ print_token($1); }
	| NUMCONST	{ print_token($1); }
	| OR		{ print_token($1); }
	| RECORD	{ print_token($1); }
	| RETURN	{ print_token($1); }
	| STATIC	{ print_token($1); }
	| SUBASS	{ print_token($1); }
	| WHILE		{ print_token($1); }
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
	fprintf(stdout, "  Input: %s", tok->input);

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
	char* name;

	if (token_class >= 258) {
		/* NOTE: We first undo an offset of 258 introduced by the flex token
		 * class numbering, and then add 3 as bison puts 3 tokens ("$end",
		 * "error", "$undefined") at the begining of the toke name table.
		 */
		name = yytname[token_class - 258 + 3];
	} else {
		/* Implicit single-character token type */
		name = malloc(sizeof(char) * 2);
		snprintf(name, (size_t) 2, "%c", (char) token_class);
	}

	return name;
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
