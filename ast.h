#ifndef _AST_H_
#define _AST_H_

#include "token.h"

#define AST_MAX_CHILDREN 3

struct _ast {
	int lineno;
	int num_children;
	value_mode_t value_mode;
	value_t value;
	struct _ast* sibling;
	struct _ast* child[AST_MAX_CHILDREN];
};
typedef struct _ast ast_t;

void ast_print(ast_t* tree);
void ast_add_sibling(ast_t* root, ast_t* sibling);
void ast_add_child(ast_t* root, int index, ast_t* child);
ast_t* ast_create_node();
ast_t* ast_from_token(token_t* tok);

#endif /* _AST_H_ */
