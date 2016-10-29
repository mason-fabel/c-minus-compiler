#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "../symtab.h"
#include "error.h"
#include "id.h"

extern SymbolTable sem_symtab;

int id_defined(ast_t* node) {
	int pass;
	ast_t* def;

	def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));

	pass = def != NULL;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Symbol '%s' is not defined.\n", node->data.name);
	}

	return pass;
}

int id_not_func(ast_t* node) {
	int pass;
	ast_t* def;

	def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
	if (!def) return 0;

	pass = def->type != NODE_FUNC;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Cannot use function '%s' as a variable.\n",
			node->data.name);
	}

	return pass;
}

int id_only_func(ast_t* node) {
	int pass;
	ast_t* def;

	def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
	if (!def) return 0;

	pass = def->type == NODE_FUNC;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "'%s' is a simple variable and cannot be called.\n",
			node->data.name);
	}

	return pass;
}
