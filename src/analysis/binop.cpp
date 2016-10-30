#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "binop.h"
#include "error.h"

int binop_match_array(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!(lhs && rhs)) return 0;

	pass = lhs->data.is_array == rhs->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"'%s' requires that either both or neither operands be arrays.\n",
			node->data.name);
	}

	return pass;
}

int binop_no_array(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	pass = !((lhs && lhs->data.is_array) || (rhs && rhs->data.is_array));

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "The operation '%s' does not work with arrays.\n",
			node->data.name);
	}

	return pass;
}

int binop_no_void(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	pass = 1;
	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs) return 0;
	if (!rhs) return 0;

	if (lhs->data.type == TYPE_VOID) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout,
			"'%s' requires operands of NONVOID but lhs is of %s.\n",
			node->data.name, ast_type_string(lhs->data.type));
	}

	if (rhs->data.type == TYPE_VOID) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout,
			"'%s' requires operands of NONVOID but rhs is of %s.\n",
			node->data.name, ast_type_string(rhs->data.type));
	}

	return pass;
}

int binop_only_array(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return 0;

	pass = lhs->data.is_array && rhs->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "The operation '%s' only works with arrays.\n",
			node->data.name);
	}

	return pass;
}

int binop_only_int(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return 0;

	if (lhs->data.type != TYPE_INT && lhs->data.type != TYPE_NONE) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout,
			"'%s' requires operands of type int but lhs is of %s.\n",
			node->data.name, ast_type_string(lhs->data.type));
	}

	if (rhs->data.type != TYPE_INT && rhs->data.type != TYPE_NONE) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout,
			"'%s' requires operands of type int but rhs is of %s.\n",
			node->data.name, ast_type_string(rhs->data.type));
	}

	return pass;
}

int binop_only_bool(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return 0;

	if (lhs->data.type != TYPE_BOOL && lhs->data.type != TYPE_NONE) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout,
			"'%s' requires operands of type bool but lhs is of %s.\n",
			node->data.name, ast_type_string(lhs->data.type));
	}

	if (rhs->data.type != TYPE_BOOL && rhs->data.type != TYPE_NONE) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout,
			"'%s' requires operands of type bool but rhs is of %s.\n",
			node->data.name, ast_type_string(rhs->data.type));
	}

	return pass;
}

int binop_only_char_or_int(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	pass = 1;
	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return 0;

	if (lhs->data.type != TYPE_CHAR && lhs->data.type != TYPE_INT
		&& lhs->data.type != TYPE_NONE
	) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout, "'%s' requires operands of type char or type int ",
			node->data.name);
		fprintf(stdout, "but lhs is of %s.\n",
			ast_type_string(lhs->data.type));
	}

	if (rhs->data.type != TYPE_CHAR && rhs->data.type != TYPE_INT
		&& rhs->data.type != TYPE_NONE
	) {
		pass = 0;
		error_lineno(node);
		fprintf(stdout, "'%s' requires operands of type char or type int ",
			node->data.name);
		fprintf(stdout, "but rhs is of %s.\n",
			ast_type_string(rhs->data.type));
	}

	return pass;
}

int binop_same_type(ast_t* node) {
	int pass;
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return 0;

	if (lhs->data.type == TYPE_NONE) return 0;
	if (rhs->data.type == TYPE_NONE) return 0;

	pass = lhs->data.type == rhs->data.type;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "'%s' requires operands of the same type but ",
			node->data.name);
		fprintf(stdout, "lhs is %s and rhs is %s.\n",
			ast_type_string(lhs->data.type), ast_type_string(rhs->data.type));
	}

	return pass;
}
