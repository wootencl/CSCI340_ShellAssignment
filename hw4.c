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
#include "shell.h"

// -----------------------------------
// Main function
// Arguments:	argc = number of arguments suppled by user
//				argv = array of argument values
//
//
int main( int argc, char** argv ) {
	int done;
	char input[255];
	command_t temp_command;
	char tempPath[255];

	//if statements for signal handling
	if (signal(SIGCHLD, sig_child_handler) == SIG_ERR) {
		perror("Unable to create new SIGCHLD signal handler!");
		exit(-1);
	}
	// //if statements for ctrl-c handling
	// if (signal(SIGINT, sig_int_handler) == SIG_ERR) {
	// 	perror("Unable to create new SIGCHLD signal handler!");
	// 	exit(-1);
	// }

	done = FALSE;
	while (!done) {
		printf("$");
		fflush(stdout);
		gets(input);
		parse(input, &temp_command);
		if (stringCompare(temp_command.name,"exit")==1)
		{
			done = TRUE;
		}
		else if (is_builtin(&temp_command))
		{
			do_builtin(&temp_command);
		}
		else if (find_fullpath(tempPath, &temp_command))
		{
			execute(&temp_command);
		}
		else {
			printf("Command not found.\n");
		}
		cleanup(&temp_command);
	}

	// printf( "CSCI 340 Homework Assignment 2 - Have Fun!\n" );

	return 0;

} // end main function

