#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <vector>
#include "ast.h"
#include "semantic.h"
#include "symtab.h"
#include "analysis/analysis.h"

struct mem_data_t {
	int compound_depth;
	int offset;
};

void _sem_analysis(ast_t* node);
void pre_action(ast_t* node);
void post_action(ast_t* node);
void check_node(ast_t* node);
ast_t* _sem_link_io(ast_t* tree);

int break_depth;
int compound_depth;
int num_return;
ast_t* func_def;
std::stack<int> mem_offset;
std::vector<ast_t*> recursive_calls;

extern int errors;
extern int offset;
extern int warnings;

SymbolTable sem_symtab;

ast_t* sem_analysis(ast_t* tree) {
	ast_t* def;

	func_def = NULL;
	break_depth = 0;
	num_return = 0;
	compound_depth = 0;
	mem_offset.push(0);

	tree = _sem_link_io(tree);
	_sem_analysis(tree);

	def = (ast_t*) sem_symtab.lookupGlobal("main");
	if (def == NULL || def->type != NODE_FUNC) {
		errors++;
		fprintf(stdout, "ERROR(LINKER): Procedure main is not defined.\n");
	}

	offset = mem_offset.top();

	return tree;
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
	tmp->data.type = TYPE_CHAR;
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

void pre_action(ast_t* node) {
	char* msg;
	ast_t* def;

	switch (node->type) {
		case NODE_BREAK:
			node->data.type = TYPE_VOID;
			break;
		case NODE_CALL:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def == NULL) {
				error_func_defined(node);
			} else if (def == func_def) {
				node->data.type = def->data.type;
				recursive_calls.push_back(node);
			} else {
				node->data.type = def->data.type;
				node->data.mem.size = def->data.mem.size;
			}
			break;
		case NODE_COMPOUND:
			msg = (char*) malloc(sizeof(char) * 80);
			sprintf(msg, "compound stmt %i", node->lineno);
			if (!node->data.is_func_body) sem_symtab.enter(std::string(msg));
			compound_depth++;
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
			num_return = 0;
			mem_offset.push(0);
			break;
		case NODE_ID:
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			if (def && def->type != NODE_FUNC) {
				node->data.type = def->data.type;
				node->data.is_array = def->data.is_array;
				node->data.mem.scope = def->data.mem.scope;
				node->data.mem.size = def->data.mem.size;
				node->data.mem.loc = def->data.mem.loc;
			} else if (def && def->type == NODE_FUNC) {
				node->data.mem.scope = def->data.mem.scope;
				if (def == func_def) {
					/* I'm not sure why SCOPE_LOCAL and -3 are the right values
					 * but there's really no reasonable value for an ID that
					 * references a function and this matches the given output
					 */
					// recursive_calls.push_back(node);
					node->data.mem.scope = SCOPE_LOCAL;
					node->data.mem.size = -3;
				} else {
					node->data.mem.size = def->data.mem.size;
				}
				node->data.mem.loc = def->data.mem.loc;
			}
			break;
		case NODE_PARAM:
			if (!sem_symtab.insert(node->data.name, node)) {
				error_symbol_defined(node);
			} else {
				node->data.mem.loc = mem_offset.top() - 2;
				mem_offset.top() -= node->data.mem.size;
			}
			node->data.mem.scope = SCOPE_PARAM;
			break;
		case NODE_RETURN:
			num_return++;
			break;
		case NODE_WHILE:
			node->data.type = TYPE_VOID;
			break_depth++;
			break;
	}

	return;
}

void post_action(ast_t* node) {
	std::vector<ast_t*>::iterator call;

	switch (node->type) {
		case NODE_ASSIGN:
			switch (node->data.op) {
				case OP_ADDASS:
				case OP_DIVASS:
				case OP_MULASS:
				case OP_SUBASS:
				case OP_INC:
				case OP_DEC:
					node->data.type = TYPE_INT;
					break;
				default:
					node->data.type = (node->child[0])->data.type;
					node->data.is_array = (node->child[0])->data.is_array;
			}
			break;
		case NODE_COMPOUND:
			if (!node->data.is_func_body) sem_symtab.leave();
			compound_depth--;
			break;
		case NODE_FUNC:
			if (func_def->data.type != TYPE_VOID && func_def->lineno != -1
				&& num_return < 1
			) {
				warning_lineno(node);
				fprintf(stdout, "Expecting to return %s but function '%s' ",
					ast_type_string(node->data.type), node->data.name);
				fprintf(stdout, "has no return statement.\n");
			}

			node->data.mem.scope = SCOPE_GLOBAL;
			node->data.mem.size = mem_offset.top() - 2;
			node->data.mem.loc = 0;

			call = recursive_calls.begin();
			while (call != recursive_calls.end()) {
				(*call)->data.mem.size = node->data.mem.size;
				call++;
			}
			recursive_calls.clear();

			sem_symtab.leave();
			mem_offset.pop();
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
				node->data.mem.scope = SCOPE_LOCAL;
				node->data.mem.size = node->data.is_array
					? node->data.int_val + 1 : 1;
				node->data.mem.loc = node->data.is_array ? -1 : 0;
			} else {
				if (node->data.is_static) {
					std::stack<int> reverse;
					if (node->data.is_array) node->data.mem.loc -= 1;
					while (!mem_offset.empty()) {
						reverse.push(mem_offset.top());
						mem_offset.pop();
					}
					node->data.mem.scope = SCOPE_STATIC;
					node->data.mem.size = node->data.is_array
						? node->data.int_val + 1 : 1;
					node->data.mem.loc = reverse.top();
					if (node->data.is_array) node->data.mem.loc -= 1;
					reverse.top() -= node->data.mem.size;
					while (!reverse.empty()) {
						mem_offset.push(reverse.top());
						reverse.pop();
					}
				} else {
					node->data.mem.scope = compound_depth
						? SCOPE_LOCAL : SCOPE_GLOBAL;
					node->data.mem.size = node->data.is_array
						? node->data.int_val + 1 : 1;
					node->data.mem.loc = compound_depth
						? mem_offset.top() - 2 : mem_offset.top();
					if (node->data.is_array) node->data.mem.loc -= 1;
					mem_offset.top() -= node->data.mem.size;
				}
			}
			break;
		case NODE_WHILE:
			break_depth--;
			break;
	}

	return;
}

void check_node(ast_t* node) {
	ast_t* def;
	switch (node->type) {
		case NODE_ASSIGN:
			switch (node->data.op) {
				case OP_ADDASS:
				case OP_DIVASS:
				case OP_MULASS:
				case OP_SUBASS:
					binop_only_int(node);
					binop_no_array(node);
					break;
				case OP_INC:
				case OP_DEC:
					unary_only_int(node);
					unary_no_array(node);
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
			def = (ast_t*) sem_symtab.lookup(std::string(node->data.name));
			id_only_func(node);
			call_params(node, def);
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
					binop_no_void(node) && binop_same_type(node);
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
			return_match_type(node, func_def);
			return_no_array(node);
			break;
		case NODE_VAR:
			init_only_const(node);
			init_match_type(node);
			break;
		case NODE_WHILE:
			test_while_only_bool(node);
			test_while_no_array(node);
			break;
	}

	return;
}
