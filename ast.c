#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "token.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void _ast_print(ast_t* node, int level, int sibling_num, int child_num);

ast_t* ast_create_node() {
	int i;
	ast_t* node;

	node = (ast_t*) malloc(sizeof(ast_t));
	assert(node != NULL);

	node->lineno = 0;
	node->num_children = 0;
	node->value_mode = MODE_NONE;
	node->sibling = NULL;
	for (i = 0; i < AST_MAX_CHILDREN; i++) {
		node->child[i] = NULL;
	}

	return node;
}

ast_t* ast_from_token(token_t* tok) {
	ast_t* node;

	node = ast_create_node();
	node->lineno = tok->lineno;
	node->value_mode = tok->value_mode;

	switch (node->value_mode) {
		case MODE_INT:
			node->value.int_val = tok->value.int_val;
			break;
		case MODE_CHAR:
			node->value.char_val = tok->value.char_val;
			break;
		case MODE_STR:
			node->value.str_val = strdup(tok->value.str_val);
			break;
		case MODE_NONE:
			break;
		default:
			fprintf(stderr, "ast_from_token: unknown token: %s\n", tok->input);
			exit(1);
	}

	return node;
}

void ast_print(ast_t* tree) {
	_ast_print(tree, 0, -1, -1);

	return;
}

void ast_add_sibling(ast_t* root, ast_t* sibling) {
	while (root->sibling != NULL) {
		root = root->sibling;
	}

	root->sibling = sibling;
	sibling->sibling = NULL;

	return;
}

void ast_add_child(ast_t* root, int index, ast_t* child) {
	assert(index >= 0);
	assert(index < AST_MAX_CHILDREN);

	root->num_children = MAX(root->num_children, index + 1);
	root->child[index] = child;

	return;
}

void _ast_print(ast_t* node, int level, int sibling_num, int child_num) {
	int i;

	if (node == NULL) return;

	for (i = 0; i < level; i++) {
		fprintf(stdout, "!   ");
	}

	if (sibling_num > -1) {
		fprintf(stdout, "Sibling: %i ", sibling_num);
	}

	if (child_num > -1) {
		fprintf(stdout, "Child: %i ", child_num);
	}

	switch (node->value_mode) {
		case MODE_INT:
			fprintf(stdout, "Value: %i ", node->value.int_val);
			break;
		case MODE_CHAR:
			fprintf(stdout, "Value: %c ", node->value.char_val);
			break;
		case MODE_STR:
			fprintf(stdout, "Value: %s ", node->value.str_val);
			break;
		case MODE_NONE:
			fprintf(stdout, "No value ");
			break;
		default:
			fprintf(stdout, "Invalid value mode ");
	}

	fprintf(stdout, "[line: %i]", node->lineno);
	fprintf(stdout, "\n");

	_ast_print(node->sibling, level, sibling_num + 1, -1);

	for (i = 0; i < node->num_children; i++) {
		_ast_print(node->child[i], level + 1, -1, i);
	}

	return;
}
