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

	if (opts.aug) fprintf(stdout, " [type %s]", node->data.type);

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
		case TYPE_ASSIGN:
			fprintf(stdout, "Assign: %s", node->data.name);
			break;
		case TYPE_BREAK:
			fprintf(stdout, "Break");
			break;
		case TYPE_CALL:
			fprintf(stdout, "Call: %s", node->data.name);
			break;
		case TYPE_COMPOUND:
			fprintf(stdout, "Compound");
			break;
		case TYPE_CONST_BOOL:
			fprintf(stdout, "Const: %s",
				node->data.bool_val ? "true" : "false");
			break;
		case TYPE_CONST_CHAR:
			fprintf(stdout, "Const: '%c'", node->data.char_val);
			break;
		case TYPE_CONST_INT:
			fprintf(stdout, "Const: %i", node->data.int_val);
			break;
		case TYPE_FUNC_BOOL:
			fprintf(stdout, "Func %s returns type bool", node->data.name);
			break;
		case TYPE_FUNC_CHAR:
			fprintf(stdout, "Func %s returns type char", node->data.name);
			break;
		case TYPE_FUNC_INT:
			fprintf(stdout, "Func %s returns type int", node->data.name);
			break;
		case TYPE_FUNC_REC:
			fprintf(stdout, "Func %s returns type record", node->data.name);
			break;
		case TYPE_FUNC_VOID:
			fprintf(stdout, "Func %s returns type void", node->data.name);
			break;
		case TYPE_ID:
			fprintf(stdout, "Id: %s", node->data.name);
			break;
		case TYPE_IF:
			fprintf(stdout, "If");
			break;
		case TYPE_NONE:
			break;
		case TYPE_OP:
			fprintf(stdout, "Op: %s", node->data.name);
			break;
		case TYPE_PARAM_BOOL:
			fprintf(stdout, "Param %s of type bool", node->data.name);
			break;
		case TYPE_PARAM_CHAR:
			fprintf(stdout, "Param %s of type char", node->data.name);
			break;
		case TYPE_PARAM_INT:
			fprintf(stdout, "Param %s of type int", node->data.name);
			break;
		case TYPE_PARAM_REC:
			fprintf(stdout, "Param %s of type record", node->data.name);
			break;
		case TYPE_PARAM_BOOL_ARRAY:
			fprintf(stdout, "Param %s is array of type bool", node->data.name);
			break;
		case TYPE_PARAM_CHAR_ARRAY:
			fprintf(stdout, "Param %s is array of type char", node->data.name);
			break;
		case TYPE_PARAM_INT_ARRAY:
			fprintf(stdout, "Param %s is array of type int", node->data.name);
			break;
		case TYPE_PARAM_REC_ARRAY:
			fprintf(stdout, "Param %s is array of type record",
				node->data.name);
			break;
		case TYPE_RECORD:
			fprintf(stdout, "Record %s ", node->data.name);
			break;
		case TYPE_RETURN:
			fprintf(stdout, "Return");
			break;
		case TYPE_TOKEN_CHAR:
			fprintf(stdout, "Token %s of value %c",
				node->data.name, node->data.char_val);
			break;
		case TYPE_TOKEN_INT:
			fprintf(stdout, "Token %s of value %i",
				node->data.name, node->data.int_val);
			break;
		case TYPE_TOKEN_NONE:
			fprintf(stdout, "Token %s", node->data.name);
			break;
		case TYPE_TOKEN_STR:
			fprintf(stdout, "Token %s of value %s",
				node->data.name, node->data.str_val);
			break;
		case TYPE_VAR_BOOL:
			fprintf(stdout, "Var %s of type bool", node->data.name);
			break;
		case TYPE_VAR_CHAR:
			fprintf(stdout, "Var %s of type char", node->data.name);
			break;
		case TYPE_VAR_INT:
			fprintf(stdout, "Var %s of type int", node->data.name);
			break;
		case TYPE_VAR_REC:
			fprintf(stdout, "Var %s of type record", node->data.name);
			break;
		case TYPE_VAR_BOOL_ARRAY:
			fprintf(stdout, "Var %s is array of type bool", node->data.name);
			break;
		case TYPE_VAR_CHAR_ARRAY:
			fprintf(stdout, "Var %s is array of type char", node->data.name);
			break;
		case TYPE_VAR_INT_ARRAY:
			fprintf(stdout, "Var %s is array of type int", node->data.name);
			break;
		case TYPE_VAR_REC_ARRAY:
			fprintf(stdout, "Var %s is array of type record", node->data.name);
		case TYPE_WHILE:
			fprintf(stdout, "While");
			break;
		default:
			fprintf(stdout, "unknown node %i", node->type);
	}

	return;
}
