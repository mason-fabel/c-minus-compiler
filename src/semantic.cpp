#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "symtab.h"

void _sem_type(ast_t* node);
void pre_action(ast_t* node);
void post_action(ast_t* node);
void check_node(ast_t* node);

int binop_no_array(ast_t* node);
int binop_no_void(ast_t* node);
int binop_only_array(ast_t* node);
int binop_only_bool(ast_t* node);
int binop_only_char_or_int(ast_t* node);
int binop_only_int(ast_t* node);
int binop_same_type(ast_t* node);
int id_defined(ast_t* node);
int id_not_func(ast_t* node);
int id_only_func(ast_t* node);
int index_no_array(ast_t* node);
int index_only_array(ast_t* node);
int index_only_int(ast_t* node);
int return_no_array(ast_t* node);
int unary_no_array(ast_t* node);
int unary_only_array(ast_t* node);
int unary_only_bool(ast_t* node);
int unary_only_int(ast_t* node);

void error_lineno(ast_t* node);
void error_symbol_defined(ast_t* node);

extern int errors;
extern int warnings;

SymbolTable sem_symtab;

void sem_analysis(ast_t* tree) {
	ast_t* def;

	_sem_type(tree);

	def = (ast_t*) sem_symtab.lookupGlobal("main");
	if (def == NULL || def->type != NODE_FUNC) {
		errors++;
		fprintf(stdout, "ERROR(LINKER): Procedure main is not defined.\n");
	}

	return;
}

void _sem_type(ast_t* node) {
	int i;

	if (node == NULL) return;

	pre_action(node);

	for (i = 0; i < node->num_children; i++) {
		_sem_type(node->child[i]);
	}

	check_node(node);
	post_action(node);

	_sem_type(node->sibling);

	return;
}

void pre_action(ast_t* node) {
	char* msg;
	ast_t* def;

	switch (node->type) {
		case NODE_BREAK:
			node->data.type = TYPE_VOID;
			break;
		case NODE_CALL:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def != NULL) node->data.type = def->data.type;
			else id_defined(node);
			break;
		case NODE_COMPOUND:
			msg = (char*) malloc(sizeof(char) * 80);
			sprintf(msg, "compound stmt %i", node->lineno);
			if (!node->data.is_func_body) sem_symtab.enter(std::string(msg));
			break;
		case NODE_IF:
			node->data.type = TYPE_VOID;
			break;
		case NODE_FUNC:
			if (node->child[1]) (node->child[1])->data.is_func_body = 1;
			if (!sem_symtab.insert(node->data.name, node)) {
				error_symbol_defined(node);
			}
			msg = (char*) malloc(sizeof(char) * 80);
			sprintf(msg, "function %s", node->data.name);
			sem_symtab.enter(std::string(msg));
			break;
		case NODE_ID:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def != NULL && def->type != NODE_FUNC) {
				node->data.type = def->data.type;
				node->data.is_array = def->data.is_array;
			}
			break;
		case NODE_PARAM:
			if (!sem_symtab.insert(node->data.name, node)) {
				error_symbol_defined(node);
			}
			break;
		case NODE_WHILE:
			node->data.type = TYPE_VOID;
			break;
	}

	return;
}

void post_action(ast_t* node) {
	switch (node->type) {
		case NODE_ASSIGN:
			switch (node->data.op) {
				case OP_ADDASS:
				case OP_SUBASS:
					node->data.type = TYPE_INT;
					break;
				default:
					node->data.type = (node->child[0])->data.type;
			}
			break;
		case NODE_COMPOUND:
			if (!node->data.is_func_body) sem_symtab.leave();
			break;
		case NODE_FUNC:
			sem_symtab.leave();
			break;
		case NODE_OP:
			switch (node->data.op) {
				case OP_AND:
				case OP_EQ:
				case OP_GRT:
				case OP_GRTEQ:
				case OP_LESS:
				case OP_LESSEQ:
				case OP_NOT:
				case OP_NOTEQ:
				case OP_OR:
					node->data.type = TYPE_BOOL;
					break;
				case OP_ADD:
				case OP_DIV:
				case OP_MOD:
				case OP_MUL:
				case OP_NEG:
				case OP_QMARK:
				case OP_SIZE:
				case OP_SUB:
					node->data.type = TYPE_INT;
					break;
				default:
					node->data.type = (node->child[0])->data.type;
			}
			break;
		case NODE_VAR:
			if (!sem_symtab.insert(node->data.name, node)) {
				error_symbol_defined(node);
			}
			break;
	}

	return;
}

void check_node(ast_t* node) {
	switch (node->type) {
		case NODE_ASSIGN:
			switch (node->data.op) {
				case OP_ADDASS:
				case OP_SUBASS:
					binop_no_array(node) && binop_only_int(node);
					break;
				default:
					binop_no_void(node) && binop_same_type(node);
			}
			break;
		case NODE_CALL:
			id_only_func(node);
			break;
		case NODE_ID:
			id_defined(node);
			id_not_func(node);
			break;
		case NODE_OP:
			switch (node->data.op) {
				case OP_ADD:
				case OP_SUB:
				case OP_MUL:
				case OP_DIV:
				case OP_MOD:
					binop_only_int(node);
					binop_no_array(node);
					break;
				case OP_AND:
				case OP_OR:
					binop_only_bool(node);
					binop_no_array(node);
					break;
				case OP_EQ:
				case OP_NOTEQ:
					binop_same_type(node);
					binop_no_void(node);
					break;
				case OP_GRT:
				case OP_GRTEQ:
				case OP_LESS:
				case OP_LESSEQ:
					binop_same_type(node);
					binop_only_char_or_int(node);
					binop_no_array(node);
					break;
				case OP_NEG:
				case OP_QMARK:
					unary_only_int(node);
					unary_no_array(node);
					break;
				case OP_NOT:
					unary_only_bool(node);
					unary_no_array(node);
					break;
				case OP_SIZE:
					unary_only_array(node);
					break;
				case OP_SUBSC:
					index_only_array(node);
					index_only_int(node);
					index_no_array(node);
					break;
				default:
					binop_no_array(node) && binop_only_int(node);
			}
			break;

		case NODE_RETURN:
			return_no_array(node);
			break;
	}

	return;
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

int index_no_array(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[1];
	if (!arg) return 0;

	pass = !arg->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "Array index is the unindexed array '%s'.\n",
			arg->data.name);
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

int unary_no_array(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[0];

	pass = !arg->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "The operation '%s' does not work with arrays.\n",
			node->data.name);
	}

	return pass;
}

int unary_only_array(ast_t* node) {
	int pass;
	ast_t* arr;

	arr = node->child[0];
	if (!arr) return 0;

	pass = arr->data.is_array;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout, "The operation '%s' only works with arrays.\n",
			node->data.name);
	}

	return pass;
}

int unary_only_bool(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[0];
	if (!arg) return 0;

	pass = arg->data.type == TYPE_BOOL || arg->data.type == TYPE_NONE;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"Unary '%s' requires an operand of type %s but was given %s.\n",
			node->data.name, ast_type_string(TYPE_BOOL),
			ast_type_string(arg->data.type));
	}

	return pass;
}

int unary_only_int(ast_t* node) {
	int pass;
	ast_t* arg;

	arg = node->child[0];
	if (!arg) return 0;

	pass = arg->data.type == TYPE_INT || arg->data.type == TYPE_NONE;

	if (!pass) {
		error_lineno(node);
		fprintf(stdout,
			"Unary '%s' requires an operand of type %s but was given %s.\n",
			node->data.name, ast_type_string(TYPE_INT),
			ast_type_string(arg->data.type));
	}

	return pass;
}

void error_lineno(ast_t* node) {
	errors++;

	fprintf(stdout, "ERROR(%i): ", node->lineno);

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
