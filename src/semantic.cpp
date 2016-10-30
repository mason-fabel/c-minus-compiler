#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "symtab.h"
#include "analysis/analysis.h"

void _sem_analysis(ast_t* node);
void pre_action(ast_t* node);
void post_action(ast_t* node);
void check_node(ast_t* node);
ast_t* _sem_link_io(ast_t* tree);

int break_depth;
ast_t* func_def;

extern int errors;
extern int warnings;

SymbolTable sem_symtab;

ast_t* sem_analysis(ast_t* tree) {
	ast_t* def;

	func_def = NULL;
	break_depth = 0;

	tree = _sem_link_io(tree);
	_sem_analysis(tree);
	return tree;

	def = (ast_t*) sem_symtab.lookupGlobal("main");
	if (def == NULL || def->type != NODE_FUNC) {
		errors++;
		fprintf(stdout, "ERROR(LINKER): Procedure main is not defined.\n");
	}

	return tree;
}

ast_t* _sem_link_io(ast_t* tree) {
	ast_t* head;
	ast_t* curr;
	ast_t* tmp;

	/* input */
	head = ast_create_node();
	head->lineno = -1;
	head->type = NODE_FUNC;
	head->data.name = (char*) "input";
	head->data.type = TYPE_INT;
	curr = head;

	/* output */
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_FUNC;
	tmp->data.name = (char*) "output";
	tmp->data.type = TYPE_VOID;
	ast_add_sibling(curr, tmp);
	curr = curr->sibling;
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_PARAM;
	tmp->data.name = (char*) "*dummy*";
	tmp->data.type = TYPE_INT;
	ast_add_child(curr, 0, tmp);

	/* inputb */
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_FUNC;
	tmp->data.name = (char*) "inputb";
	tmp->data.type = TYPE_BOOL;
	ast_add_sibling(curr, tmp);
	curr = curr->sibling;

	/* outputb */
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_FUNC;
	tmp->data.name = (char*) "outputb";
	tmp->data.type = TYPE_VOID;
	ast_add_sibling(curr, tmp);
	curr = curr->sibling;
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_PARAM;
	tmp->data.name = (char*) "*dummy*";
	tmp->data.type = TYPE_BOOL;
	ast_add_child(curr, 0, tmp);

	/* inputc */
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_FUNC;
	tmp->data.name = (char*) "inputc";
	tmp->data.type = TYPE_CHAR;
	ast_add_sibling(curr, tmp);
	curr = curr->sibling;

	/* outputc */
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_FUNC;
	tmp->data.name = (char*) "outputc";
	tmp->data.type = TYPE_VOID;
	ast_add_sibling(curr, tmp);
	curr = curr->sibling;
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_PARAM;
	tmp->data.name = (char*) "*dummy*";
	tmp->data.type = TYPE_VOID;
	ast_add_child(curr, 0, tmp);

	/* outnl */
	tmp = ast_create_node();
	tmp->lineno = -1;
	tmp->type = NODE_FUNC;
	tmp->data.name = (char*) "outnl";
	tmp->data.type = TYPE_VOID;
	ast_add_sibling(curr, tmp);
	curr = curr->sibling;

	ast_add_sibling(head, tree);

	return head;
}

void _sem_analysis(ast_t* node) {
	int i;

	if (node == NULL) return;

	pre_action(node);

	for (i = 0; i < node->num_children; i++) {
		_sem_analysis(node->child[i]);
	}

	check_node(node);
	post_action(node);

	_sem_analysis(node->sibling);

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
			else error_func_defined(node);
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
			func_def = node;
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
			break_depth++;
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
			if (func_def->data.type != TYPE_VOID && func_def->lineno != -1
				&&  !return_exists(func_def->child[1])
			) {
				warning_lineno(node);
				fprintf(stdout, "Expecting to return %s but function '%s' ",
					ast_type_string(node->data.type), node->data.name);
				fprintf(stdout, "has no return statement.\n");
			}
			func_def = NULL;
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
		case NODE_WHILE:
			break_depth--;
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
					binop_match_array(node);
			}
			break;
		case NODE_BREAK:
			if (break_depth < 1) error_invalid_break(node);
			break;
		case NODE_CALL:
			id_only_func(node);
			break;
		case NODE_ID:
			id_defined(node);
			id_not_func(node);
			break;
		case NODE_IF:
			test_if_only_bool(node);
			test_if_no_array(node);
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
					binop_match_array(node);
					break;
				case OP_GRT:
				case OP_GRTEQ:
				case OP_LESS:
				case OP_LESSEQ:
					binop_only_char_or_int(node) && binop_same_type(node);
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
			return_match_type(node, func_def);
			break;
		case NODE_WHILE:
			test_while_only_bool(node);
			test_while_no_array(node);
			break;
	}

	return;
}
