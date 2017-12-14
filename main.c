#include <stdio.h>
#include <string.h>
#include "compile.h"

void determineArguments(int argc, char **argv);
void runMachine(int l, int a, int v);

int main(int argc, char **argv)
{
	determineArguments(argc, argv);
	return 0;
}

void determineArguments(int argc, char **argv)
{
	int i, lFlag = 0, aFlag = 0, vFlag = 0;
	
	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-l") == 0)
			lFlag = 1;
		if(strcmp(argv[i], "-a") == 0)
			aFlag = 1;
		if(strcmp(argv[i], "-v") == 0)
			vFlag = 1;
	}
	
	runMachine(lFlag, aFlag, vFlag);
}

void runMachine(int l, int a, int v)
{
	// Run lexical analyzer
	performLexical(l);
	// Run the parser/code generator
	program(a);
	// Runs the virtual machine
	startVirtualMachine(v);
}