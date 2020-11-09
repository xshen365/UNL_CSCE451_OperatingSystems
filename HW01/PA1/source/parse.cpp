/* 
*   Version : 0.8
*   CommandParser.cpp
*       Parse the input line to extract commands, copy the commands
*   to the Command structure, and link the commands into a chain
*   DATE: 2019-01-16
*/


#ifndef _OSH_COMMANDPARSER_CPP_ 
#define _OSH_COMMANDPARSER_CPP_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

const int status_success = 0;
const int status_failure = 1;

// Symbol type represents the command seperator
typedef enum SymbolType {
    RedirectIn,             // <
    RedirectInAppend,       // <<
    RedirectOut,            // >
    RedirectOutAppend,      // >>
    ExecuteOnSuccess,         // && - exec on success
    ExecuteOnFailure,          // || - exec on failure 
    Pipe,                   // | 
    Null,                   // end of string (null character encountered)
    NewLine,                // end of command due to new line
    Semicolon,              // ;
} SymbolType;


// Command contains the parsed command. These structures are chained in a 
// doubly linked list 
typedef struct Command {
    char *file;             // file to execute 
    char **arglist;         // argument list to executable
    SymbolType symbolType;  // command seperator 

    FILE *inFilePtr;        // file pointer to input stream 
    FILE *outFilePtr;       // file pointer to output stream 
    FILE *errorFilePtr;     // file pointer to error stream 
    int inFileHandle;       // file handle to input stream 
    int outFileHandle;      // file handle to output stream
    int errorFileHandle;    // file handle to error stream 
    int status;             // exit code of the commnad

    struct Command *next, *prev;   
} Command;


/* some utility functions */
// trim the line of white spaces
int trim(char *line, size_t *length)
{
    char *read = NULL, *write = NULL;
    int i = 0, j = 0;

    // trim from back
    i = *length - 1; 
    while(isspace(line[i]))
        i--;
    line[i+1] = '\0';

    // chomp the spaces in front
    read = write = line;
    while (isspace(*read)) 
        read++;

    // move the characters 
    for(  ; *write = *read; read++, write++) ;

    // update new length
    *length = strlen(line);

     return 0;   
}

/*
//  wrapper around realloc. allocates buffer if the currentsize < requested newsize
//
//  return value : 
//      status_success -    memory was successfully allocated, buffer points to newly 
//                          allocated memory, currentSize is updated to new size
//      status_failure -    memory allocation failed. buffer and currentSize is not
//                          modified
*/
int allocateBuffer(void **buffer, size_t *currentSize, size_t newSize, size_t datasize) 
{
    void *buf;
    int status = status_success;

    if(newSize > *currentSize) {
        buf = (void *)realloc(*buffer, (newSize * datasize));

        if(NULL == buf) {
            printf("Unable to allocate memory for buffer\n");
            status = status_failure;
            goto exit_1;
        }       

        *buffer = (void*)buf;
        *currentSize = newSize;
    }

exit_1:
    return status;
}

// wrapper around strncpy. alloates buffer and copies string
int allocateAndCopyString(char** destination, char *source) 
{
	int status = status_success;
	char *temp = NULL;
	size_t buffersize = 0;
	size_t newsize = strlen(source) + 1;

	if(status_failure == 
		(status = allocateBuffer((void**)&temp, &buffersize, newsize+1, sizeof(char)))) {
		*destination = NULL;
		goto exit1;
	}
	
	strncpy(temp, source, newsize);
	temp[newsize] = '\0';
	
	*destination = temp;
	
exit1:
	return status;
}

// list of Symbols that seperate command
bool isCommandBreaker(char ch)
{
    if(ch == '|' || ch == ';' || ch == '\n' ||
            ch == '<' || ch == '>' || ch == '&' || ch == '\0')
        return true;

    return false;
}


/*
// allocate a new commnad structure and initialize it with default values
//
// return value :  
//      statu_succes -      success, and command points to the newly allocated
//                          memory
//      status_failure -    failed to allocate memory, command points to NULL
//
// notes:
*/
int allocateNewCommand(Command **command) 
{
    Command *temp = NULL;
    int status = status_success;

    if(NULL == (temp = (Command*)malloc(sizeof(Command)))) {
        printf("could not allocate memory for command\n");
        status = status_failure;
        *command = NULL;
        goto exit1;
    }

    temp->file = NULL;
    temp->next = NULL;
    temp->prev = NULL;
    temp->inFilePtr = stdin;
    temp->outFilePtr = stdout;
    temp->inFileHandle = STDIN_FILENO;
    temp->outFileHandle = STDOUT_FILENO;
    temp->status = status_success;

    *command = temp;

exit1:
    return status;
}

/*
// delete the command from the doubly linked list 
//
// return value : 
//      status_success -    success, and deletion was successful
//
// notes : 
*/
int deleteCommand(Command *command)
{
    int status = status_success;
    Command *temp = command;
    char **arg;
    int i;

    // adjust previous command's fwd link
    if(temp->prev != NULL) {
        (temp->prev)->next = temp->next;
    }

    // adjust next command's prev link
    if(temp->next != NULL) {
        (temp->next)->prev = temp->prev;
    }

    // free argument list
	if(NULL != temp->arglist) {
		for(i=0, arg = temp->arglist; arg[i] != NULL; i++) {
			free(arg[i]);
		}
		
		free(arg);
	}

    // free the arglist and command
    free(command);

    return status;

}

/*
// prints the contents of command. (debugging purposes)
*/
void DumpCommand(Command* command) 
{
    int i = 0;

	printf("----------------------------------------------------\n");
	printf("file : %s\n", command->file);
	printf("arglist : "); 

	if(NULL != command->arglist) {
		for(i = 0; NULL != command->arglist[i]; i++) {
			printf("%s \n", command->arglist[i]);
		}
	}
	
    printf("\n Token : %d\n", command->symbolType);
	printf("---------------------------------------------------\n");
}

/*
// adds the command to the tail of the doubly linked list
// pointed to by the head
// 
// notes : 
//          head can be NULL.i.e. empty list 
//          head is updated when the list is NULL
*/
int AddCommand(Command **head, Command *add)
{
    int status = status_success;
    Command *temp;

    if(NULL == *head) {
        *head = add;
        return status;
    }

    // walk till end of list
    for (temp = *head; NULL != temp->next; temp = temp->next) {
    }

    temp->next = add;
    add->prev = temp;

    return status; 
}

/*
// prints each command in the doubly linked list in sequence (debugging purposes)
*/
void DumpCommandChain(Command *head)
{
    while (head != NULL) {
        DumpCommand(head);
        head = head->next;
    }
}

/*
//  returns the length of the doubly linked list
*/
int GetComandChainLength(Command *head) {
    int length = 0;
    while (head != NULL) {
        head = head->next; 
        length++;
    }

    return length;
}

/*
// deletes all the members in the doubly linked list
*/
int DeleteCommandChain(Command *head) {
    int status = status_success;

    Command *temp = head;

    while (temp != NULL) {
        head = temp->next;
        deleteCommand(temp);
        temp = head;
    }

    return status;
}

/*
//  returns the SymbolType encountered in the input
//  
//  return value : 
//      status_success -   symbol is updated to contain symbol type, and read
//                         pointer (*line) is advanced after reading the symbol
//
//  notes : 
*/
int GetCommandBreaker(char **line, 
                        SymbolType *symbol)
{
    int status = status_success;
    char *temp = *line;
    size_t adv = 0;         // no of chars to advance after read
    SymbolType symtype;

    switch(*temp) {
        case '\0' : symtype = Null;
                    break;

        case '>' :  if(*(temp+1) == '>') {
                        symtype = RedirectOutAppend;
                        adv = 2;
                    } else {
                        symtype = RedirectOut;
                        adv = 1;
                    }

                    break;

        case '<' :  if(*(temp+1) == '<') {
                        symtype = RedirectInAppend;
                        adv = 2;
                    } else {
                        symtype = RedirectIn;
                        adv = 1;
                    }

                    break;                   

        case '&' :  if(*(temp+1) == '&') {
                        symtype = ExecuteOnSuccess;
                        adv = 2;
                    } else {
                        printf("The symbol is not accepted : (%c)", *(temp+1));
                        adv = 1;
                    }

                    break;

        case '|' :  if(*(temp +1) == '|') {
                        symtype = ExecuteOnFailure;
                        adv = 2;
                    } else {
                        symtype = Pipe;
                        adv = 1;
                    }

                    break;

        case '\n' : symtype = NewLine;
                    adv = 1;
                    break;

        case ';' : symtype = Semicolon;
                    adv = 1;
                    break;

        default :   printf("Unknown symbol");
                    adv = 1;
                    break;
    }
    
    *line += adv;
    *symbol = symtype;
    return status;
}


/*
//  reads the input string, and returns the substring that contains one command.
//  this would be sequence of characters that are seperated by any symbols (|, >, >> etc)
//
//  return value:
//      status_success -    commandString points to the copy of the substring that contains the command,
//                          line is advanced due to read, commandBufferLength contains the length of the 
//                          buffer containing the substring, and commandLength contains the length of command 
//      status_failure -    commandLength is updated to 0
//
// notes : 
//      memory is allocated by the function by using realloc (allocateBuffer). In case of failure, 
//      commandString is not changed from the value set by allocateBuffer
*/
int GetNextCommandString(char **line, 
                char **commandString, 
                size_t *commandBufferLength, 
                size_t *commandLength)
{
    int status = status_success;
    char *read = *line;
    size_t length = 0;

    // read the command line till we get a symbol. 
    while(false == isCommandBreaker(*read)) {
        read++;
        length += 1;
    }

    // allocate buffer using length
    if(status_failure == (status = allocateBuffer
                    ((void**)commandString, commandBufferLength, length +1, sizeof(char)))) {
        *commandLength = 0;
        goto exit1;
    }

    // copy and update 
    strncpy(*commandString, *line, length);
    (*commandString)[length] = '\0';

    // we want to advance the length so that it does not include
    // commandString breaker
    *commandLength = length;
    *line += *commandLength;  

exit1:
    return status;
}

/*
// parses the input command string (which contains a single command), 
// extracts the binary to execute, arguments to pass to binary and 
// stores in command structure
//
// return value:
//      status_success -  command points to the new command structure 
//                        that contains the parsed contents
//      status_failure -  command is set to NULL
*/
int ParseCommand(Command **command, char *commandLine, SymbolType symtype)
{
   int status = status_success;
   Command *commandtemp = NULL;
   char *file = NULL;
   char *temp = NULL;
   size_t tempbufferlength = 0;
   char *token = NULL;
   char **arglist = NULL;
   char **temparglist;
   char *temparg = NULL;
   char delimiter[2] = " ";
   char *saveptr1 = NULL;
   size_t commandLineLength = strlen(commandLine);
   size_t tokenbuffersize = 0;
   size_t arglistLength = 0;
   int i=0; 
   int j=0;
   
	if(0 == commandLineLength) {  // null command
		file = NULL;
		arglist = NULL;
		symtype = symtype;
	} else {
	   // break string into tokens (space seperated)
		if(status_failure == (status = allocateBuffer((void**)&temp, 
			&tempbufferlength, commandLineLength + 1, sizeof(char)))) {
			*command = NULL;
			goto exit1;
		}
		strncpy(temp, commandLine, commandLineLength);
		temp[commandLineLength] = '\0';
		
		trim(temp, &commandLineLength);
		
		// get filename
		if(NULL == (token = strtok_r(temp, delimiter, &saveptr1))) {
			goto exit1;
		}

		// update filename
		if(status_failure == (status = allocateAndCopyString(&file, token))) {
			goto exit1;
		}
	 
		j=0;
		if(NULL  == (temparglist = (char**) realloc(arglist, sizeof(char*) * (j+2)))) {
			goto exit1;
		}
		arglist = temparglist;
			
		temparg = NULL;
		if(status_failure == (status = allocateAndCopyString(&temparg, token))) {
			goto exit1;
		}
		
		arglist[j] = temparg;
		arglist[j+1] = NULL; 
		j++;
		
		// read tokens and create argv
		while(true) {
			if(NULL  == (temparglist = (char**) realloc(arglist, sizeof(char*) * (j+2)))) {
				goto exit1;
			}
			arglist = temparglist;

			temp = NULL;
			if(NULL == (token = strtok_r(temp, delimiter, &saveptr1))) {
				arglist[j] = NULL;
				break;
			}
		
			temparg = NULL;
			if(status_failure == (status = allocateAndCopyString(&temparg, token))) {
				goto exit1;
			}

			arglist[j] = temparg;
			arglist[j+1] = NULL; 
			j++;
		}
	}

	// allocate memory for command
	if(*command == NULL) {
		if(status_failure == (status = allocateNewCommand(command)))
		{
			*command = NULL;
			goto exit1;
		}
	}

    // update command
	commandtemp = *command;
    (*command)->file = file;
    (*command)->arglist = arglist;
    (*command)->symbolType = symtype;

exit1:
    return status; 
}


/* 
// reads a line from the stdin, extracts substrings that 
// contains commands, parses the substring extracts the binary to
// execute, arglist etc and loads it into command structure. The 
// commands are chained together to form a doubly linked list
//
// return value :
//      status_success - head points to the doubly linked list of 
//                       commands
//      status_failure - head is set to NULL
*/
int GetCommandChain(Command **head)
{
    int status = status_success;
    char *line = NULL;
    char *commandString = NULL;
    size_t lineLength = 0;
    size_t bufferLength = 0;
    size_t commandBufferLength = 0;
    size_t commandLength = 0;
    SymbolType symtype = Null; 
    Command *headCommand = NULL, *tempCommand = NULL;

    // buffer is allocated by getline if the length is not sufficient
    if(-1 != (lineLength = getline(&line, &bufferLength, stdin))) {
        trim(line, &lineLength);
        
        headCommand = NULL;
        tempCommand = NULL;
        while(true) {
            if(status_failure == 
				(status = GetNextCommandString
				(&line, &commandString, &commandBufferLength, &commandLength)))
                break; // error
				
            if(status_failure == (status = GetCommandBreaker(&line, &symtype)))
                break; // error

			trim(commandString, &commandLength);
			
            if(0 == commandLength && (symtype != Null || symtype != NewLine)) {
				// break; // we will create a null command and add to chain
			}
			
            // parse the command string
            tempCommand = NULL;
            if(status_failure == 
                (status = ParseCommand(&tempCommand, commandString, symtype))){
                DeleteCommandChain(headCommand);
                *head = NULL;
                goto exit1;
            }

			//printf("Get command length : (%d)(%d)%s\n", symtype, commandLength, line);
			//fflush(stdout);
            AddCommand(&headCommand, tempCommand);

            // exit We are done parsing the line
            if(NewLine == symtype || Null == symtype)
                break;  
        }
    }

   *head = headCommand;

exit1:
    return status_success;
}

/*
int main()
{
             Command *head = NULL;
        
                // get the command chain   
                    if(status_failure != GetCommandChain(&head)) {
                            DumpCommandChain(head);
                                    DeleteCommandChain(head);
                                        }
       
                                           return status_success;
}
*/
#endif
