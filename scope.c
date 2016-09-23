#include <string>
#include "scope.h"
#include "symbolTable.h"

void* scope_create(char* name) {
	return (void*) new Scope(std::string(name));
}

void scope_debug(void* scope, int state) {
	((Scope*) scope)->debug((bool) state);

	return;
}

void scope_print(void* scope, void (*printData)(void*)) {
	((Scope*) scope)->print(printData);

	return;
}

int scope_insert(void* scope, char* sym, void* ptr) {
	return (int) ((Scope*) scope)->insert(std::string(sym), ptr);
}

void* scope_lookup(void* scope, char* sym) {
	return ((Scope*) scope)->lookup(std::string(sym));
}

const char* scope_scope_name(void* scope) {
	return (((Scope*) scope)->scopeName()).c_str();
}
