// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "shell.h"

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist, however in future, more could 
// be added to the builtin command array.
// --------------------------------------------
const char* valid_builtin_commands[] = {"cd", "exit", NULL};

void parse( char* line, command_t* p_cmd ) {
	// Initialization of variables
	int i = 0;
	// This i will hold the place in the line throughout the function
	int commandIterator=  0;
	int commandPtr;
	int argc = 0;
	int commandLength;
	int commandPlace;
	// Get rid of leading whitespaces
	while (*line == ' ' && *line != '\0') {
		line++;
	}
	while (line[i] != '\0') {
		//if while is entered there is at minimum one arg. thus iterate.
		argc++;
		while (line[i] != '\0') {
			//will continue to loop through the line.
			if (line[i] == ' ') 
			// if this is hit then a space has been encountered after first arg. 
			{
				//will check to see if mulitple spaces. if so iterate through.
				while (line[i] == ' ' && line[i] != '\0') {i++;}
				//spaces haven been consumed. will return to original while loop and either break out of it or iterate arg and keep moving. 
				break;
			}
			i++;
		}

	}
	p_cmd->argc = argc;
	p_cmd->argv = (char**) malloc((argc+1)*sizeof(char*));
	// printf("allocated %d spaces for argv array\n", argc+1);
	i = 0;
	for (commandIterator = 0; commandIterator < argc; commandIterator++)
	{
		commandLength = 0;
		//iterate over the chars
		while (line[i] != ' ' && line[i] != '\0') {
			i++;
			commandLength++;
		}
		// Malloc the space needed for the argv cell
		p_cmd->argv[commandIterator] = malloc(commandLength+1);
		// printf("malloced %d space for argv[%d]\n", commandLength+1, commandIterator);
		commandPtr = 0;
		commandPlace = i - commandLength;
		while (commandPlace < i) {
			p_cmd->argv[commandIterator][commandPtr] = line[commandPlace];
			commandPtr++;
			commandPlace++;
		}
		// if (commandIterator > 2)
		// {
		// 	printf("2nd argv[2]:%s\n",p_cmd->argv[2]);
		// 	printf("commandIterator: %d, commandPlace: %d\n", commandIterator, commandPlace);
		// }
		p_cmd->argv[commandIterator][commandPtr] = '\0';
		// printf("argv[%d] = %s\n", commandIterator, p_cmd->argv[commandIterator]);
		// if (commandIterator > 2)
		// {
		// 	printf("3rd argv[2]:%s\n",p_cmd->argv[2]);
		// }
		// iterate past any extra whitespaces
		while (line[i] == ' ' && line[i] != '\0') {
			i++;
		}
	}
	p_cmd->argv[commandIterator] = NULL;
	//insertion of the name into the struct
	i = 0;
	commandPtr = 0;
	while (line[i] != ' ' && line[i] != '\0') {
		i++;
	}
	p_cmd->name = malloc(i+1);
	while (commandPtr < i) {
		p_cmd->name[commandPtr] = line[commandPtr];
		commandPtr++;
	}
	p_cmd->name[commandPtr] = '\0';
	// printf("p_cmd->name = %s\n", p_cmd->name);
}

int is_builtin( command_t* p_cmd ) {
	int i = 0;
	int j = 0;
	int flag = 0;

	// essentially copied the stringCompare method here. Wasn't able to use it because of the whole const issue
	while (valid_builtin_commands[i] != NULL) {
		j = 0;
		while (valid_builtin_commands[i][j] != '\0' && p_cmd->name[j] != '\0') {
			// Debugging leftovers.
			// printf("%c == %c\n", valid_builtin_commands[i][j], p_cmd->name[j]);
			if(valid_builtin_commands[i][j] != p_cmd->name[j]){
            	flag=1;
             	break;
         	}
         	j++;
		}
		if (flag==0 && valid_builtin_commands[i][j] == '\0' && p_cmd->name[j] == '\0') {
			return TRUE;
		}
		i++;
	}
	return FALSE;
}
int find_fullpath( char* fullpath, command_t* p_cmd ) {
	char* path_env_variable;
	char tempPath[255];
	int i=0; //Incrementer for the envpath as a whole
	int j; //Incrementer for each seperate path. 
	int k; //Incrementer for the name of the cmd.
	int pathCount = 0;
	int pathIncr;
	struct stat buffer;
	int exists;


	path_env_variable = getenv( "PATH" );
	// count the number of colons in the Path. to be used in the for loop below
	while (path_env_variable[i] != '\0') {
		if (path_env_variable[i] == ':') 
		{
			pathCount++;
		}
		i++;
	}
	pathCount++; //add the last path that doesn't have the :
	i = 0;
 	for (pathIncr = 0; pathIncr < pathCount; ++pathIncr)
 	  {
		j = 0;
		k = 0;
		// put the path into the the tempPath
		while (path_env_variable[i] != ':' && path_env_variable[i] != '\0') {
			tempPath[j] = path_env_variable[i];
			i++;	
			j++;		
		}
		tempPath[j] = '/'; //add the necessary /
		j++; //increment j past the /
		//while loop to append the cmd to the path
		while (p_cmd->name[k] != '\0') {
			tempPath[j] = p_cmd->name[k];
			k++;
			j++;
		}
		tempPath[j] = '\0';
		// begininng of code that checks whether or not the file exist
		exists = stat(tempPath, &buffer);
		if (exists == 0 && (S_IFREG & buffer.st_mode)) {
			i = 0;
			//move the fullpath into the return variable
			while (tempPath[i] != '\0') {
				fullpath[i] = tempPath[i];
				i++;
			}
			fullpath[i] = '\0';
			// printf("File exists\n");
			return TRUE;;
		}
		// i++ to increment past :
		i++;
	}
	return FALSE;
}
int do_builtin( command_t* p_cmd ) {
	int returnBool;
	if (stringCompare(p_cmd->name, "cd") == 1) {
		//not sure if this is ok but i can't think of a case where chdir will have more than one input
		returnBool = chdir(p_cmd->argv[1]);
		if (returnBool == 0)
		{
			return SUCCESSFUL;
		} else {
			perror("Cd terminated with an error condition!\n");
			return ERROR;
		}
	}
	return 0;
}
int execute( command_t* p_cmd ) {
	int fnd = FALSE;
	char fullpath1[255];
	char fullpath2[255];
	int stat;
	// pipeBoolean will serve as the flag that there is a pipe in the command and pipeLoc will save the location.
	int pipeBoolean = 0;
	int pipeLoc = 0;
	// redirectBoolean will serve as the flag that there is a redirect in the command and redirectLoc will save the location.
	int redirectBoolean = 0;
	int redirectLoc = 0;
	int outfile;
	// ampBoolean will serve ad the flag that there is a ampersand (&) in the command
	int ampBoolean = 0;
	int i = 0;
	int j = 0;
	int child_process_status;
	int fds[2];
	pid_t cpid1, cpid2;
	command_t parentStruct;
	command_t childStruct;

	// Scanning the argv array for a pipe (|)
	while (p_cmd->argv[i] != NULL) {
		if (p_cmd->argv[i][0] == '|') {
			pipeBoolean = 1;
			pipeLoc = i;
			break;
		} 
		i++;
	}
	i = 0;
	// Scanning the argv array for a redirect (>)
	while (p_cmd->argv[i] != NULL) {
		if (p_cmd->argv[i][0] == '>') {
			redirectBoolean = 1;
			redirectLoc = i;
			break;
		} 
		i++;
	}
	i = 0;
	//Check if the last element is an ampersand (&)
	if (p_cmd->argv[(p_cmd->argc-1)][0] ==  '&') {
		ampBoolean = 1;
	}
	//  if statement to determine if a pipe was found
	if (pipeBoolean == 1) {
		// Creation of the parentStruct

		parentStruct.name = p_cmd->argv[0];
		parentStruct.argv = (char**) malloc(sizeof(char*)*p_cmd->argc);
		parentStruct.argv[0] = p_cmd->argv[0];

		for (i = 1; i < pipeLoc; i++)
		{
			parentStruct.argv[i] = p_cmd->argv[i];
		}

		parentStruct.argv[i] = NULL;
		parentStruct.argc = i;
		// Creation of the cargs array
		childStruct.name = p_cmd->argv[pipeLoc+1];
		childStruct.argv = (char**) malloc(sizeof(char*)*p_cmd->argc);
		childStruct.argv[0] = p_cmd->argv[i];
		for (i = (pipeLoc + 1); i < (p_cmd->argc); i++)
		{
			childStruct.argv[j] = p_cmd->argv[i];
			j++;
		}
		childStruct.argv[j] = NULL;
		childStruct.argc = j;

		pipe(fds);
		if ((cpid1 = fork()) == 0) {
			// Find the path to the first command.
			find_fullpath(fullpath1, &parentStruct);
			close(1);
			dup(fds[1]);
			close(fds[0]);
			execv(fullpath1, parentStruct.argv);
			perror("Execute terminated with an error!\n");
		}
		if ((cpid2 = fork()) == 0) {
			find_fullpath(fullpath2, &childStruct);
			close(0);
			dup(fds[0]);
			close(fds[1]);
			execv(fullpath2, childStruct.argv);
			perror("Execute terminated with an error!\n");
		}

		close(fds[0]);
		close(fds[1]);
		waitpid(cpid1, &child_process_status, 0);
		waitpid(cpid2, &child_process_status, 0);

		free(childStruct.argv);
		free(parentStruct.argv);
	} else {
		// Code to run a nonpiped command. 
		fnd = find_fullpath( fullpath1, p_cmd );	
		if ( fnd ) {
			// If to determine if simple command or a redirect
			if (redirectBoolean == 1) {
			// Redirect detected. Adjust execution for that.
				outfile = open(p_cmd->argv[redirectLoc+1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP );

				if (outfile == -1) {
					fprintf(stderr, "failed to open file\n");
				} else if ((cpid1 = fork()) == 0) {
					dup2(outfile, 1);
					p_cmd->argv[redirectLoc] = NULL;
					execv(fullpath1, p_cmd->argv);
					exit(1);
				}
				close(outfile);
				waitpid(cpid1, &child_process_status, 0);
			} else if (ampBoolean == 1) {
			// Ampersand detected. Adjust execution for that.
				cpid1 = fork();

				if (cpid1 == 0)
				{
					p_cmd->argv[(p_cmd->argc-1)] = NULL;
					execv( fullpath1, p_cmd->argv);
					perror("Child process terminated in error condition!");
					exit(-1);
				}
			} else {
			// Simple command, execute as normal.
				if ( fork() == 0 ) {
				execv( fullpath1, p_cmd->argv );
				perror("Execute terminated with an error!\n");
				}
				wait(&stat);
			}
		} else {
		// display to user cmd not found
		perror("Execute terminated with an error!\n");
		}
	}

	return 0;
}
void cleanup( command_t* p_cmd ) {
	int i = 0;

	// while (p_cmd->argv[i] != NULL) {
	// 	printf("%s\n", p_cmd->argv[i]);
	// 	i++;
	// }
	i=0;
	free(p_cmd->name);
	p_cmd->name = NULL;
	while (p_cmd->argv[i] != NULL) {
		free(p_cmd->argv[i]);
		p_cmd->argv[i] = NULL;
		//printf("i = %d\n", i);
		i++;
	}
	free(p_cmd->argv);
	p_cmd->argv = NULL;
}

// void sig_int_handler(int sig) {
// 	printf("Running process terminated!\n");
// 	exit(0);
// }

void sig_child_handler(int sig) {
	int status;
	pid_t pid;

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		// printf("Child process (%d) has terminated!\n", pid);
	}
}
// --------------------------------------------
// Implementation for each prototype below
// --------------------------------------------
// void parse( char* line, command_t* p_cmd );
// int execute( command_t* p_cmd );
// int find_fullpath( char* fullpath, command_t* p_cmd );
// int is_builtin( command_t* p_cmd );
// int do_builtin( command_t* p_cmd );
// void cleanup( command_t* p_cmd );

// String compare function i took from the web. Source: 
// http://www.cquestions.com/2011/09/how-to-compare-two-strings-in-c-without.html
 int stringCompare(char str1[],char str2[]){
    int i=0,flag=0;
   
    // printf("%s %s\n", str1, str2);

    while(str1[i]!='\0' && str2[i]!='\0'){
         if(str1[i]!=str2[i]){
             flag=1;
             break;
         }
         i++;
    }

    if (flag==0 && str1[i]=='\0' && str2[i]=='\0')
         return 1;
    else
         return 0;

}