%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"
#include "ast.h"
#include "symtab.h"
#include "token.h"
#include "yyerror.h"

#define DEFINED 1

extern void scanner_use_file(char*);
extern int yylex(void);

extern int yydebug;
extern int optind;

const char* token_name(int token_class);

Scope* record_types = new Scope("record");
ast_t* syntax_tree;
%}

%error-verbose

%union {
	token_t* token;
	ast_t* node;
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
%type<node> declarationList
%type<node> declaration
%type<node> recDeclaration
%type<node> varDeclaration
%type<node> scopedVarDeclaration
%type<node> varDeclList
%type<node> varDeclInitialize
%type<node> varDeclId
%type<node> scopedTypeSpecifier
%type<node> typeSpecifier
%type<node> returnTypeSpecifier
%type<node> funDeclaration
%type<node> params
%type<node> paramList
%type<node> paramTypeList
%type<node> paramIdList
%type<node> paramId
%type<node> statement
%type<node> matchedStmt
%type<node> unmatchedStmt
%type<node> otherStmt
%type<node> compoundStmt
%type<node> localDeclarations
%type<node> statementList
%type<node> expressionStmt
%type<node> returnStmt
%type<node> breakStmt
%type<node> expression
%type<node> assop
%type<node> simpleExpression
%type<node> andExpression
%type<node> unaryRelExpression
%type<node> relExpression
%type<node> relop
%type<node> sumExpression
%type<node> sumop
%type<node> term
%type<node> mulop
%type<node> unaryExpression
%type<node> unaryop
%type<node> factor
%type<node> mutable
%type<node> immutable
%type<node> call
%type<node> args
%type<node> argList
%type<node> constant

%token-table

%start program

%%

program					: declarationList {
							syntax_tree = $1;
						}
						;

declarationList			: declarationList declaration {
							$$ = $1;
							ast_add_sibling($$, $2);
						}
						| declaration {
							$$ = $1;
						}
						;

declaration				: varDeclaration {
							$$ = $1;
						}
						| funDeclaration {
							$$ = $1;
						}
						| recDeclaration {
							$$ = $1;
						}
						| error {
							$$ = ast_create_node();
						}
						;

recDeclaration			: RECORD ID '{' localDeclarations '}' {
							record_types->insert(
								std::string($2->value.str_val),
								(void*) DEFINED);

							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_RECORD;
							$$->data.name = strdup($2->input);
							ast_add_child($$, 0, $4);
						}
						;

varDeclaration			: typeSpecifier varDeclList ';' {
							ast_t* node;
							ast_t* decl;

							$$ = NULL;
							node = $2;


							while (node != NULL) {
								decl = ast_create_node();
								decl->lineno = node->lineno;
								decl->type = NODE_VAR;
								if (node->data.name) {
									decl->data.name = strdup(node->data.name);
								}
								decl->data.type = $1->data.type;
								decl->data.is_array = node->data.is_array;
								decl->data.int_val = node->data.int_val;

								if (node->child[0]) {
									ast_add_child(decl, 0, node->child[0]);
								}

								if ($$ == NULL) {
									$$ = decl;
								} else {
									ast_add_sibling($$, decl);
								}

								node = node->sibling;
							}

							yyerrok;
						}
						| error varDeclList ';' {
							$$ = ast_create_node();
						}
						| typeSpecifier error ';' {
							$$ = ast_create_node();
							yyerrok;
						}
						;

scopedVarDeclaration	: scopedTypeSpecifier varDeclList ';' {
							ast_t* node;
							ast_t* decl;

							$$ = NULL;
							node = $2;

							while (node != NULL) {
								decl = ast_create_node();
								decl->lineno = node->lineno;
								if (node->data.name) {
									decl->data.name = strdup(node->data.name);
								}
								decl->type = NODE_VAR;
								decl->data.type = $1->data.type;
								decl->data.is_array = node->data.is_array;
								decl->data.int_val = node->data.int_val;
								decl->data.is_static = $1->data.is_static;

								if (node->child[0]) {
									ast_add_child(decl, 0, node->child[0]);
								}

								if ($$ == NULL) {
									$$ = decl;
								} else {
									ast_add_sibling($$, decl);
								}

								node = node->sibling;
							}

							yyerrok;
						}
						| error varDeclList ';' {
							$$ = ast_create_node();
							yyerrok;
						}
						| scopedTypeSpecifier error ';' {
							$$ = ast_create_node();
							yyerrok;
						}
						;

varDeclList				: varDeclList ',' varDeclInitialize {
							$$ = $1;
							ast_add_sibling($$, $3);
							yyerrok;
						}
						| varDeclList ',' error {
							$$ = $1;
						}
						| varDeclInitialize {
							$$ = $1;
						}
						| error {
							$$ = ast_create_node();
						}
						;

varDeclInitialize		: varDeclId {
							$$ = $1;
						}
						| varDeclId ':' simpleExpression {
							$$ = $1;
							ast_add_child($$, 0, $3);
						}
						| error ':' simpleExpression {
							$$ = ast_create_node();
							yyerrok;
						}
						| varDeclId ':' error {
							$$ = ast_create_node();
						}
						;

varDeclId				: ID {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_ID;
							$$->data.name = strdup($1->input);
						}
						| ID '[' NUMCONST ']' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_ID;
							$$->data.name = strdup($1->input);
							$$->data.is_array = 1;
							$$->data.int_val = $3->value.int_val;
						}
						| ID '[' error {
							$$ = ast_create_node();
						}
						| error ']' {
							$$ = ast_create_node();
							yyerrok;
						}
						;

scopedTypeSpecifier		: STATIC typeSpecifier {
							$$ = $2;
							$$->data.is_static = 1;
						}
						| typeSpecifier {
							$$ = $1;
						}
						;

typeSpecifier			: returnTypeSpecifier {
							$$ = $1;
						}
						| RECTYPE {
							$$ = ast_from_token($1);
							$$->data.type = TYPE_RECORD;
						}
						;

returnTypeSpecifier		: INT {
							$$ = ast_from_token($1);
							$$->data.type = TYPE_INT;
						}
						| BOOL {
							$$ = ast_from_token($1);
							$$->data.type = TYPE_BOOL;
						}
						| CHAR {
							$$ = ast_from_token($1);
							$$->data.type = TYPE_CHAR;
						}
						;

funDeclaration			: typeSpecifier ID '(' params ')' statement {
							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_FUNC;

							switch ($1->data.token_class) {
								case BOOL:
									$$->data.type = TYPE_BOOL;
									break;
								case CHAR:
									$$->data.type = TYPE_CHAR;
									break;
								case INT:
									$$->data.type = TYPE_INT;
									break;
								case RECTYPE:
									$$->data.type = TYPE_RECORD;
									break;
							}

							$$->data.name = strdup($2->value.str_val);

							ast_add_child($$, 0, $4);
							ast_add_child($$, 1, $6);
						}
						| typeSpecifier error {
							$$ = ast_create_node();
						}
						| typeSpecifier ID '(' error {
							$$ = ast_create_node();
						}
						| typeSpecifier ID '(' params ')' error {
							$$ = ast_create_node();
						}
						| ID '(' params ')' statement {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_FUNC;
							$$->data.type = TYPE_VOID;
							$$->data.name = strdup($1->value.str_val);
							ast_add_child($$, 0, $3);
							ast_add_child($$, 1, $5);
						}
						| ID '(' error {
							$$ = ast_create_node();
						}
						| ID '(' params ')' error {
							$$ = ast_create_node();
						}
						;

params					: paramList {
							$$ = $1;
						}
						| %empty {
							$$ = NULL;
						}
						;

paramList				: paramList ';' paramTypeList {
							$$ = $1;
							ast_add_sibling($$, $3);
							yyerrok;
						}
						| paramList ';' error {
							$$ = $1;
						}
						| paramTypeList {
							$$ = $1;
						}
						| error {
							$$ = ast_create_node();
						}
						;

paramTypeList			: typeSpecifier paramIdList {
							ast_t* node;
							ast_t* decl;

							$$ = NULL;
							node = $2;

							while (node != NULL) {
								decl = ast_create_node();
								decl->lineno = node->lineno;
								decl->type = NODE_PARAM;
								if (node->data.name) {
									decl->data.name = strdup(node->data.name);
								}
								decl->data.type = $1->data.type;
								decl->data.is_array = node->data.is_array;

								if (node->child[0]) {
									ast_add_child(decl, 0, node->child[0]);
								}

								if ($$ == NULL) {
									$$ = decl;
								} else {
									ast_add_sibling($$, decl);
								}

								node = node->sibling;
							}
						}
						| typeSpecifier error {
							$$ = ast_create_node();
						}
						;

paramIdList				: paramIdList ',' paramId {
							$$ = $1;
							ast_add_sibling($$, $3);
							yyerrok;
						}
						| paramIdList ',' error {
							$$ = $1;
						}
						| paramId {
							$$ = $1;
						}
						| error {
							$$ = ast_create_node();
						}
						;

paramId					: ID {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_ID;
							$$->data.name = strdup($1->input);
						}
						| ID '[' ']' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_ID;
							$$->data.name = strdup($1->input);
							$$->data.is_array = 1;
						}
						| error ']' {
							$$ = ast_create_node();
							yyerrok;
						}
						;

statement				: matchedStmt {
							$$ = $1;
						}
						| unmatchedStmt {
							$$ = $1;
						}
						;

matchedStmt				: IF '(' simpleExpression ')' matchedStmt ELSE matchedStmt {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_IF;
							ast_add_child($$, 0, $3);
							ast_add_child($$, 1, $5);
							ast_add_child($$, 2, $7);
						}
						| IF '(' error {
							$$ = ast_create_node();
						}
						| IF error ')' matchedStmt ELSE matchedStmt {
							$$ = ast_create_node();
							yyerrok;
						}
						| WHILE '(' simpleExpression ')' matchedStmt {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_WHILE;
							ast_add_child($$, 0, $3);
							ast_add_child($$, 1, $5);
						}
						| WHILE error ')' matchedStmt {
							$$ = ast_create_node();
							yyerrok;
						}
						| WHILE '(' error ')' matchedStmt {
							$$ = ast_create_node();
							yyerrok;
						}
						| WHILE error {
							$$ = ast_create_node();
						}
						| error {
							$$ = ast_create_node();
						}
						| otherStmt {
							$$ = $1;
						}
						;

unmatchedStmt			: IF '(' simpleExpression ')' matchedStmt {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_IF;
							ast_add_child($$, 0, $3);
							ast_add_child($$, 1, $5);
						}
						| IF error {
							$$ = ast_create_node();
						}
						| IF '(' simpleExpression ')' unmatchedStmt {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_IF;
							ast_add_child($$, 0, $3);
							ast_add_child($$, 1, $5);
						}
						| IF error ')' statement {
							$$ = ast_create_node();
							yyerrok;
						}
						| IF '(' simpleExpression ')' matchedStmt ELSE unmatchedStmt {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_IF;
							ast_add_child($$, 0, $3);
							ast_add_child($$, 1, $5);
							ast_add_child($$, 2, $7);
						}
						| IF error ')' matchedStmt ELSE unmatchedStmt {
							$$ = ast_create_node();
							yyerrok;
						}
						| WHILE '(' simpleExpression ')' unmatchedStmt {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_WHILE;
							ast_add_child($$, 0, $3);
							ast_add_child($$, 1, $5);
						}
						| WHILE error ')' unmatchedStmt {
							$$ = ast_create_node();
							yyerrok;
						}
						| WHILE '(' error ')' unmatchedStmt {
							$$ = ast_create_node();
							yyerrok;
						}
						;

otherStmt				: expressionStmt {
							$$ = $1;
						}
						| compoundStmt {
							$$ = $1;
						}
						| returnStmt {
							$$ = $1;
						}
						| breakStmt {
							$$ = $1;
						}
						;

compoundStmt			: '{' localDeclarations statementList '}' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_COMPOUND;
							$$->data.type = TYPE_VOID;
							ast_add_child($$, 0, $2);
							ast_add_child($$, 1, $3);
							yyerrok;
						}
						| '{' error statementList '}' {
							$$ = ast_create_node();
							yyerrok;
						}
						| '{' localDeclarations error '}' {
							$$ = ast_create_node();
							yyerrok;
						}
						;

localDeclarations		: localDeclarations scopedVarDeclaration {
							if ($1 == NULL) {
								$$ = $2;
							} else {
								$$ = $1;
								ast_add_sibling($$, $2);
							}
						}
						| %empty {
							$$ = NULL;
						}
						;

statementList			: statementList statement {
							if ($1 == NULL && $2 == NULL) {
								$$ = NULL;
							} else if ($1 == NULL) {
								$$ = $2;
							} else if ($2 == NULL) {
								$$ = $1;
							} else {
								/* This call to ast_add_siblings segfaults if the RHS
								 * statement had a syntax error. Somewhere a sibling pointer
								 * is set to a value which is out of bounds. I can't bloody
								 * find the error, so instead we're not going to even try
								 * to build the tree if an error occured. This is WRONG, but
								 * since nobody tries to traverse the tree after a parser 
								 * error nobody should notice.
								 */
								if (!errors) {
									$$ = $1;
									ast_add_sibling($1, $2);
								}
							}
						}
						| %empty {
							$$ = NULL;
						}
						;

expressionStmt			: expression ';' {
							$$ = $1;
							yyerrok;
						}
						| ';' {
							$$ = NULL;
							yyerrok;
						}
						;

returnStmt				: RETURN ';' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_RETURN;
							$$->data.type = TYPE_VOID;
							yyerrok;
						}
						| RETURN expression ';' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_RETURN;
							$$->data.type = TYPE_VOID;
							ast_add_child($$, 0, $2);
							yyerrok;
						}
						;

breakStmt				: BREAK ';' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_BREAK;
							yyerrok;
						}
						;

expression				: mutable assop expression {
							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_ASSIGN;
							$$->data.name = $2->data.name;
							$$->data.op = $2->data.op;
							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, $3);
						}
						| error assop error {
							$$ = ast_create_node();
						}
						| mutable INC {
							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_ASSIGN;
							$$->data.name = strdup($2->input);
							$$->data.op = OP_INC,
							ast_add_child($$, 0, $1);
							yyerrok;
						}
						| error INC {
							$$ = ast_create_node();
							yyerrok;
						}
						| mutable DEC {
							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_ASSIGN;
							$$->data.name = strdup($2->input);
							$$->data.op = OP_DEC,
							ast_add_child($$, 0, $1);
							yyerrok;
						}
						| error DEC {
							$$ = ast_create_node();
							yyerrok;
						}
						| simpleExpression {
							$$ = $1;
						}
						;

assop					: '=' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->data.op = OP_ASS;
							$$->data.name = $1->input;
						}
						| ADDASS {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->data.op = OP_ADDASS;
							$$->data.name = $1->input;
						}
						| DIVASS {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->data.op = OP_DIVASS;
							$$->data.name = $1->input;
						}
						| MULASS {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->data.op = OP_MULASS;
							$$->data.name = $1->input;
						}
						| SUBASS {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->data.op = OP_SUBASS;
							$$->data.name = $1->input;
						}
						;

simpleExpression		: simpleExpression OR andExpression {
							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($2->input);
							$$->data.op = OP_OR,
							$$->data.is_const =
								$1->data.is_const && $3->data.is_const;
							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, $3);
						}
						| simpleExpression OR error {
							$$ = ast_create_node();
						}
						| andExpression {
							$$ = $1;
						}
						;

andExpression			: andExpression AND unaryRelExpression {
							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($2->input);
							$$->data.op = OP_AND;
							$$->data.is_const =
								$1->data.is_const && $3->data.is_const;
							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, $3);
						}
						| andExpression AND error {
							$$ = ast_create_node();
						}
						| unaryRelExpression {
							$$ = $1;
						}
						;

unaryRelExpression		: NOT unaryRelExpression {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_NOT;
							$$->data.is_const = $2->data.is_const;
							ast_add_child($$, 0, $2);
						}
						| NOT error {
							$$ = ast_create_node();
						}
						| relExpression {
							$$ = $1;
						}
						;

relExpression			: sumExpression relop sumExpression {
							$$ = $2;
							$$->data.is_const =
								$1->data.is_const && $3->data.is_const;
							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, $3);
						}
						| sumExpression relop error {
							$$ = $2;
						}
						| error relop sumExpression {
							$$ = $2;
							yyerrok;
						}
						| sumExpression {
							$$ = $1;
						}
						;

relop					: LESSEQ {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_LESSEQ;
						}
						| '<' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_LESS;
						}
						| '>' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_GRT;
						}
						| GRTEQ {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_GRTEQ;
						}
						| EQ {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_EQ;
						}
						| NOTEQ {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_NOTEQ;
						}
						;

sumExpression			: sumExpression sumop term {
							$$ = $2;
							$$->data.is_const =
								$1->data.is_const && $3->data.is_const;
							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, $3);
						}
						| sumExpression sumop error {
							$$ = $2;
							yyerrok;
						}
						| term {
							$$ = $1;
						}
						;

sumop					: '+' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_ADD;
						}
						| '-' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_SUB;
						}
						;

term					: term mulop unaryExpression {
							$$ = $2;
							$$->data.is_const =
								$1->data.is_const && $3->data.is_const;
							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, $3);
						}
						| term mulop error {
							$$ = $2;
						}
						| unaryExpression {
							$$ = $1;
						}
						;

mulop					: '*' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_MUL;
						}
						| '/' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_DIV;
						}
						| '%' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_MOD;
						}
						;

unaryExpression			: unaryop unaryExpression {
							$$ = $1;
							$$->data.is_const = $2->data.is_const;
							ast_add_child($$, 0, $2);
						}
						| unaryop error {
							$$ = $1;
						}
						| factor {
							$$ = $1;
						}
						;

unaryop					: '-' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_NEG;
						}
						| '*' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_SIZE;
						}
						| '?' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($1->input);
							$$->data.op = OP_QMARK;
						}
						;

factor					: immutable {
							$$ = $1;
						}
						| mutable {
							$$ = $1;
						}
						;

mutable					: ID {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_ID;
							$$->data.name = strdup($1->input);
						}
						| mutable '[' expression ']' {
							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($2->input);
							$$->data.op = OP_SUBSC;
							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, $3);
						}
						| mutable '.' ID {
							ast_t* id;

							$$ = ast_create_node();
							$$->lineno = $2->lineno;
							$$->type = NODE_OP;
							$$->data.name = strdup($2->input);
							$$->data.op = OP_DOT;

							id = ast_create_node();
							id->lineno = $3->lineno;
							id->type = NODE_ID;
							id->data.name = strdup($3->input);

							ast_add_child($$, 0, $1);
							ast_add_child($$, 1, id);
						}
						;

immutable				: '(' expression ')' {
							$$ = $2;
							$$->data.is_const = $2->data.is_const;
							yyerrok;
						}
						| '(' error {
							$$ = NULL;
						}
						| error ')' {
							yyerrok;
						}
						| call {
							$$ = $1;
						}
						| constant {
							$$ = $1;
						}
						;

call					: ID '(' args ')' {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_CALL;
							$$->data.name = strdup($1->value.str_val);
							ast_add_child($$, 0, $3);
						}
						| error '(' {
							yyerrok;
						}
						;

args					: argList {
							$$ = $1;
						}
						| %empty {
							$$ = NULL;
						}
						;

argList					: argList ',' expression {
							ast_add_sibling($$, $3);
							yyerrok;
						}
						| argList ',' error
						| expression {
							$$ = $1;
						}
						;

constant				: NUMCONST {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_CONST;
							$$->data.type = TYPE_INT;
							$$->data.int_val = $1->value.int_val;
							$$->data.is_const = 1;
						}
						| CHARCONST {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_CONST;
							$$->data.type = TYPE_CHAR;
							$$->data.char_val = $1->value.char_val;
							$$->data.is_const = 1;
						}
						| BOOLCONST {
							$$ = ast_create_node();
							$$->lineno = $1->lineno;
							$$->type = NODE_CONST;
							$$->data.type = TYPE_BOOL;
							$$->data.bool_val = $1->value.int_val;
							$$->data.is_const = 1;
						}
						;

%%

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
