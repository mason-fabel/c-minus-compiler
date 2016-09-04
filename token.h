#ifndef _TOKEN_H_
#define _TOKEN_H_
typedef enum {
	MODE_NONE,
	MODE_INT,
	MODE_CHAR,
	MODE_STR
} value_mode_t;

typedef union {
	int int_val;
	char char_val;
	char* str_val;
} value_t;

typedef struct {
	int type;
	int lineno;
	char* input;
	value_mode_t value_mode;
	value_t value;
} token_t;

#endif /* _TOKEN_H_ */
