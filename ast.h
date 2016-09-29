#ifndef _AST_H_
#define _AST_H_

#define AST_MAX_CHILDREN 3

#include "token.h"

typedef enum {
	TYPE_ASSIGN,
	TYPE_COMPOUND,
	TYPE_FUNC_BOOL,
	TYPE_FUNC_CHAR,
	TYPE_FUNC_INT,
	TYPE_IF,
	TYPE_NONE,
	TYPE_OP,
	TYPE_PARAM_BOOL,
	TYPE_PARAM_CHAR,
	TYPE_PARAM_INT,
	TYPE_PARAM_BOOL_ARRAY,
	TYPE_PARAM_CHAR_ARRAY,
	TYPE_PARAM_INT_ARRAY,
	TYPE_TOKEN_CHAR,
	TYPE_TOKEN_INT,
	TYPE_TOKEN_STR,
	TYPE_TOKEN_NONE,
	TYPE_VAR_BOOL,
	TYPE_VAR_CHAR,
	TYPE_VAR_INT,
	TYPE_VAR_BOOL_ARRAY,
	TYPE_VAR_CHAR_ARRAY,
	TYPE_VAR_INT_ARRAY,
	TYPE_WHILE,
} ast_type_t;

typedef struct {
	char* name;
	int token_class;
	int int_val;
	int bool_val;
	char char_val;
	char* str_val;
} ast_data_t;

struct _ast {
	int lineno;
	int num_children;
	ast_type_t type;
	ast_data_t data;
	struct _ast* child[AST_MAX_CHILDREN];
	struct _ast* sibling;
};
typedef struct _ast ast_t;

void ast_print(ast_t* tree);
void ast_add_sibling(ast_t* root, ast_t* sibling);
void ast_add_child(ast_t* root, int index, ast_t* child);
ast_t* ast_create_node();
ast_t* ast_from_token(token_t* tok);

#endif /* _AST_H_ */
