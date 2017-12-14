# PL0Compiler
Compiler for the PL0 langauge developed in C for Systems Software course

===========================================================
Note: The following document contains instructions for 
compiling and running the compiler. Please following
these instructions to ensure the program runs properly.
===========================================================

Code Authors: Nicholas Espinosa and Paul Tucker

Purpose: This code is the PL0 Scanner as described in the
assignment documentation. For further details on the purpose
and potential use of this program, please refer to that document.

----------------------
IMPORTANT! PLEASE READ
----------------------
Your source code must be named "input1.txt" and follow the input 
restrictions as established in the assignment document provided
by Professor Euripides Montagne. 
You must also have the files “input2.txt” and “input3.txt.” The files must be in the same directory as main.c. Failure to do so will result
in the program failing to run as intended.

===========
Directions
===========

1. Connect to the Eustis server using your login credentials

2. Upload the folder onto your Eustis server allocation (see important section).

3. Compile the program by running the command “make”

4. Then run ./compile followed by any of the following: “-l”, “-a”, and/or “-v”

======
Output
======
The work done by lexical.c will be stored in “input2.txt.”
The work done by parse.c will be stored in “input3.txt.”
The output of the source code will be printed to the command prompt.
Any other output will be dependent upon whatever arguments are passed to compile.
