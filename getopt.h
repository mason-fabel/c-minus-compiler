#ifndef _GETOPT_H_
#define _GETOPT_H_

char *optarg;
int optind;
int opterr;
int optopt;
int getopt(int argc, char **argv, char *opts);

#endif /* _GETOPT_H_ */
