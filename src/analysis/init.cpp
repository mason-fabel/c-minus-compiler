#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "../symtab.h"
#include "error.h"
#include "id.h"

int init_only_const(ast_t* node) {
	int pass;

	if (!node->child[0]) return 1;

	pass = node->child[0]->data.is_const;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"Initializer for variable '%s' is not a constant expression.\n",
			node->data.name);
	}

	return pass;
}

int init_match_type(ast_t* node) {
	int pass;
	ast_type_t init;
	ast_type_t var;

	var = node->data.type;
	init = node->child[0] ? node->child[0]->data.type : TYPE_NONE;

	pass = var == init || init == TYPE_NONE;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Variable '%s' is of %s but is being initialized ",
			node->data.name, ast_type_string(var));
		fprintf(stdout, "with an expression of %s.\n", ast_type_string(init));
	}

	return pass;
}
