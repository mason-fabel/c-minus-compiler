#include <map>
#include <stack>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "ast.h"
#include "emit.h"
#include "codegen.h"

#define PARAM_STR_LEN 10

extern int offset;

static void traverse(ast_t* node);
static int base_reg(ast_t* var);

static int main_addr;
static int tmp_offset;
static std::map<std::string, int> func_addr;
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

	backPatchAJumpToHere(0, "Jump to INIT [BACKPATCH]");

	emitComment("INIT");
	emitRM("LD", GP, 0, GP,  "Set GP");
	emitRM("LDA", FP, offset, GP,  "Set first frame");
	emitRM("ST", FP, 0, FP,  "Store old FP (point to self)");
	emitComment("INIT GLOBALS");
	emitComment("END INIT GLOBALS");
	emitRM("LDA", AC, 1, PC, "Return address in AC");
	if (main_addr > 0) emitRMAbs("LDA", PC, main_addr - 1, "Jump to main");
	emitRO("HALT", 0, 0, 0, "DONE!");
	emitComment("END INIT");

	return;
}

int base_reg(ast_t* var) {
	int reg;

	switch (var->data.mem.scope) {
		case SCOPE_GLOBAL:
		case SCOPE_STATIC:
			reg = GP;
			break;
		case SCOPE_LOCAL:
		case SCOPE_PARAM:
			reg = FP;
			break;
	}

	return reg;
}

void traverse(ast_t* node) {
	if (node == NULL) return;

	switch (node->type) {
		case NODE_ASSIGN:
			if (node->child[0]->type == NODE_ID) {
				emitRM("LD", AC1, node->child[0]->data.mem.loc,
					base_reg(node->child[0]),
					"Load variable", node->child[0]->data.name);
				emitRM("ST", AC, tmp_offset--, FP, "Store value");
			} else if (node->child[0]->type == NODE_OP) {
				traverse(node->child[0]->child[1]);
				emitRM("LDC", AC1, node->child[0]->child[0]->data.mem.loc, NONE,
					"Load offset of array",
					node->child[0]->child[0]->data.name);
				emitRO("SUB", AC1, AC1, AC, "Find offset of element");
				emitRM("LDA", AC, 0, FP, "Copy FP to AC");
				emitRO("ADD", AC1, AC1, AC, "Find address of element");
				emitRM("LDA", AC, 0, AC1, "OP [");
				emitRM("ST", AC, tmp_offset--, FP, "Store value");
			}

			switch (node->data.op) {
				case OP_ASS:
					traverse(node->child[1]);
					++tmp_offset; // pop value
					break;
				case OP_ADDASS:
					traverse(node->child[1]);
					emitRM("LD", AC1, ++tmp_offset, FP, "Load value");
					emitRO("ADD", AC, AC1, AC, "ADD for OP +=");
					break;
				case OP_DIVASS:
					traverse(node->child[1]);
					emitRM("LD", AC1, ++tmp_offset, FP, "Load value");
					emitRO("DIV", AC, AC1, AC, "DIV for OP /=");
					break;
				case OP_MULASS:
					traverse(node->child[1]);
					emitRM("LD", AC1, ++tmp_offset, FP, "Load value");
					emitRO("MUL", AC, AC1, AC, "MUL for OP *=");
					break;
				case OP_SUBASS:
					traverse(node->child[1]);
					emitRM("LD", AC1, ++tmp_offset, FP, "Load value");
					emitRO("SUB", AC, AC1, AC, "SUB for OP -=");
					break;
				case OP_INC:
					emitRM("LD", AC1, ++tmp_offset, FP, "Load value");
					emitRM("LDC", AC, 1, NONE, "Load integer constant");
					emitRO("ADD", AC, AC1, AC, "ADD for OP ++");
					break;
				case OP_DEC:
					emitRM("LD", AC1, ++tmp_offset, FP, "Load value");
					emitRM("LDC", AC, 1, NONE, "Load integer constant");
					emitRO("SUB", AC, AC1, AC, "SUB for OP --");
					break;
			}

			if (node->child[0]->type == NODE_ID) {
				emitRM("ST", AC, node->child[0]->data.mem.loc,
					base_reg(node->child[0]),
					"Store variable", node->child[0]->data.name);
			} else if (node->child[0]->type == NODE_OP) {
				emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
				traverse(node->child[0]->child[1]);
				emitRM("LDC", AC1, node->child[0]->child[0]->data.mem.loc, NONE,
					"Load offset of array",
					node->child[0]->child[0]->data.name);
				emitRO("SUB", AC1, AC1, AC, "Find offset of element");
				emitRM("LDA", AC, 0, FP, "Copy FP to AC");
				emitRO("ADD", AC1, AC1, AC, "Find address of element");
				emitRM("LD", AC, ++tmp_offset, FP, "Load RHS");
				emitRM("ST", AC, 0, AC1,
					"Store element in array",
					node->child[0]->child[0]->data.name);
			}

			break;


		case NODE_CALL:
			int i;
			char* str;
			ast_t* param;

			emitComment("CALL", node->data.name);
			emitRM("ST", FP, tmp_offset, FP, "Store old FP in ghost frame");

			tmp_offset -= 2;

			i = 0;
			for (param = node->child[0]; param; param = param->sibling) {
				str = (char*) malloc(sizeof(char) * PARAM_STR_LEN);
				sprintf(str, "%i", ++i);
				emitComment("LOAD PARAM", str);
				traverse(param);
				emitRM("ST", AC, tmp_offset--, FP,
					"Store parameter");
			}

			tmp_offset += i;
			tmp_offset += 2;

			emitComment("JUMP TO", node->data.name);
			emitRM("LDA", FP, tmp_offset, FP, "Load addr of new frame");
			emitRM("LDA", AC, 1, PC, "Return addr in AC");
			emitRMAbs("LDA", PC, func_addr[node->data.name],
				"CALL", node->data.name);
			emitRM("LDA", AC, 0, RT, "Save result in AC");
			emitComment("END CALL", node->data.name);

			break;

		case NODE_COMPOUND:
			emitComment("COMPOUND");
			traverse(node->child[0]);
			emitComment("COMPOUND BODY");
			traverse(node->child[1]);
			emitComment("END COMPOUND");
			break;

		case NODE_CONST:
			switch (node->data.type) {
				case TYPE_BOOL:
					emitRM("LDC", AC, node->data.bool_val, NONE,
						"Load boolean constant");
					break;
				case TYPE_CHAR:
					emitRM("LDC", AC, node->data.char_val, NONE,
						"Load character constant");
					break;
				case TYPE_INT:
					emitRM("LDC", AC, node->data.int_val, NONE,
						"Load integer constant");
					break;
			}
			break;

		case NODE_ID:
			if (node->data.is_array) {
				emitRM("HALT", NONE, NONE, NONE,
					"NODE_ID.is_array not implemented");
			} else {
				emitRM("LD", AC, node->data.mem.loc, base_reg(node),
					"Load variable", node->data.name);
			}
			break;

		case NODE_IF:
			int jump_addr;
			int then_addr;
			int else_addr;

			emitComment("IF");
			traverse(node->child[0]);

			emitComment("THEN");
			then_addr = emitSkip(1);
			traverse(node->child[1]);
			jump_addr = emitSkip(1);
			backPatchAJumpToHere("JZR", AC, then_addr,
				"Jump past THEN if false [BACKPATCH]");
			emitBackup(jump_addr);

			if (node->child[2]) {
				emitComment("ELSE");
				else_addr = emitSkip(1);
				traverse(node->child[2]);
				backPatchAJumpToHere(else_addr,
					"Jump around the ELSE [BACKPATCH]");
			}

			emitComment("END IF");

			break;

		case NODE_FUNC:
			func_addr[std::string(node->data.name)] = emitSkip(0);

			emitComment("FUNCTION", node->data.name);
			emitRM("ST", AC, -1, FP, "Store return address");

			if (!strcmp("input", node->data.name)) {
				emitRO("IN", RT, NONE, NONE, "Grab int input");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else if (!strcmp("inputb", node->data.name)) {
				emitRO("INB", RT, NONE, NONE, "Grab bool input");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else if (!strcmp("inputc", node->data.name)) {
				emitRO("INC", RT, NONE, NONE, "Grab char input");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else if (!strcmp("output", node->data.name)) {
				emitRM("LD", AC, (node->child[0])->data.mem.loc, FP,
					"Load parameter");
				emitRO("OUT", AC, NONE, NONE, "Output int");
				emitRM("LDC", RT, 0, NONE, "Set return value to 0");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else if (!strcmp("outputb", node->data.name)) {
				emitRM("LD", AC, (node->child[0])->data.mem.loc, FP,
					"Load parameter");
				emitRO("OUTB", AC, NONE, NONE, "Output bool");
				emitRM("LDC", RT, 0, NONE, "Set return value to 0");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else if (!strcmp("outputc", node->data.name)) {
				emitRM("LD", AC, (node->child[0])->data.mem.loc, FP,
					"Load parameter");
				emitRO("OUTC", AC, NONE, NONE, "Output char");
				emitRM("LDC", RT, 0, NONE, "Set return value to 0");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else if (!strcmp("outnl", node->data.name)) {
				emitRO("OUTNL", NONE, NONE, NONE, "Output newline");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else if (!strcmp("main", node->data.name)) {
				curr_func = node;
				tmp_offset = node->data.mem.size;
				main_addr = emitSkip(0);
				traverse(node->child[1]);
				emitComment("FUNCTION CLOSE", node->data.name);
				emitRM("LDC", RT, 0, NONE, "Set return value to 0");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			} else {
				curr_func = node;
				tmp_offset = node->data.mem.size;
				traverse(node->child[1]);
				emitComment("FUNCTION CLOSE", node->data.name);
				emitRM("LDC", RT, 0, NONE, "Set return value to 0");
				emitRM("LD", AC, -1, FP, "Load return address");
				emitRM("LD", FP, 0, FP, "Adjust FP");
				emitRM("LDA", PC, 0, AC, "Return");
			}


			emitComment("END FUNCTION", node->data.name);

			curr_func = NULL;
			tmp_offset = 0;

			break;

		case NODE_OP:
			switch (node->data.op) {
				case OP_ADD:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("ADD", AC, AC, AC1, "OP +");
					break;
				case OP_AND:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("AND", AC, AC, AC1, "OP and");
					break;
				case OP_DIV:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("DIV", AC, AC, AC1, "OP /");
					break;
				case OP_EQ:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("TEQ", AC, AC, AC1, "OP ==");
					break;
				case OP_GRT:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("TGT", AC, AC, AC1, "OP >");
					break;
				case OP_GRTEQ:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("TGE", AC, AC, AC1, "OP >=");
					break;
				case OP_LESS:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("TLT", AC, AC, AC1, "OP <");
					break;
				case OP_LESSEQ:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("TLE", AC, AC, AC1, "OP <=");
					break;
				case OP_MOD:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("TLT", AC2, AC, AC1, "OP % CHECK");
					emitRM("JNZ", AC2, 2, PC, "Jump past OP % if true");
					emitRO("SUB", AC, AC, AC1, "SUB RHS from LHS");
					emitRM("LDA", PC, -4, PC, "Loop to CHECK");
					break;
				case OP_MUL:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("MUL", AC, AC, AC1, "OP *");
					break;
				case OP_NEG:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRM("LDC", AC1, -1, NONE, "Load integer constant");
					emitRO("MUL", AC, AC, AC1, "UNARY OP -");
					break;
				case OP_NOT:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRM("LDC", AC1, 0, NONE, "Load integer constant");
					emitRO("TEQ", AC, AC, AC1, "UNARY OP not");
					break;
				case OP_NOTEQ:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("TNE", AC, AC, AC1, "OP !=");
					break;
				case OP_QMARK:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("RND", AC, AC, NONE, "UNARY OP ?");
					break;
				case OP_OR:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("OR", AC, AC, AC1, "OP or");
					break;
				case OP_SIZE:
					emitRM("LD", AC, node->child[0]->data.mem.loc + 1,
						base_reg(node->child[0]), "UNARY OP *");
					break;
				case OP_SUB:
					traverse(node->child[0]);
					emitRM("ST", AC, tmp_offset--, FP, "Store LHS");
					traverse(node->child[1]);
					emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
					emitRM("LD", AC1, ++tmp_offset, FP, "Load RHS");
					emitRM("LD", AC, ++tmp_offset, FP, "Load LHS");
					emitRO("SUB", AC, AC, AC1, "OP -");
					break;
				case OP_SUBSC:
					traverse(node->child[1]);
					emitRM("LDC", AC1, node->child[0]->data.mem.loc, NONE,
						"Load offset of array",
						node->child[0]->data.name);
					emitRO("SUB", AC1, AC1, AC, "Find offset of element");
					emitRM("LDA", AC, 0, FP, "Copy FP to AC");
					emitRO("ADD", AC1, AC1, AC, "Find address of element");
					emitRM("LD", AC, 0, AC1, "OP [");
					break;
				// emitRM("ST", AC, tmp_offset--, FP, "Store RHS");
				// emitRM("LD", AC, ++tmp_offset, FP, "Load RHS");
			}

			break;

		case NODE_RETURN:
			emitComment("RETURN");
			if (node->child[0]) {
				traverse(node->child[0]);
				emitRM("LDA", RT, 0, AC, "Copy result to RT");
			}
			emitRM("LD", AC, -1, FP, "Load return address");
			emitRM("LD", FP, 0, FP, "Adjust FP");
			emitRM("LDA", PC, 0, AC, "Return");
			break;

		case NODE_VAR:
			if (node->data.mem.scope != SCOPE_LOCAL) break;
			/* Initialize locals */
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
