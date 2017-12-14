// Nicholas Espinosa and Paul Tucker
// COP 3402 - 0001
// Lexical Analyzer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum token_type{ 
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym};

// Struct node to carry input information
typedef struct node
{
    int token, error;
    char *word;
    struct node* next;
}node;

// Functions Used
void performLexical(int flag);
node* readInput(int flag);
node* createNode();
void buildNode(node* current, char input[], int *length);
char* addWord(char input[], int *length);
int isSymbol(char c);
int isSingleSymbol(char c);
int isInvalidSymbol(char c);
void printLexemeTable(node* head);
char* printError(int error);
void printLexemeList(node* head);
int lexeme(char word[], int size);
void printListToFile(node *head);
node* clearList(node* head);

// Runs the program
void performLexical(int flag)
{
	// Creates the linked list
	node* head;
	
    // Reads the input of the given file and gains the linked list
    head = readInput(flag);
    
	// Prints out the Lexeme Table and List
	if(flag == 1)
	{
		printLexemeTable(head);
		printLexemeList(head);
	}
	
	//Writes list to file
	printListToFile(head);
    
    // Frees the data within the linked list
    head = clearList(head);
}

// Obtains input from file and allocates data to a linked list
node* readInput(int flag)
{
    int count = 0, comment = 0;
    char c, word[250];
    node* head = createNode(), *current;
    FILE *ifp = fopen("input1.txt", "r");
    
    // Exits the program if file is not found
    if(ifp == NULL)
    {
        printf("Error: Input File Not Found.\n");
        exit(1);
    }
    // Setting the follower at the start of the linked list
    current = head;
    
    // Formatting source code output
	printf("Source Code:\n");
    
	c = fgetc(ifp);
	
	printf("%c", c);
    
    
    // Reads until the end of the file
	while(!feof(ifp))
    { 
        // If the character is a letter
        if(isalpha(c))
        {            
            // Create a word until a dead end is reached
            do 
            {
                // Builds up the word
                word[count++] = c;
                word[count] = '\0';
                c = fgetc(ifp);
                
                // Prevents the EOF symbol from printing
                if(c != EOF)
                    printf("%c", c);
            }while(isalpha(c) || isdigit(c));
            
            // Allocate the results to a node in a linked list
            if(count > 12)
            {
                current->error = 3;
                current->token = nulsym;
            }
            
            buildNode(current, word, &count);
            current = current->next;
        }
        // If the character is a number
        if(isdigit(c))
        {
            // Continue until no longer a number
            do 
            {
                // Identifier not starting with letter
                if(isalpha(c))
                {
                    current->token = nulsym;
                    current->error = 1;
                }
                
                // Builds up the word
                word[count++] = c;
                word[count] = '\0';
                c = fgetc(ifp);
                
                // Prevents the EOF symbol from printing
                if(c != EOF)
                    printf("%c", c);
            }while(isalnum(c));
            // If no errors found in creation
            if(current->token == 0)
            {
                if(count >=5) // If the number is too long, flag for error
                    current->error = 2;
                    current->token = nulsym;
                    
                current->token = numbersym;
            }
               
            buildNode(current, word, &count);
            current = current->next;
        }
        // If a valid symbol is detected
        if(isSymbol(c))
        {
            // Continue until no longer a valid symbol
            do 
            {
                // Builds up the word
                word[count++] = c;
                word[count] = '\0';
                c = fgetc(ifp);
                
                // Prevents the EOF symbol from printing
                if(c != EOF) 
                    printf("%c", c);
                
                // If the symbol only appears by itself
                if(isSingleSymbol(word[0]))
                    break;
                
                // Finds a comment comments
                if(count == 2 && strcmp("/*", word) == 0)
                { 
                    // Sets a flag so word is not added to linked list 
                    comment = 1;
                    
                    // Until the end of the comment is reached
                    do 
                    {
                        // Builds up the word
                        word[count++] = c;
                        word[count] = '\0';
                        c = fgetc(ifp);
                        
                        // Prevents the EOF symbol from printing
                        if(c != EOF)
                            printf("%c", c);
                        
                        // Allows for infinite comments
                        if(count == 249)
                        {
                            if(word[248] == '*')
                            {
                                word[0] = '*';
                                count = 1;
                            }
                            else
                                count = 0;
                        }
                    }while(strstr(word, "*/") == NULL);
                    // Erase the comment
                    count = 0;
                    word[count] = '\0';
                }
            }while(isSymbol(c));
            
            // Allocate the results to a node in a linked list if not a comment
            if(comment == 0)
            {
                buildNode(current, word, &count);
                current = current->next;
            }
            // Resets the comment flag
            comment = 0;
        }
        
        // If the character is whitespace
        if(isspace(c))
        {
            do
            {
                // Continues to grab characters
                c = fgetc(ifp);
                
                // Prevents the EOF symbol from printing
                if(c != EOF)
                    printf("%c", c);
            }while(isspace(c));
        }
        // If an invalid symbol is found
        if(isInvalidSymbol(c))  
        {
            do
            {
                // Builds up the word
                word[count++] = c;
                word[count] = '\0';
                c = fgetc(ifp);
                
                // Prevents the EOF symbol from printing
                if(c != EOF && flag == 1)
                    printf("%c", c);
            }while(isInvalidSymbol(c));
            // Sets the error and token
            current->token = nulsym;
            current->error = 4;
            
            // Continues the process
            buildNode(current, word, &count);
            current = current->next;
        }
    }
    // For Formatting reasons
	printf("\n");
    
    return head;
}

// Creates a new node when called
node* createNode()
{
    node* newNode = (node*)malloc(sizeof(node));
    if(newNode == NULL)
    {
        printf("Error: Failed to create new node");
        exit(0);
    }
    newNode->error = 0;
    
    return newNode;
}

// Builds the node we wish to create
void buildNode(node* current, char input[], int *length)
{
    current->word = addWord(input, length);
    
    if(current->token == 0)
        current->token = lexeme(input, *length);
    
    if(current->token != 19)
        current->next = createNode();
    
    input[0] = '\0';
    *length = 0;
}

// Creates a string of dynamic length
char* addWord(char input[], int *length)
{
   char* word = malloc(sizeof(char) * (*length));
   if(input[*length] == EOF)
       input[*length] = '\0';
   
    stpcpy(word, input);
    return word;
}

// Runs through the lexeme table
int lexeme(char word[], int size)
{
    // Checks to see if it matches any reserved symbols or words
    if(strcmp("+", word) == 0)
        return plussym;
    if(strcmp("-", word) == 0)
        return minussym;
    if(strcmp("*", word) == 0)
        return multsym;
    if(strcmp("/", word) == 0)
        return slashsym;
    if(strcmp("odd", word) == 0)
        return oddsym;
    if(strcmp("=", word) == 0)
        return eqsym;
    if(strcmp("<>", word) == 0)
        return neqsym;
    if(strcmp("<", word) == 0)
        return lessym;
    if(strcmp("<=", word) == 0)
        return leqsym;
    if(strcmp(">", word) == 0)
        return gtrsym;
    if(strcmp(">=", word) == 0)
        return geqsym;
    if(strcmp("(", word) == 0)
        return lparentsym;
    if(strcmp(")", word) == 0)
        return rparentsym;
    if(strcmp(",", word) == 0)
        return commasym;
    if(strcmp(";", word) == 0)
        return semicolonsym;
    if(strcmp(".", word) == 0)
        return periodsym;
    if(strcmp(":=", word) == 0)
        return becomessym;
    if(strcmp("begin", word) == 0)
        return beginsym;
    if(strcmp("end", word) == 0)
        return endsym;
    if(strcmp("if", word) == 0)
        return ifsym;
    if(strcmp("then", word) == 0)
        return thensym;
    if(strcmp("while", word) == 0)
        return whilesym;
    if(strcmp("do", word) == 0)
        return dosym;
    if(strcmp("call", word) == 0)
        return callsym;
    if(strcmp("const", word) == 0)
        return constsym;
    if(strcmp("var", word) == 0)
        return varsym;
    if(strcmp("procedure", word) == 0)
        return procsym;
    if(strcmp("write", word) == 0)
        return writesym;
    if(strcmp("read", word) == 0)
        return readsym;
    if(strcmp("else", word) == 0)
        return elsesym;
    
    return identsym;
}

// Determines if the symbol is valid
int isSymbol(char c)
{
    int flag;
    
    switch(c)
    {
        case '+':
            flag = 1;
            break;
        case '-':
            flag = 1;
            break;
        case '*':
            flag = 1;
            break;
        case '/':
            flag = 1;
            break;
        case '(':
            flag = 1;
            break;
        case ')':
            flag = 1;
            break;
        case '=':
            flag = 1;
            break;
        case ',':
            flag = 1;
            break;
        case '.':
            flag = 1;
            break;
        case '<':
            flag = 1;
            break;
        case '>':
            flag = 1;
            break;
        case ';':
            flag = 1;
            break;
        case ':':
            flag = 1;
            break;
        default:
            flag = 0;
            break;
    }
    
    return flag;
}

// If the symbol is a single letter
int isSingleSymbol(char c)
{
        int flag;
    
    switch(c)
    {
        case '+':
            flag = 1;
            break;
        case '-':
            flag = 1;
            break;
        case '*':
            flag = 1;
            break;
        case '(':
            flag = 1;
            break;
        case ')':
            flag = 1;
            break;
        case ',':
            flag = 1;
            break;
        case '.':
            flag = 1;
            break;
        case ';':
            flag = 1;
            break;
        default:
            flag = 0;
            break;
    }
    
    return flag;
    
}

int isInvalidSymbol(char c)
{
    if(isalpha(c) || isdigit(c) || isSymbol(c) || isspace(c) || EOF)
        return 0;

    return 1;
}

// Prints out the Lexeme Table
void printLexemeTable(node* head)
{
    // Keeps track of the current node
    node* current = head;
    
    // Formatting the table
    printf("Lexeme Table:\nlexeme\ttoken type\n");
    
    //Until the end of the list is reached 
    while(current != NULL)
    {
        //Accounts for the end of the list
        if(current->next == NULL)
        {
            if(current->error == 0) // If no errors found
                printf("%s\t%d\n\n", current->word, current->token);
            else
                printf("%s\t%d\t%s\n\n", current->word, current->token, printError(current->error));
        }
        else // If at the begining of middle
        {
            if(current->error == 0) // If no errors found
                printf("%s\t%d\n", current->word, current->token);
            else
                printf("%s\t%d\t%s\n", current->word, current->token, printError(current->error));
        }
        // Advance the list
        current = current->next;
    }
}

// If called, prints out the related error that occured
char* printError(int error)
{
    if(error == 1)
        return "Error: Identifier does not start with letter";
    else if(error == 2)
        return "Error: Number Is Too Long";
    else if(error == 3)
        return "Error: Identifier Is Too Long";

    return "Error: Invalid Symbol Used";
}

// Prints the Lexeme List
void printLexemeList(node* head)
{
    // Keeps Track of the current node
    node *current = head;
    
    // Formatting
    printf("Lexeme List:\n");
    
    // Continues until end of list
    while(current != NULL)
    {
        // If the current token is a letter or number
        if(current->token == 2 || current->token == 3)
            printf("%d %s ", current->token, current->word);
        else if(current->next == NULL) // If the end of the list
            printf("%d\n", current->token);            
        else // If at begining or middle
            printf("%d ", current->token);
            
        // Continue the list    
        current = current->next;
    }   
}

void printListToFile(node *head)
{
	// Creates file pointer
	FILE *ofp = fopen("input2.txt", "w");
	if(ofp == NULL)
    {
        printf("Error: Output File Not Found.\n");
        exit(1);
    }
	
	// Keeps Track of the current node
    node *current = head;
    
    // Continues until end of list
    while(current != NULL)
    {
        // If the current token is a letter or number
        if(current->token == 2 || current->token == 3)
            fprintf(ofp, "%d %s ", current->token, current->word);
        else if(current->next == NULL) // If the end of the list
            fprintf(ofp, "%d", current->token);            
        else // If at begining or middle
            fprintf(ofp, "%d ", current->token);
            
        // Continue the list    
        current = current->next;
    }   
	
	fclose(ofp);
}

// Deletes the linked list recursively after use
node* clearList(node* head)
{
    // If the recursion has reached the end
    if(head == NULL)
        return NULL;
    
    // Continue until stopped
    clearList(head->next);
    
    // First free the word then the node iteself
    free(head->word);
    free(head);
    
    // Returns null once complete
    return NULL;
}