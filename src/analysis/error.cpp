#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "../ast.h"
#include "../symtab.h"
#include "error.h"

extern int errors;
extern int warnings;
extern SymbolTable sem_symtab;

void error_lineno(ast_t* node) {
	errors++;

	fprintf(stdout, "ERROR(%i): ", node->lineno);

	return;
}

void error_func_defined(ast_t* node) {
	error_lineno(node);
	fprintf(stdout, "Function '%s' is not defined.\n", node->data.name);

	return;
}

void error_symbol_defined(ast_t* node) {
	ast_t* def;

	def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
	if (!def) return;

	error_lineno(node);
	fprintf(stdout, "Symbol '%s' is already defined at line %i.\n",
		node->data.name, def->lineno);

	return;
}

void warning_lineno(ast_t* node) {
	warnings++;

	fprintf(stdout, "WARNING(%i): ", node->lineno);

	return;
}
