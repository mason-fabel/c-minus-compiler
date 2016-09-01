#ifndef _TOKEN_H_
#define _TOKEN_H_
typedef union {
	int int_val;
	char char_val;
	char* str_val;
} value_t;

typedef struct {
	int lineno;
	char* input;
	value_t value;
} token_t;

#endif /* _TOKEN_H_ */
