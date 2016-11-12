#ifndef _ANALYSIS_RETURN_H_
#define _ANALYSIS_RETURN_H_

int return_exists(ast_t* node);
int return_match_type(ast_t* node, ast_t* def);
int return_no_array(ast_t* node);

#endif /* _ANALYSIS_RETURN_H_ */
