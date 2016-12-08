#include <stack>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "emit.h"
#include "codegen.h"

static void traverse(ast_t* node);

static int main_addr;
static int tmp_offset;
static ast_t* curr_func;

void codegen(ast_t* tree, FILE* fout) {
	curr_func = NULL;
	main_addr = -1;
	tmp_offset = 0;

	emitSetFile(fout);

	emitComment("C- compiler version F16");
	emitComment("Author: Mason Fabel");

	emitSkip(1);

	traverse(tree);

	backPatchAJumpToHere(0, "Jump to init [backpatch]");

	emitComment("INIT");
	emitRM("LDA", AC, 1, PC, "Return address in AC");
	if (main_addr > 0) emitRMAbs("LDA", PC, main_addr - 1, "Jump to main");
	emitRO("HALT", 0, 0, 0, "DONE!");
	emitComment("END INIT");

	return;
}

void traverse(ast_t* node) {
	if (node == NULL) return;

	switch (node->type) {
		case NODE_COMPOUND:
			emitComment("COMPOUND");
			traverse(node->child[0]);
			emitComment("COMPOUND BODY");
			traverse(node->child[1]);
			emitComment("END COMPOUND");
			break;

		case NODE_FUNC:
			emitComment("FUNCTION", node->data.name);

			emitRM("ST", AC, -1, FP, "Store return address");

			if (!strcmp("input", node->data.name)) {
				emitRO("IN", RT, NONE, NONE, "Grab int input");
			} else if (!strcmp("inputb", node->data.name)) {
				emitRO("INB", RT, NONE, NONE, "Grab bool input");
			} else if (!strcmp("inputc", node->data.name)) {
				emitRO("INC", RT, NONE, NONE, "Grab char input");
			} else if (!strcmp("output", node->data.name)) {
				emitRM("LD", AC, (node->child[0])->data.mem.loc, FP,
					"Load parameter");
				emitRO("OUT", AC, NONE, NONE, "Output int");
			} else if (!strcmp("outputb", node->data.name)) {
				emitRM("LD", AC, (node->child[0])->data.mem.loc, FP,
					"Load parameter");
				emitRO("OUTB", AC, NONE, NONE, "Output bool");
			} else if (!strcmp("outputc", node->data.name)) {
				emitRM("LD", AC, (node->child[0])->data.mem.loc, FP,
					"Load parameter");
				emitRO("OUTC", AC, NONE, NONE, "Output char");
			} else if (!strcmp("outnl", node->data.name)) {
				emitRO("OUTNL", NONE, NONE, NONE, "Output newline");
			} else if (!strcmp("main", node->data.name)) {
				curr_func = node;
				tmp_offset = node->data.mem.size;
				main_addr = emitSkip(0);
				traverse(node->child[1]);
			} else {
				curr_func = node;
				tmp_offset = node->data.mem.size;
				traverse(node->child[1]);
			}

			emitComment("FUNCTION CLOSE", node->data.name);
			emitRM("LDC", RT, 0, NONE, "Set return value to 0");
			emitRM("LD", AC, -1, FP, "Load return address");
			emitRM("LD", FP, 0, FP, "Adjust FP");
			emitRM("LDA", PC, 0, AC, "Return");

			emitComment("END FUNCTION", node->data.name);

			curr_func = NULL;

			break;

		case NODE_OP:
			traverse(node->child[0]);
			emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
			traverse(node->child[1]);
			emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
			emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
			emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");

			switch (node->data.op) {
				case OP_ADD:
					emitRO("ADD", AC, AC, AC1, "Op +");
					break;
			}

			break;

		case NODE_CONST:
			switch (node->data.type) {
				case TYPE_INT:
					emitRM("LDC", AC, node->data.int_val, NONE,
						"Load integer constant");
			}
			break;

		case NODE_VAR:
			if (node->data.mem.scope != SCOPE_LOCAL) break;
			if (node->data.is_array) {
				emitRM("LDC", AC, node->data.mem.size - 1, NONE,
					"Load size of array", node->data.name);
				emitRM("ST", AC, node->data.mem.loc + 1, FP,
					"Save size of array", node->data.name);
			} else if (node->child[0]) {
				traverse(node->child[0]);
				emitRM("ST", AC, node->data.mem.loc, FP,
					"Store variable", node->data.name);
			}
			break;
	}

	traverse(node->sibling);

	return;
}
