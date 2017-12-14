// Nicholas Espinosa and Paul Tucker
// COP 3402
// Parser/Code Generator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Struct to store symbol table values
typedef struct symbol
{ 
	int kind; 		// const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		// number (ASCII value) 
	int level; 		// L level
	int addr; 		// M address
} symbol; 

//Enum to simplify comparisons 
enum token_type{ 
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym};

// Enum to simplify calling operations
enum op_type{
LIT = 1, RTN, LOD, STO, CAL, INC, JMP, JPC, SIO, NEG, ADD,
SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ};

// Required functions
void program(int flag);
void block(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx);
void statement(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx);
void condition(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx);
void expression(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx);
void term(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx);
void factor(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx);

// Assistant functions
void enter(symbol symTable[], int *tx, int kind, char ident[], int value, int level, int address);
void emit(int code[][4], int *cx, int op, int rx, int lx, int m);
void getToken(FILE *ifp, int *token);
void getIdent(FILE *ifp, char ident[]);
int getNum(FILE *ifp);
int findIdent(FILE *ifp, symbol symTable[], int *tx);
int isRelation(int *token);
int isAdding(int *token);
int isMult(int *token);
int getRelation(int *token);
void writeCode(int code[][4], int *cx);
void printCode(int code[][4], int *cx);
void error(int error);

// Central program function
void program(int flag)
{
	// Getting the initial variables
	int token, cx = 0, rx = 0, tx = 0, lx = 0;
	int code[10000][4];
	symbol symTable[10000];
	
	// Getting the input file
	FILE *ifp = fopen("input2.txt", "r");
	
	// input file was not found
	if(ifp == NULL)
		error(14);
	
	// Obtains the first token
	getToken(ifp, &token);
	
	// Enters the block
	block(ifp, code, symTable, &token, &cx, &rx, &tx, lx);
	
	// Period must follow end symbol
	if(token != periodsym)
		error(19);
	
	// Emit the end operation
	emit(code, &cx, SIO, 0, 0, 3);
	
	// Close the file pointer
	fclose(ifp);

	// Writes code to a file
	writeCode(code, &cx);
	if(flag == 1)
	{
		// Prints out the generated code
		printCode(code, &cx);
		
		// Program was parsed without error
		printf("\nNo errors, program is syntactically correct\n\n");
	}
}

// Code for the block
void block(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx)
{
	int num, space = 4, stemp = *tx, jtemp = *cx;
	char ident[12];
	
	// Emit jump operation
	emit(code, cx, JMP, 0, 0, 0);
	
	// Found a constant
	if(*token == constsym)
	{
		do
		{
			// Get the next token
			getToken(ifp, token);
			
			// Constant must be followed by identifier
			if(*token != identsym)
				error(1);
			
			// Obtain the identifier
			getIdent(ifp, ident);
			
			// Gets the next token
			getToken(ifp, token);
			
			// Constant ident must be followed be an eqsym
			if(*token != eqsym)
				error(2);
			
			// Get the next token
			getToken(ifp, token);
			
			// Eqsym must be followed by a number
			if(*token != numbersym)
				error(3);
			
			// Obtains a number from the file
			num = getNum(ifp);
			
			//Constants require kind, name, and value only
			enter(symTable, tx, 1, ident, num, lx, space);
			
			// Obtains the next identifier
			getToken(ifp, token);
			
		} while(*token == commasym);
		
		// Constant declarations must be followed by a semicolon
		if(*token != semicolonsym)
			error(5);
		
		// Obtains the next token
		getToken(ifp, token);
	}
	// Found a variable
	if(*token == varsym)
	{	
		do
		{
			// Get the next token
			getToken(ifp, token);
			
			// Variable must be followed by identifier
			if(*token != identsym)
				error(1);
			
			// Obtain the identifier
			getIdent(ifp, ident);

			// Get the next token
			getToken(ifp, token);

			//void enter(symbol symTable[], int *tx, int kind, char ident[], int value, int level, int address);
			//Variables require kind, name, and value only
			enter(symTable, tx, 2, ident, 0, lx, space);
			
			// Increment the space for the current block
			space++;
			
		}while(*token == commasym);
		
		// Variable declarations must be followed by a semicolon
		if(*token != semicolonsym)
			error(4);
		
		// Obtains the next token
		getToken(ifp, token);
		
	}
	// Found a procedure
	if(*token == procsym)
	{
		while(*token == procsym)
		{
			// Get the next token
			getToken(ifp, token);
			
			// Procedure must be followed by an identifier
			if(*token != identsym)
				error(5);
			
			// Obtain the identifier
			getIdent(ifp, ident);
			
			/// Procedure require kind, name, level, and address
			enter(symTable, tx, 3, ident, 0, lx, *cx);
			
			// Get the next token
			getToken(ifp, token);
			
			// Procedure declaration must be followed by semicolon
			if(*token != semicolonsym)
				error(4);
			
			// Obtain the next token
			getToken(ifp, token);
			
			// Calls the block function
			block(ifp, code, symTable, token, cx, rx, tx, lx + 1);
			
			// No semicolon at the end of block
			if(*token != semicolonsym)
				error(6);
			
			// Get next token
			getToken(ifp, token);
		}
	}
	
	// Updating jump function
	code[jtemp][3] = *cx;
	
	// Emit increment operation
	emit(code, cx, INC, 0, 0, space);
	
	// Call statement function
	statement(ifp, code, symTable, token, cx, rx, tx, lx);
	
	// Emit return if above main level
	if(lx > 0)
		emit(code, cx, RTN, 0, 0, 0);
	
	// Deleting old values
	*tx = stemp;
}

void statement(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx)
{
	int ident, cx1, cx2;
	
	if(*token == identsym)
	{
		// Obtains the identifier
		ident = findIdent(ifp, symTable, tx);
		
		// Get next token
		getToken(ifp, token);
		
		// Missing become symbol
		if(*token != becomessym)
			error(7);
		
		// Get next token
		getToken(ifp, token);
		
		// Call the expression function
		expression(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Emit the store operation
		emit(code, cx, STO, *rx, lx - symTable[ident].level, symTable[ident].addr);
	}
	else if(*token == callsym)
	{
		// Get next token
		getToken(ifp, token);
		
		// Call must be followed by identifier
		if(*token != identsym)
			error(17);
		
		// Obtains the identifier
		ident = findIdent(ifp, symTable, tx);
		
		// Missing Identifier
		if(ident == -1)
			error(1);
		
		//printf("Current level before call is: %d\n", lx);
		
		// Either identifier is of the right kind or not
		if(symTable[ident].kind == 3)
			emit(code, cx, CAL, 0, lx - symTable[ident].level, symTable[ident].addr);
		else
			error(18);
		
		// Get next token
		getToken(ifp, token);
	}
	else if(*token ==  beginsym)
	{
		// Get next token
		getToken(ifp, token);
		
		// Call statement function
		statement(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// While there is a semicolon
		while(*token ==  semicolonsym)
		{
			// Get the next token
			getToken(ifp, token);
			
			// Call the statement function
			statement(ifp, code, symTable, token, cx, rx, tx, lx);
		}
		
		// Begin must be closed with end
		if(*token != endsym)
			error(8);
		
		// Get next token
		getToken(ifp, token);
		
	}
	else if(*token == ifsym)
	{
		// Get next token
		getToken(ifp, token);
		
		// Condition function called
		condition(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// If condition must be followed by then
		if(*token != thensym)
			error(9);
		
		// Get the next token
		getToken(ifp, token);
		
		// Set ctemp to current code index
		cx1 = *cx;
		
		// Emit the JPC operation
		emit(code, cx, JPC, *rx, 0, 0);
		
		// Call statement function
		statement(ifp, code, symTable, token, cx, rx, tx, lx);
		
		code[cx1][3] = *cx;
		
		if(*token == elsesym)
		{
			// Storing the current code location
			cx2 = *cx;
			
			// Emits the JMP operation
			emit(code, cx, JMP, 0, 0, 0);
			
			code[cx1][3] = *cx;
			
			// Get next token
			getToken(ifp, token);
			
			// Call statement function
			statement(ifp, code, symTable, token, cx, rx, tx, lx);
			
			// Updating the jump function
			code[cx2][3] = *cx;
		}
	}
	else if(*token ==  whilesym)
	{
		// Storing the current code location
		cx1 = *cx;
		
		// Get the next token
		getToken(ifp, token);
		
		// Condition function called
		condition(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Storing the current code location
		cx2 = *cx;
		
		// Jump to given position upon condition
		emit(code, cx, JPC, *rx, 0, cx2);
		
		// While condition must be followed by do
		if(*token != dosym)
			error(10);
		
		// Get next token
		getToken(ifp, token);
		
		// Call the statement function
		statement(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Jump to given position 
		emit(code, cx, JMP, 0, 0, cx1);
		
		// Jump to given position 
		emit(code, cx, JMP, 0, 0, cx1);
		
		// Updating the JPC function
		code[cx2][3] = *cx;
		
	}
	else if(*token == readsym)
	{
		// Get the next token
		getToken(ifp, token);
		
		// Missing identifier
		if(*token != identsym)
			error(1);
		
		// Obtains the identifier
		ident = findIdent(ifp, symTable, tx);
		
		// Get the next token
		getToken(ifp, token);
		
		// Emits the read operation
		emit(code, cx, SIO, *rx, 0, 2);
		
		// Store the value into the variable
		emit(code, cx, STO, *rx, lx - symTable[ident].level, symTable[ident].addr);
	}
	else if(*token == writesym)
	{
		// Get the next token
		getToken(ifp, token);		

		// Call the expression function
		expression(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Emits the write operation
		emit(code, cx, SIO, *rx, 0, 1);
	}
	
}
void condition(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx)
{
	int relop;
	
	if(*token == oddsym)
	{
		// Get next token
		getToken(ifp, token);
		
		// Call expression function 
		expression(ifp, code, symTable, token, cx, rx, tx, lx);
	}
	else
	{	
		// Call expression function
		expression(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Determines if a relational operator is present
		if(isRelation(token) == 0)
			error(11);
		
		// Increment the register index
		*rx = *rx + 1;
		
		// Obtain the relational operator
		relop = getRelation(token);
		
		// Get next token
		getToken(ifp, token);
		
		// Call expression function
		expression(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Emit conditional operation
		emit(code, cx, relop, *rx - 1, *rx - 1, *rx);
		
		// Decrement register index
		*rx = *rx - 1;
	}
}

// Expression function
void expression(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx)
{
	int addop;
	
	// If adding term, then get next token
	if(isAdding(token) == 1)
	{
		// Setting addop to next token
		addop = *token;
		
		// Get next token
		getToken(ifp, token);
		
		// Call the term function
		term(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// If the addop is a minussym, emit negate operation
		if(addop == minussym)
			emit(code, cx, NEG, *rx - 1, *rx, 0);
	}
	else// Otherwise, call the term function
		term(ifp, code, symTable, token, cx, rx, tx, lx);
	
	while(isAdding(token) == 1)
	{
		// Increment the register index
		*rx = *rx + 1;
		
		// Setting addop to current token
		addop = *token;
		
		// Get next token
		getToken(ifp, token);
		
		// Call the term function
		term(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Determines if a plus or minus operation is performed
		if(addop == plussym)
			emit(code, cx, ADD, *rx - 1, *rx - 1, *rx);
		else
			emit(code, cx, SUB, *rx - 1, *rx - 1, *rx);
		
		// Decrement register index
		*rx = *rx - 1;
	}
}

// Term function
void term(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx)
{
	int mulop;
	
	// Call factor function
	factor(ifp, code, symTable, token, cx, rx, tx, lx);
	
	while(isMult(token) == 1)
	{
		// Increment the register index
		*rx = *rx + 1;
		
		// Set multop to current token
		mulop = *token;
		
		// Get next token
		getToken(ifp, token);
		
		// Call the factor function
		factor(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Determines if a mult or div operation is performed
		if(mulop == multsym)
			emit(code, cx, MUL, *rx - 1, *rx - 1, *rx);
		else
			emit(code, cx, DIV, *rx - 1, *rx - 1, *rx);
		
		// Decrement register index
		*rx = *rx - 1;
	}
}

// Factor function
void factor(FILE *ifp, int code[][4], symbol symTable[], int *token, int *cx, int *rx, int *tx, int lx)
{
	int num, ident;
	
	if(*token == identsym)
	{
		// Get the identifier
		ident = findIdent(ifp, symTable, tx);
		
		if(symTable[ident].kind == 2) // Load a variable into the register
			emit(code, cx, LOD, *rx, lx - symTable[ident].level, symTable[ident].addr);
		else if(symTable[ident].kind == 1) // Load a literal into the register
			emit(code, cx, LIT, *rx, 0, symTable[ident].val);
		else
			error(16);
		
		// Get next token
		getToken(ifp, token);
	}
	else if(*token == numbersym)
	{
		// Get the number
		num = getNum(ifp);
		
		// Load a literal into the register
		emit(code, cx, LIT, *rx, 0, num);
		
		// Get next token
		getToken(ifp, token);
	}
	else if(*token == lparentsym)
	{
		// Get next token
		getToken(ifp, token);
		
		// Call the expression function
		expression(ifp, code, symTable, token, cx, rx, tx, lx);
		
		// Left ( has not been closed
		if(*token != rparentsym)
			error(12);
		
		// Get the next function
		getToken(ifp, token);
	}
	else // Improper token found
		error(13);
}


// Enters a symbol into the symbol table
void enter(symbol symTable[], int *tx, int kind, char ident[], int value, int level, int address)
{
	// Operations that are the same for all symbols
	symTable[*tx].kind = kind;
	strcpy(symTable[*tx].name, ident);
	symTable[*tx].level = level;
	
	if(kind == 1) // Constant added
		symTable[*tx].val = value;
	else if(kind == 2 ||  kind == 3) // Variable or procedure added 
		symTable[*tx].addr = address;
	
	// Increment the table index
	*tx = *tx + 1;
}

// Enters operations into code table
void emit(int code[][4], int *cx, int op, int rx, int lx, int m)
{
	if(*cx >= 10000)
		error(15);
	else
	{
		code[*cx][0] = op;
		code[*cx][1] = rx;
		code[*cx][2] = lx;
		code[*cx][3] = m;
		*cx = *cx + 1;
	}
}

// Obtains a token from the input file
void getToken(FILE *ifp, int *token)
{
	fscanf(ifp, "%d ", token);
}

// Obtains the identifier to be placed into symTable
void getIdent(FILE *ifp, char ident[])
{
	int i = 0;
	char c, word[12];
	
	// Continue until broken
    while(1)
    {
		// Obtain next letter
        c = fgetc(ifp);
		
		// If the identifier name is too long
        if(!isspace(c) && i == 11)
        {
            error(21);
        }
		
		// Not reading in whitespace
        if(!isspace(c))
        {
            word[i++] = c;
            word[i] = '\0';
        }
        else // Otherwise, exit the loop
            break;
    }
	
	// Copy the identifier name into the symTable
	strcpy(ident, word);
}

// Obtains a number from the file
int getNum(FILE *ifp)
{
	int i = 0;
	char c, word[6];
	
	while(1)
    {
        c = fgetc(ifp);
		
		// If the number is too long
        if(!isspace(c) && i == 6)
            error(22);
		
		// Not reading in whitespace
        if(!isspace(c))
        {
            word[i++] = c;
            word[i] = '\0';
        }
        else // Otherwise, exit the loop
            break;
    }
	
	// Returns the number value
	return atoi(word);
}

// Determines if the identifier is in the symTable
int findIdent(FILE *ifp, symbol symTable[], int *tx)
{
	// Necessary variables
	int i = 0;
	char c, word[12];
	
	// Reads in the identifier name
	while(1)
    {
        c = fgetc(ifp);
        if(!isspace(c) && i == 11)
        {
            error(21);
        }
        if(!isspace(c))
        {
            word[i++] = c;
            word[i] = '\0';
        }
        else
            break;
    }
	
	// If the identifier exists, return the location
	for(i = *tx - 1; i >= 0; i--)
	{
		if(strcmp(word, symTable[i].name) == 0)
			return i;	
	}
	return -1;
}

// Determines if token is a relation
int isRelation(int *token)
{
	// Returns 1 if true
    if(*token == eqsym || *token == neqsym || *token == lessym || 
		*token == leqsym || *token == gtrsym || *token == geqsym)
        return 1;
    
	// Returns 0 if false
    return 0;
}

int isAdding(int *token)
{
	if(*token == plussym || *token == minussym)
		return 1;
	
	return 0;
}

int isMult(int *token)
{
	if(*token == multsym || *token == slashsym)
		return 1;
	
	return 0;
}

// Gets the relational operator
int getRelation(int *token)
{
	int function;
	
	switch(*token)
	{
		case eqsym:
			function = 17;
			break;
		case neqsym:
			function = 18;
			break;
		case lessym:
			function = 19;
			break;
		case leqsym:
			function = 20;
			break;
		case gtrsym:
			function = 21;
			break;
		case geqsym:
			function = 22;
			break;
	}
	
	return function;
}

// Write code to output file
void writeCode(int code[][4], int *cx)
{
	int i;
	FILE *ofp = fopen("input3.txt", "w");
	
	// File not found error
	if(ofp == NULL)
		error(20);
	
	// Print all code
	for(i = 0; i < *cx; i++)
	{
		if(i == *cx - 1)
			fprintf(ofp, "%d %d %d %d", code[i][0],code[i][1],code[i][2],code[i][3]);
		else
			fprintf(ofp, "%d %d %d %d\n", code[i][0],code[i][1],code[i][2],code[i][3]);
	}
	
	// Close output file
	fclose(ofp);
}

// Prints all generated code
void printCode(int code[][4], int *cx)
{	
	int i;
	
	// For formatting purposes
	printf("\n");
	
	for(i = 0; i < *cx; i++)
		printf("%d %d %d %d\n", code[i][0],code[i][1],code[i][2],code[i][3]);
}

// Displays Error Messages
void error(int error)
{
    printf("Error: "); 
    switch(error)
    {
		case 1:
			printf("Missing identifier.\n");
			break;
		case 2:
			printf("Identifier should be followed by =.\n");
			break;
		case 3:
			printf("= should be followed by a number.\n");
			break;
		case 4:
			printf("Declaration must end with ;.\n");
			break;
		case 5:
			printf("Missing procedure declaration.\n");
			break;
		case 6:
			printf("No ; at the end of block.\n");
			break;
		case 7:
			printf(":= missing in statement.\n");
			break;
		case 8:
			printf("Begin must be close with end.\n");
			break;
		case 9:
			printf("If condition must be followed by then.\n");
			break;
		case 10:
			printf("While condition must be followed by do.\n");
			break;
		case 11:
			printf("Relational operator missing in conditional.\n");
			break;
		case 12:
			printf("Left ( has not been closed.\n");
			break;
		case 13:
			printf("Identifier, (, or number expected.\n");
			break;
		case 14:
			printf("Input file not found.\n");
			break;
		case 15:
			printf("Exceeded code limit.\n");
			break;
		case 16:
			printf("Identifier of incorrect type.\n");
			break;
		case 17:
			printf("Call must be followed by identifier.\n");
			break;
		case 18:
			printf("Call of a constant or variable is meaningless.\n");
			break;
		case 19:
			printf("Program must end with a period.\n");
			break;
		case 20:
			printf("Output file not found.\n");
			break;
		case 21:
			printf("Identifier too long.\n");
			break;
		case 22:
			printf("Number too long.\n");
			break;
		default:
			printf("Unknown Error.\n");
			break;
    }
	// Shows that program exited upon error
    exit(1);
}