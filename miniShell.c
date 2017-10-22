/*******************************************************************
 *           File:  miniShell.c
 *         Author:  Ashwitha Yadav T
 *   Date Started:  September 22, 2017
 *    Description:  A simple implementation of the Unix Shell in the
 *                      C Programming language.
 *******************************************************************/

#include <stdio.h>	// For: printf(), fprintf(), getchar(), perror(), stderr
#include <stdlib.h>     // For: malloc(), realloc(), free(), exit(), execvp(), EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>     // For: strtok(), strcmp(), strcat(), strcpy()
#include <unistd.h>     // For: chdir(), fork(), exec(), pid_t, getcwd()
#include <sys/wait.h>	// For: waitpid()
#include <fcntl.h>	// For: File creation modes

#define BUILTIN_COMMANDS 5	// Number of builtin commands defined

/*
 * Environment variables
 */
char PWD[1024];		// Present Working Directory
char PATH[1024];	// Path to find the commands

/*
 * Built-in command names
 */
char * builtin[] = {"cd", "exit", "help", "pwd", "echo"};

/*
 * Built-in command functions
 */

/*
 * Function:  shell_cd
 * -------------------
 *  changes current working directory
 *
 * args: arguments to the cd command, will consider only the first argument after the command name
 */
int shell_cd(char ** args){
	if (args[1] == NULL){
		fprintf(stderr, "minsh: one argument required\n");
	}
	else if (chdir(args[1]) < 0){
		perror("minsh");
	}
	getcwd(PWD, sizeof(PWD));	// Update present working directory
	return 1;
}

/*
 * Function:  shell_exit
 * ---------------------
 *  exits from the shell
 *
 * return: status 0 to indicate termination
 */
int shell_exit(char ** args){
	return 0;
}

/*
 * Function:  shell_help
 * ---------------------
 *  prints a small description
 *
 * return: status 1 to indicate successful termination
 */
int shell_help(char ** args){
	printf("\nA mini implementation of the Unix Shell by Ashwitha Yadav T.\n");
	printf("\nCommands implemented: ");
	printf("\n\t- help");
	printf("\n\t- exit");
	printf("\n\t- cd dir");
	printf("\n\t- pwd");
	printf("\n\t- echo [string to echo]");
	printf("\n\t- clear");
	printf("\n\t- ls [-ail] [dir1 dir2 ...]");
	printf("\n\t- cp source target (or) cp file1 [file2 ...] dir");
	printf("\n\t- mv source target (or) mv file1 [file2 ...] dir");
	printf("\n\t- rm file1 [file2 ...]");
	printf("\n\t- mkdir dir1 [dir2 ...]");
	printf("\n\t- rmdir dir1 [dir2 ...]");
	printf("\n\t- ln [-s] source target");
	printf("\n\t- cat [file1 file2 ...]");
	printf("\n\n");
	printf("Other features : ");
	printf("\n\t* Input, Output and Error Redirection (<, <<, >, >>, 2>, 2>> respectively)  : ");
	printf("\n\t* Example: ls -i >> outfile 2> errfile [Space mandatory around redirection operators!]");
	printf("\n\n");
	return 1;
}

/*
 * Function:  shell_pwd
 * --------------------
 *  prints the present working directory
 *
 * return: status 1 to indicate successful termination
 */
int shell_pwd(char ** args){
	printf("%s\n", PWD);
	return 1;
}

/*
 * Function:  shell_echo
 * ---------------------
 *  displays the string provided
 * 
 * return: status 1 to indicate successful termination
 */
int shell_echo(char ** args){
	int i = 1;
	while (1){
		// End of arguments
		if (args[i] == NULL){
			break;
		}
		printf("%s ", args[i]);
		i++;
	}
	printf("\n");
}

/*
 * Array of function pointers to built-in command functions
 */
int (* builtin_function[]) (char **) = {
	&shell_cd,
	&shell_exit,
	&shell_help,
	&shell_pwd,
	&shell_echo
};


/*
 * Function:  split_command_line
 * -----------------------------
 *  splits a commandline into tokens using strtok()
 *
 * command: a line of command read from terminal
 *
 * returns: an array of pointers to individual tokens
 */
char ** split_command_line(char * command){
        int position = 0;
        int no_of_tokens = 64;
        char ** tokens = malloc(sizeof(char *) * no_of_tokens);
        char delim[2] = " ";

        // Split the command line into tokens with space as delimiter
        char * token = strtok(command, delim);
        while (token != NULL){
                tokens[position] = token;
                position++;
                token = strtok(NULL, delim);
        }
        tokens[position] = NULL;
        return tokens;
}

/*
 * Function:  read_command_line
 * ----------------------------
 *  reads a commandline from terminal
 *
 * returns: a line of command read from terminal
 */
char * read_command_line(void){
        int position = 0;
        int buf_size = 1024;
        char * command = (char *)malloc(sizeof(char) * 1024);
        char c;

        // Read the command line character by character
        c = getchar();
        while (c != EOF && c != '\n'){
                command[position] = c;

                // Reallocate buffer as and when needed
                if (position >= buf_size){
                        buf_size += 64;
                        command = realloc(command, buf_size);
                }

                position++;
                c = getchar();
        }
        return command;
}

/*
 * Function:  start_process
 * ------------------------
 *  starts and executes a process for a command
 *
 * args: arguments tokenized from the command line
 *
 * return: status 1
 */
int start_process(char ** args){

        int status;
        pid_t pid, wpid;

        pid = fork();

        if (pid == 0){  // It's the child process
		
		// Find the path of the command
		char cmd_dir[1024];
		strcpy(cmd_dir, PATH);
		strcat(cmd_dir, args[0]);
		
		// Execute the required process		
		if ( execv( cmd_dir, args ) == -1){ // Error
			perror("minsh");
		}

		exit(EXIT_FAILURE);	// To exit from child process
        }
        else if (pid < 0){      // Error in forking
		perror("minsh");
        }
        else{           // It's the parent process
                do{
                        wpid = waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }

        return 1;
}

/*
 * Function:  shell_execute
 * ------------------------
 *  determines and executes a command as a built-in command or an external command
 *
 * args: arguments tokenized from the command line
 *
 * return: return status of the command
 */
int shell_execute(char ** args){

	if (args[0] == NULL){	// Empty command
		return 1;
	}

	// Copy the current Standard Input and Output file descriptors
	// so they can be restored after executing the current command
	int std_in, std_out, std_err;
	std_in = dup(0);
	std_out = dup(1);
	std_err = dup(2);

	// Check if redirection operators are present
	int i = 1;

	while ( args[i] != NULL ){
		if ( strcmp( args[i], "<" ) == 0 ){	// Input redirection
			int inp = open( args[i+1], O_RDONLY );
			if ( inp < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(inp, 0) < 0 ){
				perror("minsh");
				return 1;
			}
			close(inp);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if ( strcmp( args[i], "<<" ) == 0 ){	// Input redirection
			int inp = open( args[i+1], O_RDONLY );
			if ( inp < 0 ){

				perror("minsh");
				return 1;
			}

			if ( dup2(inp, 0) < 0 ){
				perror("minsh");
				return 1;
			}
			close(inp);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( args[i], ">") == 0 ){	// Output redirection

			int out = open( args[i+1], O_WRONLY | O_TRUNC | O_CREAT, 0755 );
			if ( out < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(out, 1) < 0 ){
				perror("minsh");
				return 1;
			}
			close(out);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( args[i], ">>") == 0 ){	// Output redirection (append)
			int out = open( args[i+1], O_WRONLY | O_APPEND | O_CREAT, 0755 );
			if ( out < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(out, 1) < 0 ){
				perror("minsh");
				return 1;

			}
			close(out);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( args[i], "2>") == 0 ){	// Error redirection
			int err = open( args[i+1], O_WRONLY | O_CREAT, 0755 );
			if ( err < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(err, 2) < 0 ){
				perror("minsh");
				return 1;
			}
			close(err);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( args[i], "2>>") == 0 ){	// Error redirection
			int err = open( args[i+1], O_WRONLY | O_CREAT | O_APPEND, 0755 );

			if ( err < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(err, 2) < 0 ){
				perror("minsh");
				return 1;
			}
			close(err);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;

		}
		else{
			i++;
		}
	}

	// If the command is a built-in command, execute that function
	for(i = 0 ; i < BUILTIN_COMMANDS ; i++){
		if ( strcmp(args[0], builtin[i]) == 0 ){
			int ret_status = (* builtin_function[i])(args);
			
			// Restore the Standard Input and Output file descriptors
			dup2(std_in, 0);
			dup2(std_out, 1);
			dup2(std_err, 2);

			return ret_status;
		}
	}

	// For other commands, execute a child process
	int ret_status = start_process(args);

	// Restore the Standard Input and Output file descriptors
	dup2(std_in, 0);
	dup2(std_out, 1);
	dup2(std_err, 2);

	return ret_status;
}

/*
 * Function:  shell_loop
 * ---------------------
 *  main loop of the Mini-Shell
 */
void shell_loop(void){

	// Display help at startup
	int status = shell_help(NULL);

        char * command_line;
        char ** arguments;
	status = 1;

        while (status){
                printf("minsh> ");
                command_line = read_command_line();
		if ( strcmp(command_line, "") == 0 ){
			continue;
		}
                arguments = split_command_line(command_line);
                status = shell_execute(arguments);
        }
}

/*
 * Function:  main
 */
int main(int argc, char ** argv){
        // Shell initialization
	getcwd(PWD, sizeof(PWD));	// Initialize PWD Environment Variable
	strcpy(PATH, PWD);		// Initialize the command PATH
	strcat(PATH, "/cmds/");		// ^^

        // Main loop of the shell
        shell_loop();

        return 0;
}
