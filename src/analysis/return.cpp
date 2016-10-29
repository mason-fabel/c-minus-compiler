#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "../symtab.h"
#include "error.h"
#include "return.h"

extern SymbolTable sem_symtab;

int return_no_array(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[0];

	if (!arg) return 1;

	pass = !arg->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Cannot return an array.\n");
	}

	return pass;
}
