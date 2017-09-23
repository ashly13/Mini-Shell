/*******************************************************************
 *           File:  miniShell.c
 *         Author:  Ashwitha Yadav T
 *   Date Started:  September 22, 2017
 *    Description:  A simple implementation of the Unix Shell in the
 *			C Programming language.
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Function:  split_command_line
 * -----------------------------
 *  splits a commandline into tokens using strtok()
 *
 *  command: a line of command read from terminal
 *
 *  returns: an array of pointers to individual tokens
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
                printf("\n\t\t%s", token);
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
 *  returns: a line of command read from terminal
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
 * Function:  shell_loop
 * ---------------------
 *  main loop of the Mini-Shell
 */
void shell_loop(void){
        char * command_line;
        char ** arguments;
        int status = 1;

        while (status){
                printf("> ");
                command_line = read_command_line();
                arguments = split_command_line(command_line);
        }
}

/*
 * Function:  main
 */
int main(int argc, char ** argv){
        // Load configuration files


        // Main loop of the shell
        shell_loop();

        // Shell shutdown and memory cleanup


}
