/*
 * Date : 2018-07-04
 * Name : Kanteloper
 * Detail : implement first pass of assembler
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include "linkedList.h"
#include "hash.h"
#define SYM_MAX 30
#define STR_MAX 15

int symHashFunc(Key k);
int opHashFunc(Key k);
void setOpTab(HashTable* ot);

int main(int argc, char** argv)
{
	FILE *fp;
	char str[STR_MAX];
	// key for opTab
	int opKey[22] = { 0x01, 0x09, 0x03, 0x0d, 0x21, 0x08, 0x0b, 0x24, 0x0f, 
		0x2b, 0x0c, 0x23, 0x00, 0x04, 0x02, 0x05, 0x27, 0x2b, 0x02, 0x25, 0x23, 0x1c};
	char regmsg[100];
	regex_t rg_lb; // pointer for label regex
	int rt_lb; // regcomp return value for label
	int lc = 0; // location counter
	HashTable* opTab; // operator table

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

	// create predefined operator table
	opTab = createTable(TB_MAX, opHashFunc);
	setOpTab(opTab);

	// start first pass
	while(fscanf(fp, "%s", str) != EOF) { 

		rt_lb = regexec(&rg_lb, str, 0, NULL, 0); // execute regex
		if(!rt_lb) // if label
		{
			printf("%s, %d\n", str, lc);
			lc++; // increate location counter
		}

		for(int i = 0; i < 22; i++)
		{
			// search optable
			if(strcmp(str, HashSearch(opTab, opKey[i])) == 0) { 
				printf("%s, %d\n", str, lc);
				// if .word
				if(strcmp(str, ".word") == 0)
					lc += 3;
				else // other operators
					lc += 4;
			}
		}



	}

	fclose(fp);
	regfree(&rg_lb);
	free(opTab);
	// check that there is a symbol in label field
	// store symbol in symbol table
	return 0;
}

/** 
 * @brief hash function for operator table
 * @param int $k operation code
 * @return int
 */
int opHashFunc(Key k)
{
	return k | 0x00; 
}

/**
 * @brief predefined operation table
 * @param HashTable* ot
 */
void setOpTab(HashTable* ot)
{
	// total 22 operators
	HashInsert(ot, 0x01, ".word");
	HashInsert(ot, 0x09, "addiu");
	HashInsert(ot, 0x03, "jal");
	HashInsert(ot, 0x0d, "ori");
	HashInsert(ot, 0x21, "addu");
	HashInsert(ot, 0x08, "jr");
	HashInsert(ot, 0x0b, "sltiu");
	HashInsert(ot, 0x24, "and");
	HashInsert(ot, 0x0f, "lui");
	HashInsert(ot, 0x2b, "sltu");
	HashInsert(ot, 0x0c, "andi");
	HashInsert(ot, 0x23, "lw");
	HashInsert(ot, 0x00, "sll");
	HashInsert(ot, 0x04, "beq");
	HashInsert(ot, 0x02, "srl");
	HashInsert(ot, 0x05, "bne");
	HashInsert(ot, 0x27, "nor");
	HashInsert(ot, 0x2b, "sw");
	HashInsert(ot, 0x02, "j");
	HashInsert(ot, 0x25, "or");
	HashInsert(ot, 0x23, "subu");
	HashInsert(ot, 0x1c, "la");
}
