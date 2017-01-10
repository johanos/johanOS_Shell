#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#define MAX_LINE_LEN 512
#define MAX_CMD_LEN 32

void parseSingleCommand(char *line, int begin, int end);
void copySubString(char *buffer, char* begin, int length);
int execute(char ** argv);

char *trimWhiteSpace( char *str ){
	char *end;
	if ( *str == 0 ) return str; //All Spaces?
	while( isspace( (unsigned char) *str) ) str++;

	end = str + strlen(str) - 1;
	while (end > str && isspace( (unsigned char) *end) ) end--;

	*( end+1 ) = 0;

	return str;
}

int isCmdModifier(char *value){
	//pointer so i can see if its an escape char and therefore I can return nah
	switch (*value) {
		case '|':
		case '&':
		case '<':
		case '>': {
			if ( *(--value) == '\\') {
				return 0;
			}
			return 1;
		}
		default:
			return 0;
		break;
	}
}

void handleCommands(char *line){
	/* go through every character in the line. 
	 * I can figure out how many '|' '&' '<' '>' exist in the line. 
	 */
	char currentCommandLine[MAX_LINE_LEN];
	//char nextCommand[MAX_LINE_LEN];

	int numberOfModifiers = 0;
	int count = 0;
	for ( count = 0; count < MAX_LINE_LEN ; count++ ){
		if ( line[count] == 0 ) break;
		if ( isCmdModifier( line + count ) ) numberOfModifiers++;
	}
	//where I will shift and execute each command
	
	//printf("%d\n", numberOfModifiers );

	int commandCounter;
	int ccS = 0; //CurrentCommandStart
	int ccE = 0; //CurrentCommandEnd
	//n + 1 commands to take care of for num of modifiers
	for (commandCounter = 0; commandCounter < numberOfModifiers + 1; commandCounter++){
		for (ccE = 0 ; ccE < MAX_LINE_LEN; ccE++){
			if ( isCmdModifier( line + ccS + ccE )) break; // ii now holds the info on where cmd ends
			if ( *(line + ccS + ccE) == 0 ) break;

		}
		
		parseSingleCommand(line, ccS, ccS + ccE);
		ccS = ccS +  ccE + 1;
	}
	

	//rintf("%d\n", numberOfModifiers );
	//int i, commandLength;
	//printf( "%.*s\n", commandLength, line );
}

void parseSingleCommand(char *line, int begin, int end){
	//should shift this line to the right. 
	//gonna have to trim whitespace.
	int sizeLen, sizeWidth;
	sizeLen = 64; sizeWidth = 32;
	char **argumentList = (char**) calloc(sizeLen, sizeof(char*));

	for ( int i = 0; i < sizeLen; i++ )
	{
	    argumentList[i] = (char*) calloc(sizeWidth, sizeof(char));
	}
	
	char *startPtr = line + begin;
	int whiteCount = 0; 
	while( isspace( (unsigned char) *startPtr) ){ 
		startPtr++;
		whiteCount++;
	}

	//printf("%.*s\n", end - whiteCount, startPtr );
	// startPts and end - whiteCount will give me the current command

	//need to parse the commands now
	int i;
	int index = 0;
	int cmdCounter = 0;
	int currentOffset = 0;
	for (i = 0; i < (end - whiteCount - begin) + 1 ; i++){
		if ( isspace( (unsigned char) *(startPtr + i) ) || startPtr[i] == 0){
			copySubString(argumentList[index], startPtr + currentOffset , cmdCounter );
			index++;
			currentOffset += cmdCounter + 1;
			cmdCounter = 0;
		} else {
			cmdCounter++;
		}
	}
	argumentList[index] = 0; //idk but i needed to do

	//execute
	execute((char **)argumentList);
	
}

void copySubString(char *buffer, char* begin, int length){
	int i;
	for ( i = 0; i < length; i++){
			buffer[i] = begin[i];
	}
	buffer[i] = 0;
}

int execute(char **argumentList){
	pid_t pid = fork();
	int status;
	if ( pid  < 0) {     /* fork a child process           */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          //printf("%s\n", argumentList[0]);
          if (execvp(argumentList[0], argumentList) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     else {                                  /* for the parent:      */
          while (wait(&status) != pid);       /* wait for completion  */
               
     }

     return 0;
}

int main (){
	int d;
	char line[MAX_LINE_LEN]; 
	char cleanLine[MAX_LINE_LEN];
	printf("%s", "my_shell$ ");
	while ( fgets( line, MAX_LINE_LEN, stdin) ){
		strcpy( cleanLine, trimWhiteSpace(line));
		
		handleCommands(cleanLine);
		//printf( "%s\n", cleanLine );
		printf("%s", "my_shell$ " );
	}
	printf("\n");	
}

