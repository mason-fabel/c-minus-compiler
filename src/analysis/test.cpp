#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "test.h"
#include "error.h"

int test_if_no_array(ast_t* node) {
	int pass;

	pass = !((node->child[0])->data.is_array);

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"Cannot use array as test condition in if statement.\n");
	}

	return pass;
}

int test_if_only_bool(ast_t* node) {
	int pass;
	ast_t* test;

	test = node->child[0];

	pass = test->data.type == TYPE_BOOL;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Expecting Boolean test condition in if statement ");
		fprintf(stdout, "but got %s.\n", ast_type_string(test->data.type));
	}

	return pass;
}

int test_while_no_array(ast_t* node) {
	int pass;

	pass = !((node->child[0])->data.is_array);

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"Cannot use array as test condition in while statement.\n");
	}

	return pass;
}

int test_while_only_bool(ast_t* node) {
	int pass;
	ast_t* test;

	test = node->child[0];

	pass = test->data.type == TYPE_BOOL;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Expecting Boolean test condition in while statement ");
		fprintf(stdout, "but got %s.\n", ast_type_string(test->data.type));
	}

	return pass;
}
