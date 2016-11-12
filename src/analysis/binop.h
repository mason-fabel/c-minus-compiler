#ifndef _ANALYSIS_BINOP_H_
#define _ANALYSIS_BINOP_H_

int binop_match_array(ast_t* node);
int binop_no_array(ast_t* node);
int binop_no_void(ast_t* node);
int binop_only_array(ast_t* node);
int binop_only_bool(ast_t* node);
int binop_only_char_or_int(ast_t* node);
int binop_only_int(ast_t* node);
int binop_same_type(ast_t* node);

#endif /* _ANALYSIS_BINOP_H_ */
