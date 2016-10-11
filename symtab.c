#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "symtab.h"

/* This version of symtab.c is a modification of the version supplied on the CS445 course
 * website.
 */

Scope::Scope(std::string newname) {
	name = newname;
	debugFlg = false;

	return;
}

Scope::~Scope() {
	return;
}

bool Scope::debugFlg;

void Scope::debug(bool state) {
	debugFlg = state;

	return;
}

void Scope::print(void (*printData)(void*)) {
	printf("Scope: %-15s -----------------\n", name.c_str());

	for (std::map<std::string , void*>::iterator it=symbols.begin(); it!=symbols.end(); it++) {
		printf("%20s: ", (it->first).c_str());
		printData(it->second);
		printf("\n");
	}

	return;
}

void Scope::applyToAll(void (*action)(std::string , void*)) {
	for (std::map<std::string , void*>::iterator it=symbols.begin(); it!=symbols.end(); it++) {
		action(it->first, it->second);
	}

	return;
}

bool Scope::insert(std::string sym, void* ptr) {
	if (symbols.find(sym) == symbols.end()) {
		if (debugFlg) {
			printf("Scope: insert in \"%s\" the symbol \"%s\".\n", name.c_str(), sym.c_str());
		}

		symbols[sym] = ptr;

		return true;
	} else {
		if (debugFlg) {
			printf("Scope: insert in \"%s\" the symbol \"%s\" but symbol already there!\n",
				name.c_str(), sym.c_str());
		}

		return false;
	}
}

void* Scope::lookup(std::string sym) {
	if (symbols.find(sym) != symbols.end()) {
		if (debugFlg) {
			printf("Scope: lookup in \"%s\" the symbol \"%s\" and found it.\n",
				name.c_str(), sym.c_str());
		}

		return symbols[sym];
	} else {
		if (debugFlg) {
			printf("Scope: lookup in \"%s\" the symbol \"%s\" and did NOT find it.\n",
				name.c_str(), sym.c_str());
		}

		return NULL;
	}
}

SymbolTable::SymbolTable() {
	enter((std::string )"Global");
	debugFlg = false;

	return;
}


void SymbolTable::debug(bool state) {
	debugFlg = state;

	return;
}

int SymbolTable::depth() {
	return stack.size();
}

void SymbolTable::print(void (*printData)(void*)) {
	printf("===========  Symbol Table  ===========\n");

	for (std::vector<Scope*>::iterator it=stack.begin(); it!=stack.end(); it++) {
		(*it)->print(printData);
	}

	printf("===========  ============  ===========\n");

	return;
}

void SymbolTable::applyToAllGlobal(void (*action)(std::string , void*)) {
	stack[0]->applyToAll(action);

	return;
}

void SymbolTable::enter(std::string name)					 {
	if (debugFlg) {
		printf("DEBUG(SymbolTable): enter scope \"%s\".\n", name.c_str());
	}

	stack.push_back(new Scope(name));

	return;
}

void SymbolTable::leave() {
	if (debugFlg) {
		printf("DEBUG(SymbolTable): leave scope \"%s\".\n",
			(stack.back()->scopeName()).c_str());
	}

	if (stack.size()>1) {
		delete stack.back();
		stack.pop_back();
	} else {
		printf("ERROR(SymbolTable): You cannot leave global scope.  Number of scopes: %d.\n",
			(int)stack.size());
	}

	return;
}

void*  SymbolTable::lookup(std::string sym) {
	void* data;

	for (std::vector<Scope*>::reverse_iterator it=stack.rbegin(); it!=stack.rend(); it++) {
		data = (*it)->lookup(sym);

		if (data!=NULL) {
			break;
		}
	}

	if (debugFlg) {
		printf("DEBUG(SymbolTable): lookup the symbol \"%s\" and %s.\n", sym.c_str(),
			(data ? (char*)"found it" : (char*)"did NOT find it"));
	}

	return data;
}

void*  SymbolTable::lookupGlobal(std::string sym) {
	void* data;

	data = stack[0]->lookup(sym);

	if (debugFlg) {
		printf("DEBUG(SymbolTable): lookup the symbol \"%s\" and %s.\n", sym.c_str(),
			(data ? "found it" : "did NOT find it"));
	}

	return data;
}

bool SymbolTable::insert(std::string sym, void* ptr) {
	if (debugFlg) {
		printf("DEBUG(SymbolTable): insert the symbol \"%s\".\n", sym.c_str());
	}

	return (stack.back())->insert(sym, ptr);
}

bool SymbolTable::insertGlobal(std::string sym, void* ptr) {
	if (debugFlg) {
		printf("DEBUG(SymbolTable): insert the global symbol \"%s\".\n", sym.c_str());
	}

	return stack[0]->insert(sym, ptr);
}
