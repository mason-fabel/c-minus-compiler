#ifndef _ANALYSIS_ERROR_H_
#define _ANALYSIS_ERROR_H_

void error_lineno(ast_t* node);
void error_func_defined(ast_t* node);
void error_symbol_defined(ast_t* node);
void warning_lineno(ast_t* node);

#endif /* _ANALYSIS_ERROR_H_ */
