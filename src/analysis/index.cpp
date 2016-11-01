#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "../symtab.h"
#include "error.h"
#include "index.h"

extern SymbolTable sem_symtab;

int index_no_array(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[1];
	if (!arg) return 0;

	pass = !arg->data.is_array;

	if (!pass) {
		error_lineno(node);

		if (arg->type == NODE_ID) {
			fprintf(stdout, "Array index is the unindexed array '%s'.\n",
				arg->data.name);
		} else {
			fprintf(stdout, "Array index is an unindexed array.\n");
		}
	}

	return pass;
}

int index_only_array(ast_t* node) {
	int pass;
	ast_t* arr;

	arr = node->child[0];
	if (!arr) return 0;

	pass = arr->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Cannot index nonarray");
		if (arr->type == NODE_ID) {
			fprintf(stdout, " '%s'", arr->data.name);
		}
		fprintf(stdout, ".\n");
	}

	return pass;
}

int index_only_int(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[1];
	if (!arg) return 0;

	pass = arg->data.type == TYPE_INT || arg->data.type == TYPE_NONE;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Array '%s' should be indexed by ",
			(node->child[0])->data.name);
		fprintf(stdout, "type int but got %s.\n",
			ast_type_string(arg->data.type));
	}

	return pass;
}
