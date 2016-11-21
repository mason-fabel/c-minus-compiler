#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "flags.h"
#include "getopt.h"
#include "print_tree.h"
#include "semantic.h"
#include "symtab.h"
#include "yyerror.h"

#define FALSE 0
#define TRUE 1

extern void scanner_use_file(char* fname);
extern int yyparse(void);

extern int yydebug;
extern int optind;
extern ast_t* syntax_tree;
extern SymbolTable sem_symtab;

int errors;
int offset;
int warnings;
flags_t flags;

int main(int argc, char** argv) {
	char c;

	/* Set default values */
	flags.yydebug = 0;
	flags.symtab_debug = 0;
	flags.print_ast = 0;
	flags.print_aug_ast = 0;
	errors = 0;
	offset = 0;
	warnings = 0;

	initErrorProcessing();

	/* Read command line options */
	while ((c = getopt(argc, argv, (char*) "dDhpP")) != -1) {
		switch (c) {
			case 'd':
				flags.yydebug = 1;
				break;
			case 'D':
				flags.symtab_debug = 1;
				break;
			case 'h':
				fprintf(stdout, "Usage: %s [options] [file]\n\n", argv[0]);
				fprintf(stdout, "Options:\n");
				fprintf(stdout, "  -d\tEnable parser debugging traces\n");
				fprintf(stdout, "  -D\tEnable symbol table debugging traces\n");
				fprintf(stdout, "  -h\tPrint this help information and exit\n");
				fprintf(stdout, "  -p\tPrint syntax tree before semantic analysis\n");
				fprintf(stdout, "  -P\tPrint syntax tree after semantic analysis\n\n");
				fprintf(stdout, "If [file] is omitted then input is read from stdin.\n");
				exit(0);
				break;
			case 'p':
				flags.print_ast = 1;
				break;
			case 'P':
				flags.print_aug_ast = 1;
				break;
		}
	}

	/* Find input source */
	switch (argc - optind) {
		case 1:
			scanner_use_file(argv[optind]);
			break;
		case 0:
			/* read from STDIN */
			break;
	}

	if (flags.yydebug) yydebug = 1;
	if (flags.symtab_debug) sem_symtab.debug(true);

	yyparse();

	if (flags.print_ast) ast_print(syntax_tree, FALSE);
	if (errors) goto end;

	syntax_tree = sem_analysis(syntax_tree);

	if (flags.print_aug_ast) ast_print(syntax_tree, TRUE);

	end:
	fprintf(stdout, "Offset for end of global space: %i\n", offset);
	fprintf(stdout, "Number of warnings: %i\n", warnings);
	fprintf(stdout, "Number of errors: %i\n", errors);

	exit(0);
}
