#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "../symtab.h"
#include "error.h"
#include "unary.h"

extern SymbolTable sem_symtab;

int unary_no_array(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[0];

	pass = !arg->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "The operation '%s' does not work with arrays.\n",
			node->data.name);
	}

	return pass;
}

int unary_only_array(ast_t* node) {
	int pass;
	ast_t* arr;

	arr = node->child[0];
	if (!arr) return 0;

	pass = arr->data.type == TYPE_NONE || arr->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "The operation '%s' only works with arrays.\n",
			node->data.name);
	}

	return pass;
}

int unary_only_bool(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[0];
	if (!arg) return 0;

	pass = arg->data.type == TYPE_BOOL || arg->data.type == TYPE_NONE;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"Unary '%s' requires an operand of %s but was given %s.\n",
			node->data.name, ast_type_string(TYPE_BOOL),
			ast_type_string(arg->data.type));
	}

	return pass;
}

int unary_only_int(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[0];
	if (!arg) return 0;

	pass = arg->data.type == TYPE_INT || arg->data.type == TYPE_NONE;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"Unary '%s' requires an operand of %s but was given %s.\n",
			node->data.name, ast_type_string(TYPE_INT),
			ast_type_string(arg->data.type));
	}

	return pass;
}
