#ifndef _AST_H_
#define _AST_H_

#define AST_MAX_CHILDREN 3

#include "token.h"

typedef enum {
	NODE_ASSIGN,
	NODE_BREAK,
	NODE_CALL,
	NODE_COMPOUND,
	NODE_CONST,
	NODE_FUNC,
	NODE_ID,
	NODE_IF,
	NODE_NONE,
	NODE_OP,
	NODE_PARAM,
	NODE_RECORD,
	NODE_RETURN,
	NODE_TOKEN,
	NODE_VAR,
	NODE_WHILE,
} ast_node_t;

typedef enum {
	TYPE_BOOL,
	TYPE_CHAR,
	TYPE_INT,
	TYPE_NONE,
	TYPE_RECORD,
	TYPE_STR,
	TYPE_VOID,
} ast_type_t;

typedef struct {
	char* name;
	ast_type_t type;
	int token_class;
	int is_array;
	int bool_val;
	int int_val;
	char char_val;
	char* str_val;
} ast_data_t;

struct _ast {
	int lineno;
	int num_children;
	ast_node_t type;
	ast_data_t data;
	struct _ast* child[AST_MAX_CHILDREN];
	struct _ast* sibling;
};
typedef struct _ast ast_t;

void ast_add_sibling(ast_t* root, ast_t* sibling);
void ast_add_child(ast_t* root, int index, ast_t* child);
ast_t* ast_create_node();
ast_t* ast_from_token(token_t* tok);

#endif /* _AST_H_ */
