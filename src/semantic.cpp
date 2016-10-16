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
		case NODE_COMPOUND:
			msg = (char*) malloc(sizeof(char) * 30);
			sprintf(msg, "compound stmt %i", node->lineno);
			sem_symtab.enter(std::string(msg));
			break;
		case NODE_FUNC:
			sem_symtab.insert(node->data.name, node);
			break;
		case NODE_ID:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def != NULL) {
				/* Don't type IDs that reference functions */
				if (def->type != NODE_FUNC) node->data.type = def->data.type;
			}
			break;
		case NODE_VAR:
			sem_symtab.insert(node->data.name, node);
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
				fprintf(stdout, "'%s' requires both operants to be int.\n",
					node->data.name);
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
			sem_symtab.leave();
			break;
	}

	return;
}
