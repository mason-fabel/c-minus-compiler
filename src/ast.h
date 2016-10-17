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

typedef enum {
	OP_ADD,
	OP_ADDASS,
	OP_AND,
	OP_ASS,
	OP_DEC,
	OP_DIV,
	OP_DIVASS,
	OP_DOT,
	OP_EQ,
	OP_GRT,
	OP_GRTEQ,
	OP_INC,
	OP_LESS,
	OP_LESSEQ,
	OP_MOD,
	OP_MUL,
	OP_MULASS,
	OP_NONE,
	OP_NOT,
	OP_NOTEQ,
	OP_QMARK,
	OP_OR,
	OP_SIZE,
	OP_STAR,
	OP_SUB,
	OP_SUBASS,
	OP_SUBSC,
} ast_op_t;

typedef struct {
	char* name;
	ast_type_t type;
	ast_op_t op;
	int token_class;
	int is_array;
	int is_func_body;
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
const char* ast_type_string(ast_type_t type);

#endif /* _AST_H_ */
