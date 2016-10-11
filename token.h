#ifndef _TOKEN_H_
#define _TOKEN_H_

enum _value_mode {
	MODE_NONE,
	MODE_INT,
	MODE_CHAR,
	MODE_STR
} ;
typedef enum _value_mode value_mode_t;

union _value {
	int int_val;
	char char_val;
	char* str_val;
};
typedef union _value value_t;

struct _token {
	int type;
	int lineno;
	char* input;
	value_mode_t value_mode;
	value_t value;
};
typedef struct _token token_t;

#endif /* _TOKEN_H_ */
