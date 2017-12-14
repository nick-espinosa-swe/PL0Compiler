// Nicholas Espinosa and Paul Tucker
// COP 3402 - 0001
// Homework 1

// Necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct for input format
// Based upon struct from assignment documentation
typedef struct{
    int op;
    int r;
    int l;
    int m;
} instruction;

void startVirtualMachine(int flag);
int getInput(instruction ir[]);
void performOperations(instruction ir[], int numInstructions, int flag);
void printOperations(instruction ir[], int numInstructions);
void initializeArrays(int stack[], int reg[], int lexMark[]);
void runCode(instruction ir[], int stack[], int reg[], int lexMark[], int flag);
int cleanStack(int stack[], int start, int end);
int base(int l, int base, int stack[]);

void startVirtualMachine(int flag)
{
	// Necessary basic variables
    int numInstructions;
    instruction ir[500];

    // Obtains the number of instructions
    numInstructions = getInput(ir);

	if(flag == 2)
		printOperations(ir, numInstructions);
	
    // Performs the instructions obtained
    performOperations(ir, numInstructions, flag);
	
}

// Obtains the input for the program
int getInput(instruction ir[])
{
    // Initializing necessary variables
    int i = 0;
    FILE *ifp = fopen("input3.txt", "r");

    if(ifp == NULL)
    {
        printf("ERROR: File not found.\n");
        exit(0);
    }

    //Initializing instruction array
    for(int j = 0; j < 500; j++)
    {
        ir[j].op = 0;
        ir[j].r = 0;
        ir[j].l = 0;
        ir[j].m = 0;
    }

    // Reads in instructions until end of file reached
    while(!feof(ifp))
    {
        fscanf(ifp, "%d", &ir[i].op);
        fscanf(ifp, "%d", &ir[i].r);
        fscanf(ifp, "%d", &ir[i].l);
        fscanf(ifp, "%d", &ir[i].m);
        i++;
    }

    // Closing file
    fclose(ifp);

    // Returning the total number of instructions
    return i;
}

// Overall function responsible with performing the necessary operations
void performOperations(instruction ir[], int numInstructions, int flag)
{
    int stack[2000], reg[8], lexMark[3];

    // Prints out the operations within the program
	if(flag == 1)
		printOperations(ir, numInstructions);

    // Initializes all the arrays that are to be used
    initializeArrays(stack, reg, lexMark);

    // Runs the program
    runCode(ir, stack, reg, lexMark, flag);
}

// Prints out the operations that are to be performed
void printOperations(instruction ir[], int numInstructions)
{
    // Initializing necessary variable
    int i;
    char opNames[22][4] ={"lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "neg", "add", "sub", "mul", "div", "odd", "mod", "eql", "neq", "lss", "leq", "gtr", "geq"};

    // Formatting the printout
    printf("Factorial Op Printout:\n");

    // Prints the instructions that will be performed
    for(i = 0; i < numInstructions; i++)
    {
        printf("%d %s %d %d %d\n", i, opNames[ir[i].op - 1], ir[i].r, ir[i].l, ir[i].m);
    }

    // Simply for formatting purposes
    printf("\n");
}

// Sets the initial values of each element in arrays to specific values
void initializeArrays(int stack[], int reg[], int lexMark[])
{
    // All stack values to zero
    for(int i = 0; i < 2000; i++)
        stack[i] = 0;

    // All register values to zero
    for(int i = 0; i < 8; i++)
        reg[i] = 0;

    // Lexigraphical level markers to -1
    for(int i = 0; i < 3; i++)
        lexMark[i] = -1;
}

// Runs the code given
void runCode(instruction ir[], int stack[], int reg[], int lexMark[], int flag)
{
    // Necessary variable
    int halt = 0, bp = 1, sp = 0, marker = 0, i, j, regFlag = 0, oldSP = -1;
    int pc = 0;
    char opNames[22][4] ={"lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "neg", "add", "sub", "mul", "div", "odd", "mod", "eql", "neq", "lss", "leq", "gtr", "geq"};

    // Initial formatting
	if(flag == 1)
	{
		printf("Factorial Stack Trace:\n");
		printf("Initial Values \t\t\t\tpc\tbp\tsp\n");
	}
	if(flag == 2)
	{
		return;
	}

    // Runs the program while the halt flag is set to false
    while(halt == 0)
    {
		if(flag == 1)
			printf("%d\t%s\t%d\t%d\t%d", pc, opNames[ir[pc].op - 1], ir[pc].r, ir[pc].l, ir[pc].m);
        
		switch(ir[pc].op)
        {
            case 1: // Load a literal onto a given register
                reg[ir[pc].r] = ir[pc].m;
                pc++;
                break;
            case 2: // Returns from function
                oldSP = sp;// Set a flag to remove old values
                sp = bp - 1;
                bp = stack[sp + 3];
                pc = stack[sp + 4];
                break;
            case 3: // Load value onto register from stack offset M from L levels down
                reg[ir[pc].r] = stack[base(ir[pc].l, bp, stack) + ir[pc].m];
                pc++;
                break;
            case 4: // Store value in stack offset M from L levels down from register R
                stack[base(ir[pc].l, bp, stack) + ir[pc].m] = reg[ir[pc].r];
                pc++;
                break;
            case 5: // Call procedure at code index M
                stack[sp + 1] = 0;
                stack[sp + 2] = base(ir[pc].l, bp, stack);
                stack[sp + 3] = bp;
                stack[sp + 4] = pc + 1;
                bp = sp + 1;
                pc = ir[pc].m;
                lexMark[marker] = sp;
                marker++;
                break;
            case 6: // Increment the stack by M locals
                sp = sp + ir[pc].m;
                pc++;
                break;
            case 7: // Jump to instruction M
                pc = ir[pc].m;
                break;
            case 8: // Jump to instruction M if register R == 0
                pc = (reg[ir[pc].r] == 0) ? ir[pc].m : pc + 1;
                break;
            case 9: // Multiple possible options
                if(ir[pc].m == 1) // Flags to print out a given register
                {
                    regFlag = 1;
                    pc++;
                }
                else if(ir[pc].m == 2) // Reads in a value to a register from user
                {
                    scanf("%d", &reg[ir[pc].r]);
                    pc++;
                }
                else if(ir[pc].m == 3) // Begins termination process
                {
                    halt = 1;
                    pc = 0;
                    bp = 1;
                    sp = 0;
                }
                break;
            case 10: // Put the negative of one register into another
                reg[ir[pc].r] = -1 * reg[ir[pc].l];
                pc++;
                break;
            case 11: // Store the value of the addition of two registers
                reg[ir[pc].r] = reg[ir[pc].l] + reg[ir[pc].m];
                pc++;
                break;
            case 12: // Store the value of the subtraction of two registers
                reg[ir[pc].r] = reg[ir[pc].l] - reg[ir[pc].m];
                pc++;
                break;
            case 13: // Store the value of the multiplication of two registers
                reg[ir[pc].r] = reg[ir[pc].l] * reg[ir[pc].m];
                pc++;
                break;
            case 14: // Store the value of the division of two registers
                reg[ir[pc].r] = reg[ir[pc].l] / reg[ir[pc].m];
                pc++;
                break;
            case 15: // Determine if a register is odd
                reg[ir[pc].r] = reg[ir[pc].r] % 2;
                pc++;
                break;
            case 16: // Store the value of the modulus of two registers
                reg[ir[pc].r] = reg[ir[pc].l] % reg[ir[pc].m];
                pc++;
                break;
            case 17: // Determine if two registers are equal
                reg[ir[pc].r] = (reg[ir[pc].l] == reg[ir[pc].m]);
                pc++;
                break;
            case 18: // Determine if two registers are not equal
                reg[ir[pc].r] = (reg[ir[pc].l] != reg[ir[pc].m]);
                pc++;
                break;
            case 19: // Determine if one register is less than another
                reg[ir[pc].r] = (reg[ir[pc].l] < reg[ir[pc].m]);
                pc++;
                break;
            case 20: // Determine if one register is less or equal to another
                reg[ir[pc].r] = (reg[ir[pc].l] <= reg[ir[pc].m]);
                pc++;
                break;
            case 21: // Determine if one register is greater than another
                reg[ir[pc].r] = (reg[ir[pc].l] > reg[ir[pc].m]);
                pc++;
                break;
            case 22: // Determine if one register is greater than or equal to another
                reg[ir[pc].r] = (reg[ir[pc].l] >= reg[ir[pc].m]);
                pc++;
                break;
            default: // An invalid operation has been entered
                printf("Error: Invalid Operation");
                halt = 1;
                break;
        }
		
		// Upon return statement, clear out previous activation record
		if(oldSP != -1)
			oldSP = cleanStack(stack, sp+1, oldSP);

        // Prints out the program counter, base pointer, and stack pointer
		if(flag == 1)
		{
			printf("\t%d\t%d\t%d\t", pc, bp, sp);

			// Formats the printout of the stack if not halted
			j = 0;
			for(i = 1; i <= sp; i++)
			{
				// Formats the output of the values in the stack
				i == sp ? printf("%d\n", stack[i]) : printf("%d ", stack[i]);

				// Places markers to separate lexigraphical levels
				if(lexMark[j] == i && lexMark[j] != sp)
				{
					printf("| ");
					j++;
				}
			}
		

			//Prints out the specific register if flag is set
			if(regFlag == 1)
			{
				printf("%d\n", reg[ir[pc-1].r]);
				regFlag = 0;
			}

			// Prints the empty stack once program is halted
			if(halt == 1)
				printf("%d\n",stack[1]);

			// Prints out the register at the given point in time
			printf("RF: ");
			for(i = 0; i < 8; i++)
				i == 7 ? printf("%d\n", reg[i]) : printf("%d, ", reg[i]);
		}
    }
    // Formats the end of the printout
    printf("\nOutput: %d\n", reg[0]);
}

// Returns the base pointer of a specific lexigraphical level
// Based on function from assignment documentation
int base(int l, int base, int stack[])
{
    int b1 = base;

    while(l > 0)
	{
        b1 = stack[b1 + l];
		l = l - 1;
	}

    return b1;
}

// Clears out the values from the previous activation record
int cleanStack(int stack[], int start, int end)
{
    int i;
    for(i = start; i <= end; i++)
        stack[i] = 0;

    return -1;
}






