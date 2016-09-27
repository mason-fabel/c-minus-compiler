#ifndef _AST_H_
#define _AST_H_

#define AST_MAX_CHILDREN 3

struct _ast {
	struct _ast* child[AST_MAX_CHILDREN];
};
typedef struct _ast ast_t;

void ast_print(ast_t* tree);

#endif /* _AST_H_ */
