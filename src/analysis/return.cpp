#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "../symtab.h"
#include "error.h"
#include "return.h"

extern SymbolTable sem_symtab;

int return_match_type(ast_t* node, ast_t* def) {
	int pass;
	ast_type_t ret;

	if (!node) return 0;

	if (def->data.type == TYPE_VOID) {
		pass = !(node->child[0]);

		if (!pass) {
			error_lineno(node);

			fprintf(stdout, "Function '%s' at line %i ",
				def->data.name, def->lineno);
			fprintf(stdout, "is expecting no return value, ");
			fprintf(stdout, "but return has return value.\n");
		}
	} else {
		ret = node->child[0] ? node->child[0]->data.type : TYPE_VOID;
		pass = ret == def->data.type || ret == TYPE_NONE;

		if (!pass) {
			error_lineno(node);

			fprintf(stdout, "Function '%s' at line %i ",
				def->data.name, def->lineno);
			fprintf(stdout, "is expecting to return %s but ",
				ast_type_string(def->data.type));

			if (node->child[0]) {
				fprintf(stdout, "instead returns %s.\n",
					ast_type_string(ret));
			} else {
				fprintf(stdout, "return has no return value.\n");
			}
		}
	}

	return pass;
}

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
