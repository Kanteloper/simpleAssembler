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
	regex_t rg_lb; // pointer for label regex
	regex_t rg_w; 
	int rt_lb; // regcomp return value for label
	int rt_w; // regcomp return value for .word
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
	rt_lb = regcomp(&rg_lb, "[:]", 0); 
	rt_w = regcomp(&rg_w, "[.][w][o][r][d]", 0);

	// create symbol table
	symTab = createTable(TB_MAX, MyHashFunc);

	// start first pass
	while(fscanf(fp, "%s", str) != EOF) { 

		rt_lb = regexec(&rg_lb, str, 0, NULL, 0); // execute regex
		if(!rt_lb) // if label
		{
			printf("%s, %d\n", str, lc);
			//HashInsert(symTab, lc, str); // store label
			lc++; // increate location counter
		}

		rt_w = regexec(&rg_w, str, 0, NULL, 0);
		if(!rt_w) // if .word
		{
			// add +3 to location counter
			lc += 3;
		}
	}

	fclose(fp);
	regfree(&rg_lb);

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

