#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "symtab.h"

void _sem_type(ast_t* node);
void pre_action(ast_t* node);
void post_action(ast_t* node);
void check_node(ast_t* node);

void binop_no_array(ast_t* node);
void binop_only_array(ast_t* node);
void binop_only_int(ast_t* node);
void binop_same_type(ast_t* node);
void id_defined(ast_t* node);
void id_not_func(ast_t* node);
void index_only_array(ast_t* node);
void index_only_int(ast_t* node);
void unary_only_array(ast_t* node);
void unary_only_int(ast_t* node);

void error_lineno(ast_t* node);

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
			sem_symtab.insert(node->data.name, node);
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
			sem_symtab.insert(node->data.name, node);
			break;
		case NODE_VAR:
			sem_symtab.insert(node->data.name, node);
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
			node->data.type = (node->child[0])->data.type;
			break;
		case NODE_COMPOUND:
			if (!node->data.is_func_body) sem_symtab.leave();
			break;
		case NODE_FUNC:
			sem_symtab.leave();
			break;
		case NODE_OP:
			switch (node->data.op) {
				case OP_EQ:
				case OP_LESS:
				case OP_LESSEQ:
				case OP_GRT:
				case OP_GRTEQ:
					node->data.type = TYPE_BOOL;
					break;
				case OP_SIZE:
				case OP_ADD:
				case OP_SUB:
					node->data.type = TYPE_INT;
					break;
				default:
					node->data.type = (node->child[0])->data.type;
			}
			break;
	}

	return;
}

void check_node(ast_t* node) {
	switch (node->type) {
		case NODE_ASSIGN:
			binop_same_type(node);
			break;
		case NODE_ID:
			id_defined(node);
			id_not_func(node);
			break;
		case NODE_OP:
			switch (node->data.op) {
				case OP_SIZE:
					unary_only_array(node);
					break;
				case OP_SUBSC:
					index_only_array(node);
					index_only_int(node);
					break;
				case OP_EQ:
				case OP_NOTEQ:
					binop_same_type(node);
					break;
				case OP_AND:
				case OP_OR:
				case OP_NOT:
					binop_same_type(node);
					binop_no_array(node);
					break;
				default:
					binop_only_int(node);
					binop_no_array(node);
			}
			break;
	}

	return;
}

void binop_no_array(ast_t* node) {
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if ((lhs && lhs->data.is_array) || (rhs && rhs->data.is_array)) {
		errors++;
		fprintf(stdout, "ERROR(%i): ", node->lineno);
		fprintf(stdout, "The operation '%s' does not work with arrays.\n",
			node->data.name);
	}

	return;
}

void binop_only_array(ast_t* node) {
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return;

	if (!lhs->data.is_array || !rhs->data.is_array) {
		errors++;
		fprintf(stdout, "ERROR(%i): ", node->lineno);
		fprintf(stdout, "The operation '%s' only works with arrays.\n",
			node->data.name);
	}

	return;
}

void binop_only_int(ast_t* node) {
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return;

	if (lhs->data.type != TYPE_INT || rhs->data.type != TYPE_INT) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "'%s' requires both operants to be int.\n",
			node->data.name);
	}

	return;
}

void binop_same_type(ast_t* node) {
	ast_t* lhs;
	ast_t* rhs;

	lhs = node->child[0];
	rhs = node->child[1];

	if (!lhs || !rhs) return;

	if (lhs->data.type == TYPE_NONE) return;
	if (rhs->data.type == TYPE_NONE) return;

	if (lhs->data.type != rhs->data.type) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "'%s' requires operands of the same type but ",
			node->data.name);
		fprintf(stdout, "lhs is %s and rhs is %s.\n",
			ast_type_string(lhs->data.type), ast_type_string(rhs->data.type));
	}

	return;
}

void id_defined(ast_t* node) {
	ast_t* def;

	def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));

	if (!def) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "Symbol '%s' is not defined.\n", node->data.name);
	}

	return;
}

void id_not_func(ast_t* node) {
	ast_t* def;

	def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));

	if (!def) return;

	if (def->type == NODE_FUNC) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "Cannot use function '%s' as a variable.\n",
			node->data.name);
	}

	return;
}

void index_only_array(ast_t* node) {
	ast_t* arr;

	arr = node->child[0];

	if (!arr) return;

	if (!arr->data.is_array) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "Cannot index nonarray");
		if (arr->type == NODE_ID) {
			fprintf(stdout, " '%s'", arr->data.name);
		}
		fprintf(stdout, ".\n");
	}

	return;
}

void index_only_int(ast_t* node) {
	ast_t* arg;

	arg = node->child[1];

	if (!arg) return;

	if (arg->data.type != TYPE_INT && arg->data.type != TYPE_NONE) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "Array '%s' should be indexed by ",
			(node->child[0])->data.name);
		fprintf(stdout, "type int but got %s.\n",
			ast_type_string(arg->data.type));
	}

	return;
}

void unary_only_array(ast_t* node) {
	ast_t* arr;

	arr = node->child[0];

	if (!arr) return;

	if (!arr->data.is_array) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "The operation '%s' only works with arrays.\n",
			node->data.name);
	}

	return;
}

void unary_only_int(ast_t* node) {
	ast_t* arg;

	arg = node->child[0];

	if (!arg) return;

	if (arg->data.type != TYPE_INT && arg->data.type != TYPE_NONE) {
		errors++;
		error_lineno(node);
		fprintf(stdout, "The operation '%s' only works with arrays.\n",
			node->data.name);
	}

	return;
}

void error_lineno(ast_t* node) {
	fprintf(stdout, "ERROR(%i): ", node->lineno);

	return;
}
