%{
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include "token.h"
#include "symbolTable.h"

#define DEFINED 1

extern void scanner_use_file(char*);
extern int yylex(void);

extern int yydebug;
extern int optind;

void yyerror(const char* msg);
void print_value(token_t* tok);
void print_input(token_t* tok);
void print_token(token_t* tok);
const char* token_name(int token_class);

Scope* record_types = new Scope("record");
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
%token <token> RECTYPE
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

%start program

%%

program					: declarationList
						;

declarationList			: declarationList declaration
						| declaration
						;

declaration				: varDeclaration
						| funDeclaration
						| recDeclaration
						;

recDeclaration			: RECORD ID '{' localDeclarations '}' {
							record_types->insert(std::string($2->value.str_val),
								(void*) DEFINED);
						}
						;

varDeclaration			: typeSpecifier varDeclList ';'
						;

scopedVarDeclaration	: scopedTypeSpecifier varDeclList ';'
						;

varDeclList				: varDeclList ',' varDeclInitialize
						| varDeclInitialize
						;

varDeclInitialize		: varDeclId
						| varDeclId ':' simpleExpression
						;

varDeclId				: ID
						| ID '[' NUMCONST ']'
						;

scopedTypeSpecifier		: STATIC typeSpecifier
						| typeSpecifier
						;

typeSpecifier			: returnTypeSpecifier
						| RECTYPE
						;

returnTypeSpecifier		: INT
						| BOOL
						| CHAR
						;

funDeclaration			: typeSpecifier ID '(' params ')' statement
						| ID '(' params ')' statement
						;

params					: paramList
						| %empty
						;

paramList				: paramList ';' paramTypeList
						| paramTypeList
						;

paramTypeList			: typeSpecifier paramIdList
						;

paramIdList				: paramIdList ',' paramId
						| paramId
						;

paramId					: ID
						| ID '[' ']'
						;

statement				: matchedStmt
						| unmatchedStmt
						;

matchedStmt				: IF '(' simpleExpression ')' matchedStmt ELSE matchedStmt
						| WHILE '(' simpleExpression ')' matchedStmt
						| otherStmt
						;

unmatchedStmt			: IF '(' simpleExpression ')' matchedStmt
						| IF '(' simpleExpression ')' unmatchedStmt
						| IF '(' simpleExpression ')' matchedStmt ELSE unmatchedStmt
						| WHILE '(' simpleExpression ')' unmatchedStmt
						;

otherStmt				: expressionStmt
						| compoundStmt
						| returnStmt
						| breakStmt
						;

compoundStmt			: '{' localDeclarations statementList '}'
						;

localDeclarations		: localDeclarations scopedVarDeclaration
						| %empty
						;

statementList			: statementList statement
						| %empty
						;

expressionStmt			: expression ';'
						| ';'
						;

returnStmt				: RETURN ';'
						| RETURN expression ';'
						;

breakStmt				: BREAK ';'
						;

expression				: mutable '=' expression
						| mutable ADDASS expression
						| mutable SUBASS expression
						| mutable MULASS expression
						| mutable DIVASS expression
						| mutable INC
						| mutable DEC
						| simpleExpression
						;

simpleExpression		: simpleExpression OR andExpression
						| andExpression
						;

andExpression			: andExpression AND unaryRelExpression
						| unaryRelExpression
						;

unaryRelExpression		: NOT unaryRelExpression
						| relExpression
						;

relExpression			: sumExpression relop sumExpression
						| sumExpression
						;

relop					: LESSEQ
						| '<'
						| '>'
						| GRTEQ
						| EQ
						| NOTEQ
						;

sumExpression			: sumExpression sumop term
						| term
						;

sumop					: '+'
						| '-'
						;

term					: term mulop unaryExpression
						| unaryExpression
						;

mulop					: '*'
						| '/'
						| '%'
						;

unaryExpression			: unaryop unaryExpression
						| factor
						;

unaryop					: '-'
						| '*'
						| '?'
						;

factor					: immutable
						| mutable
						;

mutable					: ID
						| mutable '[' expression ']'
						| mutable '.' ID
						;

immutable				: '(' expression ')'
						| call
						| constant
						;

call					: ID '(' args ')'
						;

args					: argList
						| %empty
						;

argList					: argList ',' expression
						| expression
						;

constant				: NUMCONST
						| CHARCONST
						| BOOLCONST
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
		name = (char*) yytname[token_class - 258 + 3];
	} else {
		/* Implicit single-character token type */
		name = (char*) malloc(sizeof(char) * 2);
		snprintf(name, (size_t) 2, "%c", (char) token_class);
	}

	return (const char*) name;
}

int main(int argc, char** argv) {
	char c;
	int i;

	while ((c = getopt(argc, argv, (char*) "d")) != -1) {
		switch (c) {
			case 'd':
				yydebug = 1;
				break;
			case '?':
			default:
				fprintf(stderr, "getopt: case '%c'\n", c);
				exit(1);
		}
	}

	switch (argc - optind) {
		case 1:
			scanner_use_file(argv[optind]);
			break;
		case 0:
			/* read from STDIN */
			break;
		default:
			fprintf(stderr, "%s: invalid arguments:\n", argv[0]);
			for (i = optind; i < argc; i++) {
				fprintf(stderr, "  %i: %s\n", i, argv[i]);
			}
			exit(1);
	}

	yyparse();

	exit(0);
}
