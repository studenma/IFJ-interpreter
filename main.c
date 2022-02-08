// Project: Implementace překladače imperativního jazyka IFJ17.
// Author: xzaple36 (Jakub Zapletal)
#include "includes.h"

nodePtr symtable;

int main(int argc, char** argv) {

	treeInit(&symtable);
	openOutput();
	parse();
	treeDispose(&symtable);

	error(FINE); // cleaning the memory after succesful run
	return FINE;
}	