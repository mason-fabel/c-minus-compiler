#include <stdio.h>
#include <stdlib.h>
#include "../ast.h"
#include "call.h"
#include "error.h"

typedef enum {
	ERR_ARGS_LESS,
	ERR_ARGS_MORE,
	ERR_NONE,
	ERR_TYPE,
} param_error_type_t;

typedef struct {
	int index;
	param_error_type_t type;
	ast_t* call;
	ast_t* call_param;
	ast_t* def;
	ast_t* def_param;
} param_error_dat_t;

void param_error(param_error_dat_t err);
param_error_type_t param_check(ast_t* lhs, ast_t* rhs);

int call_params(ast_t* call, ast_t* def) {
	int index;
	int pass;
	param_error_dat_t err;
	param_error_type_t err_type;
	ast_t* call_param;
	ast_t* def_param;

	if (!call) return 0;
	if (!def) return 0;

	pass = 1;

	index = 1;
	call_param = call->child[0];
	def_param = def->child[0];

	while (call_param != NULL || def_param != NULL) {
		err_type = param_check(call_param, def_param);

		err.type = err_type;
		err.index = index;
		err.call = call;
		err.call_param = call_param;
		err.def = def;
		err.def_param = def_param;

		switch (err_type) {
			case ERR_ARGS_MORE:
				param_error(err);
				goto end_check;
				break;
			case ERR_ARGS_LESS:
				param_error(err);
				/* At this point, we have checked all the call parameters, but
				 * there are still definition parameters that have not been
				 * checked. Since we are validating the call, not the
				 * definition, we don't care to continue. Bail out of the param
				 * loop.
				 */
				goto end_check;
				break;
			case ERR_TYPE:
				param_error(err);
				break;
		}

		index++;
		if (call_param) call_param = call_param->sibling;
		if (def_param) def_param = def_param->sibling;
	}

	end_check:
	return pass;
}

param_error_type_t param_check(ast_t* lhs, ast_t* rhs) {
	if (!lhs) return ERR_ARGS_LESS;
	if (!rhs) return ERR_ARGS_MORE;
	if (lhs->data.type != TYPE_NONE) {
		if (lhs->data.type != rhs->data.type) return ERR_TYPE;
		if (lhs->data.is_array != rhs->data.is_array) return ERR_TYPE;
	}
	return ERR_NONE;
}

void param_error(param_error_dat_t err) {
	switch (err.type) {
		case ERR_ARGS_LESS:
			error_lineno(err.call);
			fprintf(stdout, "Too few parameters passed for function ");
			fprintf(stdout, "'%s' defined on line %i.\n",
				err.call->data.name, err.def->lineno);
			break;
		case ERR_ARGS_MORE:
			error_lineno(err.call);
			fprintf(stdout, "Too many parameters passed for function ");
			fprintf(stdout, "'%s' defined on line %i.\n",
				err.call->data.name, err.def->lineno);
			break;
		case ERR_TYPE:
			if (err.call_param->data.type != err.def_param->data.type) {
				error_lineno(err.call);
				fprintf(stdout, "Expecting %s in parameter %i of call to '%s' ",
					ast_type_string(err.def_param->data.type), err.index,
					err.call->data.name);
				fprintf(stdout, "defined on line %i but got %s.\n",
					err.def->lineno,
					ast_type_string(err.call_param->data.type));
			}

			if (err.call_param->data.is_array != err.def_param->data.is_array) {
				error_lineno(err.call);

				if (err.call_param->data.is_array) {
					fprintf(stdout, "Not expecting ");
				} else {
					fprintf(stdout, "Expecting ");
				}

				fprintf(stdout, "array in parameter %i of call to '%s' ",
					err.index, err.call->data.name);
				fprintf(stdout, "defined on line %i.\n", err.def->lineno);
			}

			break;
	}

	return;
}
