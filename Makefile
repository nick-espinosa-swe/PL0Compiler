compile: main.c lexical.c parse.c pm0vm.c compile.h 
	gcc -o compile main.c lexical.c parse.c pm0vm.c compile.h