/*
 * Date : 2018-07-04
 * Name : Kanteloper
 * Detail : implement first pass of assembler
 */

#include <stdio.h>
#include <stdlib.h>
#define SYM_MAX 30

int main(int argc, char** argv)
{
	FILE *fp;
	int lc = 0; // location counter
	char** symTab[SYM_MAX]; // symbol table

	// check parameter
	if(argc < 2) 
	{
		perror("error: input string file name");
		exit(1);
	}
	// open string file
	fp = fopen(argv[1], "r");
	if(fp == NULL)
	{
		perror("failed to open a file\n");
		exit(1);
	}

	// start first pass
	while(1)
	{
		
	}

	// check that there is a symbol in label field
	// store symbol in symbol table
	

	
}
