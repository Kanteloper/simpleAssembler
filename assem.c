/*
 * Date : 2018-07-04
 * Name : Kanteloper
 * Detail : implement first pass of assembler
 */

#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#define SYM_MAX 30
#define STR_MAX 15

int main(int argc, char** argv)
{
	FILE *fp;
	char str[STR_MAX];
	char regmsg[100];
	regex_t regex;
	int reti;
	int lc = 0; // location counter
	char** symTab[SYM_MAX]; // symbol table

	// check parameter
	if(argc < 2) 
	{
		fprintf(stderr, "error: input string file name\n");
		exit(1);
	}

	// open string file
	if((fp = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "failed to open a file\n");
		exit(1);
	}

	// compile regular expression
	reti = regcomp(&regex, "[:]", 0); if(reti)
	{
		fprintf(stderr, "failed to compile regex\n");
		exit(1);
	}

	// start first pass
	while(fscanf(fp, "%s", str) != EOF) { 

		// excute regular expression
		reti = regexec(&regex, str, 0, NULL, 0);
		// check label
		if(!reti)
		{
			// store label in symbol table
		}
		
	}

	fclose(fp);
	regfree(&regex);

	// check that there is a symbol in label field
	// store symbol in symbol table
	return 0;
}
