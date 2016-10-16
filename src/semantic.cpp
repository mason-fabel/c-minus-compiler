#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "symtab.h"

void _sem_type(ast_t* node);
void pre_action(ast_t* node);
void post_action(ast_t* node);
void check_node(ast_t* node);

extern int errors;
extern int warnings;

SymbolTable sem_symtab;

void sem_analysis(ast_t* tree) {
	ast_t* def;

	_sem_type(tree);

	def = (ast_t*) sem_symtab.lookupGlobal("main");
	if (!(def != NULL && def->type == NODE_FUNC)) {
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
		case NODE_CALL:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def != NULL) node->data.type = def->data.type;
			break;
		case NODE_COMPOUND:
			msg = (char*) malloc(sizeof(char) * 30);
			sprintf(msg, "compound stmt %i", node->lineno);
			if (!node->data.is_func_body) sem_symtab.enter(std::string(msg));
			break;
		case NODE_IF:
			node->data.type = TYPE_VOID;
			break;
		case NODE_FUNC:
			(node->child[1])->data.is_func_body = 1;
			sem_symtab.insert(node->data.name, node);
			msg = (char*) malloc(sizeof(char) * 30);
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
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def == NULL) sem_symtab.insert(node->data.name, node);
			break;
		case NODE_VAR:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def == NULL) sem_symtab.insert(node->data.name, node);
			break;
		case NODE_WHILE:
			node->data.type = TYPE_VOID;
			break;
	}

	return;
}

void check_node(ast_t* node) {
	int error;
	ast_type_t lhs;
	ast_type_t rhs;
	ast_t* def;

	switch (node->type) {
		case NODE_ASSIGN:
			error = 0;
			lhs = (node->child[0])->data.type;
			rhs = (node->child[1])->data.type;
			if (lhs != rhs) error = 1;
			if (error && (lhs == TYPE_NONE || rhs == TYPE_NONE)) error = 0;
			if (error) {
				errors++;
				fprintf(stdout, "ERROR(%i): ", node->lineno);
				fprintf(stdout, "'%s' requires operands of the same type but ",
					node->data.name);
				fprintf(stdout, "lhs is %s and rhs is %s.\n",
					ast_type_string(lhs), ast_type_string(rhs));
			}
			break;
		case NODE_ID:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def == NULL) {
				errors++;
				fprintf(stdout, "ERROR(%i): ", node->lineno);
				fprintf(stdout, "Symbol '%s' is not defined.\n",
					node->data.name);
			} else if (def->type == NODE_FUNC) {
				errors++;
				fprintf(stdout, "ERROR(%i): ", node->lineno);
				fprintf(stdout, "Cannot use function '%s' as a variable.\n",
					node->data.name);
			}
			break;
		case NODE_OP:
			error = 0;
			lhs = (node->child[0])->data.type;
			rhs = (node->child[1])->data.type;
			if (lhs != TYPE_INT && lhs != TYPE_NONE) error = 1;
			if (rhs != TYPE_INT && rhs != TYPE_NONE) error = 1;
			if (error) {
				errors++;
				fprintf(stdout, "ERROR(%i): ", node->lineno);
				fprintf(stdout,"'%s' requires both operants to be int.\n",
					node->data.name);
			}
			switch (node->data.op) {
				case OP_SUBSC:
					if (!(node->child[0])->data.is_array) {
						errors++;
						fprintf(stdout, "ERROR(%i): ", node->lineno);
						fprintf(stdout, "Cannot index nonarray");
						if ((node->child[0])->type == NODE_ID) {
						 	fprintf(stdout, " '%s'",
								(node->child[0])->data.name);
						}
						fprintf(stdout, ".\n");
					}
			}
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
				case OP_LESS:
				case OP_LESSEQ:
				case OP_GRT:
				case OP_GRTEQ:
					node->data.type = TYPE_BOOL;
					break;
				default:
					node->data.type = (node->child[0])->data.type;
			}
			break;
	}

	return;
}
