%{
#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "parser.h"
#include "symtab.h"

void scanner_error(void);
void scanner_use_file(char* fname);
int create_token(int token_class);

extern Scope* record_types;
%}

%option yylineno
%option noyywrap

%%

and						{ return create_token(AND); }
bool					{ return create_token(BOOL); }
break					{ return create_token(BREAK); }
char					{ return create_token(CHAR); }
else					{ return create_token(ELSE); }
if						{ return create_token(IF); }
int						{ return create_token(INT); }
not						{ return create_token(NOT); }
or						{ return create_token(OR); }
record					{ return create_token(RECORD); }
return					{ return create_token(RETURN); }
static					{ return create_token(STATIC); }
while					{ return create_token(WHILE); }
true|false				{ return create_token(BOOLCONST); }
[a-zA-Z][a-zA-Z0-9]*	{ return create_token(ID); }
'\\?.'					{ return create_token(CHARCONST); }
[0-9]+					{ return create_token(NUMCONST); }
\=\=					{ return create_token(EQ); }
\>\=					{ return create_token(GRTEQ); }
\<\=					{ return create_token(LESSEQ); }
\!\=					{ return create_token(NOTEQ); }
\-\-					{ return create_token(DEC); }
\+\+					{ return create_token(INC); }
\+\=					{ return create_token(ADDASS); }
\/\=					{ return create_token(DIVASS); }
\*\=					{ return create_token(MULASS); }
\-\=					{ return create_token(SUBASS); }
[\+\-\*\/\%\?]			{ return create_token((int) yytext[0]); }
[\=\<\>]				{ return create_token((int) yytext[0]); }
[\(\)\[\]\{\}]			{ return create_token((int) yytext[0]); }
[\.\,\:\;]				{ return create_token((int) yytext[0]); }
[ \t\n]+				{ /* whitespace - do nothing */ }
\/\/.*					{ /* comment - do nothing */ }
.						{ scanner_error(); }

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

int create_token(int token_class) {
	yylval.token = (token_t*) malloc(sizeof(token_t));

	if (yylval.token == NULL) {
		fprintf(stderr,
			"ERROR(%i): Unable to allocate memory for token: \"%s\"\n",
			yylineno, yytext);
	}

	yylval.token->type = token_class;
	yylval.token->lineno = yylineno;
	yylval.token->input = strdup(yytext);

	switch (token_class) {
		case BOOLCONST:
			yylval.token->value_mode = MODE_INT;
			yylval.token->value.int_val = yytext[0] == 't' ? 1 : 0;
			break;

		case NUMCONST:
			yylval.token->value_mode = MODE_INT;
			yylval.token->value.int_val = atoi(yytext);
			break;

		case CHARCONST:
			yylval.token->value_mode = MODE_CHAR;
			if (strlen(yytext) == 3) {
				/* no escape sequence */
				yylval.token->value.char_val = yytext[1];
			} else {
				/* escape sequence */
				switch (yytext[2]) {
					case '0':
						yylval.token->value.char_val = '\0';
						break;
					case 'n':
						yylval.token->value.char_val = '\n';
						break;
					default:
						yylval.token->value.char_val = yytext[2];
				}
			}
			break;

		case ID:
			if (record_types->lookup(std::string(yytext)) != NULL) {
				return create_token(RECTYPE);
			}

			yylval.token->value_mode = MODE_STR;
			yylval.token->value.str_val = strdup(yytext);
			break;

		default:
			yylval.token->value_mode = MODE_NONE;
	}

	return token_class;
}
