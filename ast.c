#include "stdlib.h"
#include "stdio.h"
#include "ast.h"

void ast_print(ast_t* tree) {
	fprintf(stdout, "tree: %i\n", (int) tree);

	return;
}
