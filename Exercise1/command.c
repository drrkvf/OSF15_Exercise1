#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#define MAX_CMD_COUNT 50
#define MAX_CMD_LEN 25


/* 
 * PURPOSE: Parses the user input so that it can be used and finds the command
 * INPUTS: 
 *	input - Point to the user input to be ran
 *	cmd - double pointer to the command to be ran later 
 * RETURN:
 *  true - If the command was found and set to be ran
 *  false - If the command could not be found or if the input is invalid
 *
 **/
bool parse_user_input (const char* input, Commands_t** cmd) {
	
	if (input == NULL || cmd == NULL){
		printf("Error: Null Pointer\n");
		return false;
	}

	char *string = strdup(input);
	
	*cmd = calloc (1,sizeof(Commands_t));
	(*cmd)->cmds = calloc(MAX_CMD_COUNT,sizeof(char*));

	unsigned int i = 0;
	char *token;
	token = strtok(string, " \n");
	for (; token != NULL && i < MAX_CMD_COUNT; ++i) {
		(*cmd)->cmds[i] = calloc(MAX_CMD_LEN,sizeof(char));
		if (!(*cmd)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}	
		strncpy((*cmd)->cmds[i],token, strlen(token) + 1);
		(*cmd)->num_cmds++;
		token = strtok(NULL, " \n");
	}
	free(string);
	return true;
}

/* 
 * PURPOSE: Frees the commands that were used in the running of the program
 * INPUTS: 
 *	cmd - double pointer to the commands that were used
 * RETURN:
 *
 **/
void destroy_commands(Commands_t** cmd) {

	if (cmd == NULL){
		printf("Error: Null pointer\n");
		return;
	}
	
	for (int i = 0; i < (*cmd)->num_cmds; ++i) {
		free((*cmd)->cmds[i]);
	}
	free((*cmd)->cmds);
	free((*cmd));
	*cmd = NULL;
}

