#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE *fp;

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

	puts("success to open a file");
	
}
