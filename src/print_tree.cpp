#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "print_tree.h"

void _ast_print(ast_t* node, int level, int sibling_num, int child_num);
void _ast_print_data(ast_t* node);

typedef struct {
	int aug;
} print_options_t;

static print_options_t opts;

void ast_print(ast_t* tree, int aug) {
	opts.aug = aug;

	_ast_print(tree, 0, -1, -1);

	return;
}

void _ast_print(ast_t* node, int level, int sibling_num, int child_num) {
	int i;

	if (node == NULL) return;

	for (i = 0; i < level; i++) {
		fprintf(stdout, "!   ");
	}

	if (sibling_num > -1) {
		fprintf(stdout, "Sibling: %i  ", sibling_num);
	}

	if (child_num > -1) {
		fprintf(stdout, "Child: %i  ", child_num);
	}

	_ast_print_data(node);

	if (opts.aug) {
		switch (node->type) {
			case NODE_FUNC:
				fprintf(stdout, " [ref: %s, size: %i, loc: %i]",
					ast_scope_string(node->data.mem.scope),
					-1 * node->data.mem.size, -1 * node->data.mem.loc);
				break;
			case NODE_CALL:
			case NODE_ID:
			case NODE_PARAM:
			case NODE_VAR:
				fprintf(stdout, " [ref: %s, size: %i, loc: %i]",
					ast_scope_string(node->data.mem.scope),
					node->data.mem.size, -1 * node->data.mem.loc);
				break;
		}

		fprintf(stdout, " [%s]", ast_type_string(node->data.type));
	}

	fprintf(stdout, " [line: %i]", node->lineno);
	fprintf(stdout, "\n");

	for (i = 0; i < node->num_children; i++) {
		_ast_print(node->child[i], level + 1, -1, i);
	}

	_ast_print(node->sibling, level, sibling_num + 1, -1);

	return;
}

void _ast_print_data(ast_t* node) {
	switch (node->type) {
		case NODE_ASSIGN:
			fprintf(stdout, "Assign: %s", node->data.name);
			break;
		case NODE_BREAK:
			fprintf(stdout, "Break");
			break;
		case NODE_CALL:
			fprintf(stdout, "Call: %s", node->data.name);
			break;
		case NODE_COMPOUND:
			fprintf(stdout, "Compound");
			break;
		case NODE_CONST:
			fprintf(stdout, "Const: ");
			switch (node->data.type) {
				case TYPE_BOOL:
					fprintf(stdout, "%s",
						node->data.bool_val ? "true" : "false");
					break;
				case TYPE_CHAR:
					fprintf(stdout, "'%c'", node->data.char_val);
					break;
				case TYPE_INT:
					fprintf(stdout, "%i", node->data.int_val);
					break;
			}
			break;
		case NODE_FUNC:
			fprintf(stdout, "Func %s returns %s", node->data.name,
				ast_type_string(node->data.type));
			break;
		case NODE_ID:
			fprintf(stdout, "Id: %s ", node->data.name);
			if (node->data.is_array) fprintf(stdout, "is array ");
			break;
		case NODE_IF:
			fprintf(stdout, "If");
			break;
		case NODE_NONE:
			break;
		case NODE_OP:
			fprintf(stdout, "Op: %s", node->data.name);
			break;
		case NODE_PARAM:
			fprintf(stdout, "Param %s ", node->data.name);
			if (node->data.is_array) fprintf(stdout, "is array ");
			break;
		case NODE_RECORD:
			fprintf(stdout, "Record %s ", node->data.name);
			break;
		case NODE_RETURN:
			fprintf(stdout, "Return");
			break;
		case NODE_TOKEN:
			fprintf(stdout, "Token %s ", node->data.name);
			switch(node->data.type) {
				case TYPE_CHAR:
					fprintf(stdout, "of value %c", node->data.char_val);
					break;
				case TYPE_INT:
					fprintf(stdout, "of value %i", node->data.int_val);
					break;
				case TYPE_STR:
					fprintf(stdout, "of value \"%s\"", node->data.str_val);
					break;
			}
			break;
		case NODE_VAR:
			fprintf(stdout, "Var %s ", node->data.name);
			if (node->data.is_array) fprintf(stdout, "is array ");
			break;
		case NODE_WHILE:
			fprintf(stdout, "While");
			break;
		default:
			fprintf(stdout, "unknown node %i", node->type);
	}

	return;
}
