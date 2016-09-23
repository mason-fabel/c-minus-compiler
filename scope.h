#ifndef _SCOPE_H_
#define _SCOPE_H_

void* scope_create(char* name);
void scope_debug(void* scope, int state);
void scope_print(void* scope, void (*printData)(void*));
int scope_insert(void* scope, char* sym, void* ptr);
void* scope_lookup(void* scope, char* sym);
const char* scope_scope_name(void* scope);

#endif /* _SCOPE_H_ */
