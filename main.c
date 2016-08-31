#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"

void yyerror(const char* str) {
	fprintf(stderr, "%s\n", str);
}

int main() {
	yyparse();

	exit(0);
}
