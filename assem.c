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
//#define DEBUG

int symHashFunc(Key k);
int opHashFunc(Key k);
void setOpTab(HashTable* ot);

int main(int argc, char** argv)
{
	FILE *fp;
	char str[STR_MAX];
	char regmsg[100];
	regex_t rg_lb, rg_w, rg_op; // pointer for label regex
	int rt_lb, rt_w, rt_op; // regcomp return value for label, .word, operator
	int lc = 0; // location counter
	HashTable* opTab; // operator table 
	HashTable* symTab; // symbol table


	// check parameter
	if(argc < 2) 
	{
		fprintf(stderr, "error: input string file name\n");
		exit(1);
	}

#ifndef DEBUG
	// open string file
	if((fp = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "failed to open a file\n");
		exit(1);
	}
#endif

#ifdef DEBUG
	if((fp = fopen("example1.txt", "r")) == NULL)
	{
		fprintf(stderr, "failed to open a file\n");
		exit(1);
	}
#endif

	// compile regular expression
	rt_lb = regcomp(&rg_lb, "[:]", 0); 
	rt_w = regcomp(&rg_w, "[.][w][o][r][d]", 0);
	rt_op = regcomp(&rg_op, "^[a-z]+$", 0);

	// predefined operator table
	opTab = createTable(TB_MAX, opHashFunc);
	setOpTab(opTab);

	symTab = createTable(TB_MAX, symHashFunc); // create symbol table


	// start first pass
	while(fscanf(fp, "%s", str) != EOF) { 

		rt_lb = regexec(&rg_lb, str, 0, NULL, 0); // execute regex
		if(!rt_lb) // if label
		{
			//printf("%s, %d\n", str, lc);
			//HashInsert(symTab, lc, str); // store label
			lc++; // increate location counter
		}
		else // if not label
		{
			// search optable (hash table)
			rt_w = regexec(&rg_w, str, 0, NULL, 0);
			if(!rt_w) // if .word
			{
				// add +3 to location counter
				lc += 3;
			}
			else
			{
				rt_op = regexec(&rg_op, str, 0, NULL, 0);
				if(rt_op)
				{
					//puts(str);
				}
			}	
		}

		//rt_op = regexec(&rg_op, str, 0, NULL, 0);
		//if(!rt_op)
		//{
			//puts(str);
		//}

	}

	fclose(fp);
	regfree(&rg_lb);
	regfree(&rg_w);
	regfree(&rg_op);
	free(symTab);
	free(opTab);
	// check that there is a symbol in label field
	// store symbol in symbol table
	return 0;
}

/**
 * @brief hash function for symbol table 
 * @param int $k location counter 
 * @return int
 */
int symHashFunc(Key k)
{
	return k % 20;
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
	HashInsert(ot, 0x09, "addiu");
	HashInsert(ot, 0x03, "jal");
	HashInsert(ot, 0x0d, "ori");
	HashInsert(ot, 0x21, "addu");
	HashInsert(ot, 0x08, "jr");
	HashInsert(ot, 0x0b, "sltiu");
	//HashInsert(ot, 0x24, "and");
	HashInsert(ot, 0x0f, "lui");
	//HashInsert(ot, 0x2b, "sltu");
	HashInsert(ot, 0x0c, "andi");
	//HashInsert(ot, 0x23, "lw");
	HashInsert(ot, 0x00, "sll");
	HashInsert(ot, 0x04, "beq");
	HashInsert(ot, 0x02, "srl");
	HashInsert(ot, 0x05, "bne");
	//HashInsert(ot, 0x27, "nor");
	//HashInsert(ot, 0x2b, "sw");
	HashInsert(ot, 0x02, "j");
	//HashInsert(ot, 0x25, "or");
	//HashInsert(ot, 0x23, "subu");
}
