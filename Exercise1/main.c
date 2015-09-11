#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include<readline/readline.h>

#include "command.h"
#include "matrix.h"

void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats);
unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, 
			const char* target);

// TODO complete the defintion of this function. 
void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats);

/* 
 * PURPOSE: This fuction controls the calls in the program and the ui for the user
 * INPUTS: 
 *  argc - The number of arguments entered
 *  argv - A double pointer to the arguments that were entered
 * RETURN: 
 *  0 - When the program terminates due to error or completion 
 **/
int main (int argc, char **argv) {
	unsigned int unsignedIntError = 0;
	bool errorCheck = true;
	srand(time(NULL));		
	char *line = NULL;
	Commands_t* cmd;

	Matrix_t *mats[10];
	memset(&mats,0, sizeof(Matrix_t*) * 10); // IMPORTANT C FUNCTION TO LEARN

	Matrix_t *temp = NULL;
	errorCheck = create_matrix (&temp,"temp_mat", 5, 5);
	if (errorCheck == false){
		printf("Error creating matrix\n");
		return 0;
	}
	unsignedIntError = add_matrix_to_array(mats,temp, 10); 
	if (unsignedIntError == -1){
		printf("Error adding matrix to array\n");
		return 0;
	}
	int mat_idx = find_matrix_given_name(mats,10,"temp_mat");

	if (mat_idx < 0) {
		perror("PROGRAM FAILED TO INIT\n");
		return 0;
	}
	random_matrix(mats[mat_idx], 10, 15);
	errorCheck = write_matrix("temp_mat", mats[mat_idx]); 
	if (errorCheck == false){
		printf("Error writing matrix to file\n");
		return 0;
	}

	line = readline("> ");
	while (strncmp(line,"exit", strlen("exit")  + 1) != 0) {
		
		if (!parse_user_input(line,&cmd)) {
			printf("Failed at parsing command\n\n");
		}
		
		if (cmd->num_cmds > 1) {	
			run_commands(cmd,mats,10);
		}
		if (line) {
			free(line);
		}
		destroy_commands(&cmd);
		line = readline("> ");
	}
	free(line);
	destroy_remaining_heap_allocations(mats,10);
	return 0;	
}

/* 
 * PURPOSE: This fuction runs the commands that were entered by the user
 * INPUTS: 
 *	cmd - A pointer to the commands to be ran
 *  mats - A double pointer to the array of matrices
 *  numm_mats - A unsigned int to the number of matrices in the array
 * RETURN: 
 **/
void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats) {
	
	if(cmd == NULL || mats == NULL || num_mats <= 0){
		printf("Error: Invalid Input \n");
		return;
	}

	unsigned int unsignedIntError = 0;
	bool errorCheck = true;
	/*Parsing and calling of commands*/
	if (strncmp(cmd->cmds[0],"display",strlen("display") + 1) == 0
		&& cmd->num_cmds == 2) {
			/*find the requested matrix*/
			int idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			if (idx >= 0) {
				display_matrix (mats[idx]);
			}
			else {
				printf("Matrix (%s) doesn't exist\n", cmd->cmds[1]);
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"add",strlen("add") + 1) == 0
		&& cmd->num_cmds == 4) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				Matrix_t* c = NULL;
				if( !create_matrix (&c,cmd->cmds[3], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					printf("Failure to create the result Matrix (%s)\n", cmd->cmds[3]);
					return;
				}
			
				unsignedIntError = add_matrix_to_array(mats,c, num_mats); 
				if (unsignedIntError == -1){
					printf("Error adding matrix to array\n");	
					return;
				}


				if (! add_matrices(mats[mat1_idx], mats[mat2_idx],c) ) {
					printf("Failure to add %s with %s into %s\n", mats[mat1_idx]->name, mats[mat2_idx]->name,c->name);
					return;	
				}
			}
	}
	else if (strncmp(cmd->cmds[0],"duplicate",strlen("duplicate") + 1) == 0
		&& cmd->num_cmds == 3 && strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if (mat1_idx >= 0 ) {
				Matrix_t* dup_mat = NULL;
				if( !create_matrix (&dup_mat,cmd->cmds[2], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					return;
				}
				errorCheck = duplicate_matrix (mats[mat1_idx], dup_mat); 
				if (errorCheck == false){
					printf("Error duplicating matrix\n");
					return;
				}
				unsignedIntError = add_matrix_to_array(mats,dup_mat,num_mats); 
				if (unsignedIntError == -1){
					printf("Error adding matrix to array\n");	
					return;
				}
				printf ("Duplication of %s into %s finished\n", mats[mat1_idx]->name, cmd->cmds[2]);
		}
		else {
			printf("Duplication Failed\n");
			return;
		}
	}
	else if (strncmp(cmd->cmds[0],"equal",strlen("equal") + 1) == 0
		&& cmd->num_cmds == 2) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				if ( equal_matrices(mats[mat1_idx],mats[mat2_idx]) ) {
					printf("SAME DATA IN BOTH\n");
				}
				else {
					printf("DIFFERENT DATA IN BOTH\n");
				}
			}
			else {
				printf("Equal Failed\n");
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"shift",strlen("shift") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const int shift_value = atoi(cmd->cmds[3]);
		if (mat1_idx >= 0 ) {
			errorCheck = bitwise_shift_matrix(mats[mat1_idx],cmd->cmds[2][0], shift_value); 
			if (errorCheck == false){
				printf("Error shifting matrix\n");
				return;
			}
			printf("Matrix (%s) has been shifted by %d\n", mats[mat1_idx]->name, shift_value);
		}
		else {
			printf("Matrix shift failed\n");
			return;
		}

	}
	else if (strncmp(cmd->cmds[0],"read",strlen("read") + 1) == 0
		&& cmd->num_cmds == 2) {
		Matrix_t* new_matrix = NULL;
		if(! read_matrix(cmd->cmds[1],&new_matrix)) {
			printf("Read Failed\n");
			return;
		}	
		
		unsignedIntError = add_matrix_to_array(mats,new_matrix, num_mats); 
		if (unsignedIntError == -1){
			printf("Error adding matrix to array\n");	
			return;
		}
		printf("Matrix (%s) is read from the filesystem\n", cmd->cmds[1]);	
	}
	else if (strncmp(cmd->cmds[0],"write",strlen("write") + 1) == 0
		&& cmd->num_cmds == 2) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if(! write_matrix(mats[mat1_idx]->name,mats[mat1_idx])) {
			printf("Write Failed\n");
			return;
		}
		else {
			printf("Matrix (%s) is wrote out to the filesystem\n", mats[mat1_idx]->name);
		}
	}
	else if (strncmp(cmd->cmds[0], "create", strlen("create") + 1) == 0
		&& strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN && cmd->num_cmds == 4) {
		Matrix_t* new_mat = NULL;
		const unsigned int rows = atoi(cmd->cmds[2]);
		const unsigned int cols = atoi(cmd->cmds[3]);

		errorCheck = create_matrix(&new_mat,cmd->cmds[1],rows, cols); 
		if (errorCheck == false){
			printf("Error creating matrix\n");
			return;
		}
		unsignedIntError = add_matrix_to_array(mats,new_mat,num_mats); 
		if (unsignedIntError == -1){
			printf("Error adding matrix to array\n");	
			return;
		}
		printf("Created Matrix (%s,%u,%u)\n", new_mat->name, new_mat->rows, new_mat->cols);
	}
	else if (strncmp(cmd->cmds[0], "random", strlen("random") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const unsigned int start_range = atoi(cmd->cmds[2]);
		const unsigned int end_range = atoi(cmd->cmds[3]);
		errorCheck = random_matrix(mats[mat1_idx],start_range, end_range); 
		if (errorCheck == false){
			printf("Error adding random data to matrix\n");
			return;
		}

		printf("Matrix (%s) is randomized between %u %u\n", mats[mat1_idx]->name, start_range, end_range);
	}
	else {
		printf("Not a command in this application\n");
	}

}

/* 
 * PURPOSE: This fuction finds the matrix that is wanted to be used
 * INPUTS: 
 *  mats - A double pointer to the array of matrices
 *	num_mats - An unsigned int which holds the number of matrices in the array mats
 *	target - A pointer to the name of the matrix to be found
 * RETURN: 
 *  i - if the matrix was found in the array it returns its position in the array
 * -1 - If it could not be found return -1
 **/
unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, const char* target) {
	
	if(mats == NULL || num_mats <= 0 || target == NULL){
		printf("Error: Invalid Input \n");
		return -1;
	}

	for (int i = 0; i < num_mats; ++i) {
		if (strncmp(mats[i]->name,target,strlen(mats[i]->name)) == 0) {
			return i;
		}
	}
	return -1;
}

/* 
 * PURPOSE: This function frees the array of matrices 
 * INPUTS: 
 *  mats - A double pointer to the array of matrices
 *	num_mats - An unsigned int which holds the number of matrices in the array mats
 * RETURN: 
 **/
void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats) {
	
	if(mats == NULL || num_mats <= 0){
		printf("Error: Invalid Input \n");
		return;
	}

	while(num_mats > 0){
		Matrix_t* temp = *mats;
		mats++;
		free(temp);
		num_mats--;
	}
}
