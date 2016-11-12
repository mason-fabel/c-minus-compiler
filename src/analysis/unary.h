#ifndef _ANALYSIS_UNARY_H_
#define _ANALYSIS_UNARY_H_

int unary_no_array(ast_t* node);
int unary_only_array(ast_t* node);
int unary_only_bool(ast_t* node);
int unary_only_int(ast_t* node);

#endif /* _ANALYSIS_UNARY_H_ */
