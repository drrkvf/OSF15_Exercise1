#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h> //UINT_MAX

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

/* 
 * PURPOSE: instantiates a new matrix with the passed name, rows, cols 
 * INPUTS: 
 *	name the name of the matrix limited to 50 characters 
 *  rows the number of rows the matrix
 *  cols the number of cols the matrix
 * RETURN:
 *  If no errors occurred during instantiation then true
 *  else false for an error in the process.
 *
 **/

bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {

	if(rows<=0 || cols<= 0 || rows > UINT_MAX || cols > UINT_MAX){
		printf("Error: Invalid Input\n");
		return false;
	}

	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	unsigned int len = strlen(name) + 1; 
	if (len > MATRIX_NAME_LEN) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

/* 
 * PURPOSE: This fuction fress the matrix at the pointer that is passed in
 * INPUTS: 
 *  m = A pointer which points to the matrix to be freed
 * RETURN:
 *  Nothing is returned
 *
 **/

void destroy_matrix (Matrix_t** m) {

	if(m == NULL){
		printf("Error: NULL pointer\n");
		return;
	}
	
	
	free((*m)->data);
	free(*m);
	*m = NULL;
}


	
/* 
 * PURPOSE: This fuction compares two matrices to see if they are equal
 * INPUTS: 
 *  a = A pointer which points to the first matrix to be compared  
 *	b = A pointer which points to the second matrix to be compared
 * RETURN:
 *  true - If the matrices are equal
 *	false - If the matrices are not equal
 **/
bool equal_matrices (Matrix_t* a, Matrix_t* b) {

	if (a == NULL || b == NULL){
		printf("Error: Null pointer\n");
		return false;
	}
	
	if (!a || !b || !a->data || !b->data) {
		return false;	
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

/* 
 * PURPOSE: This fuction duplicates a matrix onto a second matrix
 * INPUTS: 
 *  src =  A pointer to a matrix that you want to duplicate
 *	dest = A pointer which points to the matix that you want src to be duplicated on
 * RETURN: 
 *  False - If the matrices were not duplicated
 *	True - If the matrices were succesfully duplicated
 **/
bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {


	if (src == NULL || dest == NULL){
		printf("Error: Null pointer\n");
		return false;
	}

	if (!src) {
		return false;
	}
	/*
	 * copy over data
	 */
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;
	memcpy(src->data,dest->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

/* 
 * PURPOSE: This fuction compares two matrices to see if they are equal
 * INPUTS: 
 *  src =  A pointer to a matrix that you want to duplicate
 *	dest = A pointer which points to the matix that you want src to be duplicated on
 * RETURN: 
 *  False - If the matrices were not duplicated
 *	True - If the matrices were succesfully duplicated
 **/
bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	if (a == NULL || direction != 'l' && direction != 'r' || shift <= 0 || shift > UINT_MAX){
		printf("Error: Invalid Input\n");
		return false;
	}

	if (!a) {
		return false;
	}

	if (direction == 'l') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

/* 
 * PURPOSE: This fuction adds two matrices and sets their values equal to a third matrice
 * INPUTS: 
 *  a = A pointer to the first matrix that the user wants to add
 *  b = A pointer to the second matrix that the user wants to add
 *	c = A pointer to the matrix which will hold the sum of the matrices a & b
 * RETURN: 
 *  False - If the matrices could not be added
 *	True - If the matrices were succesfully added
 **/
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	if (a == NULL || b == NULL || c = NULL){
		printf("Error: Null pointer\n");
		return false;
	}

	if (a->rows != b->rows && a->cols != b->cols) {
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

/* 
 * PURPOSE: This fuction will display the matrix that is passed to it using printf
 * INPUTS: 
 *  m = A pointer which points to the matrix to be displayed
 * RETURN:
 *  Nothing is returned
 *
 **/

void display_matrix (Matrix_t* m) {
	
	if (m == NULL){
		printf("Error: Null pointer\n");
		return;
	}


	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

/* 
 * PURPOSE: This fuction reads in a matrix from a file and places it in matrix m
 * INPUTS: 
 *  matrix_input_filename = A pointer to the file to read in the input
 *  m = A pointer to the matrix to be filled with data
 * RETURN: 
 *  False - If the matrices could not be read and stored
 *	True - If the matrices were succesfully read and stored
 **/
bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	
	if (matrix_input_filename == NULL || m == NULL){
		printf("Error: Null pointer\n");
		return false;
	}


	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);
	free(data);
	if (close(fd)) {
		return false;

	}
	return true;
}

/* 
 * PURPOSE: This fuction puts data from the matrix into an output file
 * INPUTS: 
 *  matrix_output_filename = A pointer to the file to write the matrix into
 *  m = A pointer to the matrix to be written into the output file
 * RETURN: 
 *  False - If the matrices could not be written into the file
 *	True - If the matrices were succesfully written into the file
 **/
bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	if (matrix_input_filename == NULL || m == NULL){
		printf("Error: Null pointer\n");
		return false;
	}


	int fd = open (matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

/* 
 * PURPOSE: This fuction inserts random numbers into a matrix between a min and a max
 * INPUTS: 
 *  m = A pointer to the matrix that the user wants to fill with random numbers
 *	start_range = The minimum value that can be input into the matrix
 *	end_range = The maximum value that can be input into the matrix
 * RETURN: 
 *  False - If the matrix was not filled with data
 *	True - If the matrix was filled with data
 **/
bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	if (m == NULL || start_range <= 0 || start_range > UINT_MAX || end_range <= 0 || end_range > UINT_MAX){
		printf("Error: Invalid Input\n");
		return false;
	}

	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % (end_range + 1 - start_range) + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

/* 
 * PURPOSE: This fuction loads data into the matrix m
 * INPUTS: 
 *  m = A pointer to the matrix that the user wants to fill with random numbers
 *	data = data to be loaded into the matrix m
 * RETURN: 
 **/
void load_matrix (Matrix_t* m, unsigned int* data) {
	
	if (m == NULL || *data <0 || *data == UINT_MAX){
		printf("Error: Invalid Input\n");
		return;
	}

	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

/* 
 * PURPOSE: This fuction adds new_matrix to an array of matrices named mats 
 * INPUTS: 
 *  mats = This is the array of matrices
 *	new_matrix = This is the matrix to be added to the array
 *	num_mats = The number of matrices in the array mats
 * RETURN: 
 *  -1 - If the matrix was not added to the array
 *	pos - If the matrix was added to the array
 **/
unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	
	if (m == NULL || new_matrix == NULL || num_mats < 0 || num_mats > UINT_MAX){
		printf("Error: Invalid Input\n");
		return -1;
	}
	static long int current_position = 0;
	const long int pos = current_position % num_mats;
	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	} 
	mats[pos] = new_matrix;
	current_position++;
	return pos;
}
