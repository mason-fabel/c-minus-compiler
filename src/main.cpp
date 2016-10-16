#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "flags.h"
#include "getopt.h"
#include "print_tree.h"
#include "semantic.h"
#include "symtab.h"

#define FALSE 0
#define TRUE 1

extern void scanner_use_file(char* fname);
extern int yyparse(void);

extern int yydebug;
extern int optind;
extern ast_t* syntax_tree;
extern SymbolTable sem_symtab;

int warnings;
int errors;
flags_t flags;

int main(int argc, char** argv) {
	char c;

	/* Set default values */
	flags.yydebug = 0;
	flags.symtab_debug = 0;
	flags.print_ast = 0;
	flags.print_aug_ast = 0;
	warnings = 0;
	errors = 0;


	/* Read command line options */
	while ((c = getopt(argc, argv, (char*) "dDpP")) != -1) {
		switch (c) {
			case 'd':
				flags.yydebug = 1;
				break;
			case 'D':
				flags.symtab_debug = 1;
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

	sem_analysis(syntax_tree);

	if (flags.print_aug_ast) ast_print(syntax_tree, TRUE);

	fprintf(stdout, "Number of warnings: %i\n", warnings);
	fprintf(stdout, "Number of errors: %i\n", errors);

	exit(0);
}
