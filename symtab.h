#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <map>
#include <string>
#include <vector>

class Scope {
	private:
		static bool debugFlg;
		std::string name;
		std::map<std::string , void*> symbols;
	public:
		Scope(std::string newname);
		~Scope();
		void debug(bool state);
		void print(void (*printData)(void*));
		void applyToAll(void (*action)(std::string , void*));
		bool insert(std::string sym, void* ptr);
		void* lookup(std::string sym);
		std::string scopeName() { return name; };
};


class SymbolTable {
	private:
		std::vector<Scope*> stack;
		bool debugFlg;
	public:
		SymbolTable();
		void debug(bool state);
		int depth();
		void print(void (*printData)(void*));
		void applyToAllGlobal(void (*action)(std::string , void*));
		void enter(std::string name);
		void leave();
		void* lookup(std::string sym);
		void* lookupGlobal(std::string sym);
		bool insert(std::string sym, void* ptr);
		bool insertGlobal(std::string sym, void* ptr);
};

#endif /* _SYMTAB_H_ */
