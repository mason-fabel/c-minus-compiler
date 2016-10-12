#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "token.h"
#include "parser.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

extern const char* token_name(int token_class);

ast_t* ast_create_node() {
	int i;
	ast_t* node;

	node = (ast_t*) malloc(sizeof(ast_t));
	assert(node != NULL);

	node->lineno = 0;

	node->type = TYPE_NONE;
	node->data.name = NULL;
	node->data.type = NULL;
	node->data.token_class = -1;
	node->data.is_array = 0;
	node->data.int_val = 0;
	node->data.char_val = '\0';
	node->data.str_val = NULL;

	node->num_children = 0;
	for (i = 0; i < AST_MAX_CHILDREN; i++) {
		node->child[i] = NULL;
	}
	node->sibling = NULL;

	return node;
}

ast_t* ast_from_token(token_t* tok) {
	ast_t* node;

	node = ast_create_node();
	node->lineno = tok->lineno;

	node->data.name = strdup(token_name(tok->type));
	node->data.token_class = tok->type;

	switch (tok->value_mode) {
		case MODE_CHAR:
			node->type = TYPE_TOKEN_CHAR;
			node->data.char_val = tok->value.char_val;
			break;
		case MODE_INT:
			node->type = TYPE_TOKEN_INT;
			node->data.int_val = tok->value.int_val;
			break;
		case MODE_STR:
			node->type = TYPE_TOKEN_STR;
			node->data.str_val = strdup(tok->value.str_val);
			break;
		case MODE_NONE:
			node->type = TYPE_TOKEN_NONE;
			break;
		default:
			fprintf(stderr, "ast_from_token: unknown token: %s\n", tok->input);
			exit(1);
	}


	return node;
}

void ast_add_sibling(ast_t* root, ast_t* sibling) {
	while (root->sibling != NULL) {
		root = root->sibling;
	}

	root->sibling = sibling;

	return;
}

void ast_add_child(ast_t* root, int index, ast_t* child) {
	assert(index >= 0);
	assert(index < AST_MAX_CHILDREN);

	root->num_children = MAX(root->num_children, index + 1);
	root->child[index] = child;

	return;
}
