#include <string.h>
#include <stdio.h>
#include "getopt.h"

/* This version of getopt() is a modification of the version supplied on the CS445 course
 * website.
 */

#if	M_I8086 || M_I286 || MSDOS
#define SWITCH '/'
#else
#define SWITCH '-'
#endif

static int sp = 1;
char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

static int badopt(char* name, char* text) {
	if (opterr) fprintf(stderr, "%s: %s -- %c\n", name, text, optopt);

	return (int) '?';
}

int getopt(int argc, char** argv, char* opts)
{
	char ch;
	char* cp;

	if (sp == 1) {
		if (argc <= optind || argv[optind][1] == '\0') return EOF;
		if ((ch = argv[optind][0]) != '-' && ch != SWITCH) return EOF;
		if (!strcmp(argv[optind], "--")) {
			++optind;
			return EOF;
		}
	}

	optopt = (int) (ch = argv[optind][sp]);

	if (ch == ':' || (cp = strchr(opts, ch)) == NULL) {
		if (argv[optind][++sp] == '\0') {
			++optind;
			sp = 1;
		}

		return badopt(argv[0], "illegal option");
	}

	if (*++cp == ':') {
		optarg = &argv[optind][sp + 1];
		++optind;
		sp = 1;

		if (*optarg == '\0') {	/* in next word */
			if (argc <= optind)	return badopt(argv[0], "option requires an argument");
			optarg = argv[optind++];	/* to next word */
		}
	} else {
		optarg = NULL;

		if (argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
	}

	return optopt;
}
