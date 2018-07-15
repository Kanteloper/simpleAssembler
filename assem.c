/*
 * Date : 2018-07-04
 * Name : Kanteloper
 * Detail : implement first pass of assembler
 */

#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include "linkedList.h"
#include "hash.h"
#define SYM_MAX 30
#define STR_MAX 15

int MyHashFunc(int k);

int main(int argc, char** argv)
{
	FILE *fp;
	char str[STR_MAX];
	char regmsg[100];
	regex_t regex;
	int reti;
	int lc = 0; // location counter
	HashTable* symTab; // symbol table

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

	// create symbol table
	symTab = createTable(TB_MAX, MyHashFunc);

	// start first pass
	while(fscanf(fp, "%s", str) != EOF) { 

		reti = regexec(&regex, str, 0, NULL, 0); // execute regex
		if(!reti) // if label
		{
			HashInsert(symTab, lc, str); // store label
			lc++; // increate location counter
		}
		else // if not label
		{
			if(feof(fp)) break; // if EOF, go to second pass
			lc++;
		}

		// search .word
		// if .word, add 4 location counter
		
	}

	fclose(fp);
	regfree(&regex);

	// check that there is a symbol in label field
	// store symbol in symbol table
	return 0;
}

/**
 *@brief my hash function
 *@param int $k my hash key
 *@return int
 */
int MyHashFunc(int k)
{
	return k % 20;
}

