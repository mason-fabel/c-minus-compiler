#include <stdlib.h>
#include "ast.h"
#include "semantic.h"

void sem_analysis(ast_t* tree) {
	int i;

	if (tree == NULL) return;

	for (i = 0; i < tree->num_children; i++) {
		sem_analysis(tree->child[i]);
	}

	sem_analysis(tree->sibling);

	return;
}
