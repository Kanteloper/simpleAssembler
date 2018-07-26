/*
 * Date : 2018-07-04
 * Name : Kanteloper
 * Brief : implement simple assembler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

#include "linkedList.h"
#include "hash.h"

#define LINE_MAX 30
#define STR_MAX 15
#define BUF_MAX 500

int symHashFunc(Key k);
int opHashFunc(Key k);
char* toBinary(char** rg, char* arg);
void set_rOpTab(HashTable* ot);
void set_iOpTab(HashTable* ot);
void set_jOpTab(HashTable* ot);

int main(int argc, char** argv)
{
	FILE *fp;
	char line[STR_MAX];
	char arg1[STR_MAX] = {0}; char arg2[STR_MAX] = {0};
	char arg3[STR_MAX] = {0}; char arg4[STR_MAX] = {0};
	int r_key[9] = { 0x21, 0x08, 0x24, 0x2b, 0x00, 0x02, 0x27, 0x25, 0x23 };
	int i_key[8] = { 0x09, 0x0b, 0x0c, 0x23, 0x04, 0x1c, 0x05, 0x2b };
	int j_key[2] = { 0x03, 0x02 };
	regex_t rg_lb; // pointer for label regex
	int rt_lb; // regcomp return value for label
	int lc = 0; // location counter
	int txt_cnt = 0; int data_cnt = 0; // number of text and data
	HashTable* rOpTab; // operator table for R format instruction
	HashTable* iOpTab; // operator table for I format instruction
	HashTable* jOpTab; // operator table for J format instruction
	HashTable* symTab; // symbol table
	char *rgst[] = { "00000", "00001", "00010", "00011", "00100", "00101",
						 "00110", "00111", "01000", "01001", "01010", "01011",
						 "01100", "01101", "01110", "01111", "10000", "10001",
						 "10010", "10011", "10100", "10101", "10110", "10111",
						 "11000", "11001", "11010", "11011", "11100", "11101",
						 "11110", "11111" }; // register 
	//char buffer[]; // buffer for file output

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
	rt_lb = regcomp(&rg_lb, "[:.]", 0); 

	// set predefined operator table
	rOpTab = createTable(TB_MAX, opHashFunc); set_rOpTab(rOpTab);
	iOpTab = createTable(TB_MAX, opHashFunc); set_iOpTab(iOpTab);
	jOpTab = createTable(TB_MAX, opHashFunc); set_jOpTab(jOpTab);

	symTab = createTable(TB_MAX, symHashFunc); // create symbol table

	// start first pass	
	while(fgets(line, LINE_MAX, fp) != NULL) // read line by line
	{
		char* label;
		sscanf(line, "%s%s%s%s", arg1, arg2, arg3, arg4 );
		rt_lb = regexec(&rg_lb, arg1, 0, NULL, 0); // execute regexec
		if(!rt_lb) 
		{
			if(strcmp(arg1, ".data") > 0 && strcmp(arg1, ".text") > 0) // if arg1 label
			{
				label = strtok(arg1, ":");
				HashInsert(symTab, lc, label); // store label
			}
			if(strcmp(arg2, ".word") == 0) // if arg2 .word
			{
				lc += 4;
				data_cnt++;
			}
		}
		else if(strcmp(arg1, ".word") == 0) // if arg1 .word
		{
			lc += 4;
			data_cnt++;
		}
		else // if operator
		{
			lc += 4;
			txt_cnt++;
		}

		arg2[0] = '\0'; // flush arg2  
	}

	fseek(fp, 0L, SEEK_SET); // reset file pointer 
	lc = 0; // reset location counter

	// start second pass
	while(fgets(line, LINE_MAX, fp) != NULL) 
	{
		sscanf(line, "%s%s%s%s", arg1, arg2, arg3, arg4 );
		for(int i = 0; i < 9; i++)
		{
			// opcode 6bits set 0
			if(HashSearch(rOpTab, r_key[i], arg1) != NULL) // if R format instruction
			{
				char opcode[6] = "000000";
				char* tmp;
				int index = 0;
				if(strcmp(arg1, "addu") == 0) // addu
				{
					puts(arg2);				
				}
				if(strcmp(arg1, "jr") == 0) // jr
				{
					puts(arg2);				
				}
				if(strcmp(arg1, "and") == 0) // and
				{
					// extract arg2, arg3, arg4 
					puts(toBinary(rgst, arg2));
				}
				if(strcmp(arg1, "sltu") == 0) // sltu
				{
					puts(arg2);				
				}
				if(strcmp(arg1, "sll") == 0) // sll
				{
					puts(arg2);				
				}
				if(strcmp(arg1, "srl") == 0) // srl
				{
					puts(arg2);				
				}
				if(strcmp(arg1, "nor") == 0) // nor
				{
					puts(arg2);				
				}
				if(strcmp(arg1, "or") == 0) // or
				{
					puts(arg2);				
				}
				if(strcmp(arg1, "subu") == 0) // subu
				{
					puts(arg2);				
				}
			}
			//else if(HashSearch(iOpTab, i_key[i], arg1) != NULL) // if I format instruction 
			//{
			//break;
			//}
			//else if(HashSearch(jOpTab, j_key[i], arg1) != NULL)// if J format instruction
			//{
			//break;
			//}
		}
		// else if I format instruction
		// else J format instructions
		arg2[0] = '\0'; // flush arg2  
	}
	// convert text section size to binary
	// convert data section size to binary
	// start to convert instructions to binary
	// check that there is a symbol in label field
	// store symbol in symbol table

	fclose(fp);
	regfree(&rg_lb);
	free(rOpTab);
	free(iOpTab);
	free(jOpTab);
	free(symTab);
	return 0;
}

/** 
 * @brief convert register number to binary
 * @param char** $rg_array for binary value of register
 * @param char* &arg
 * @return char*
 */
char* toBinary(char** rg, char* arg)
{
	int i = 0;
	char* tmp = (char*)malloc(sizeof(char) * 5);
	strcpy(tmp, arg);
	tmp = strtok(tmp, "$");
	tmp = strtok(tmp, ",");
	i = atoi(tmp);
	return rg[i];
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
 * @brief predefined R format operation table
 * @param HashTable* ot
 */
void set_rOpTab(HashTable* ot)
{
	// total 9 operators
	HashInsert(ot, 0x21, "addu");
	HashInsert(ot, 0x08, "jr");
	HashInsert(ot, 0x24, "and");
	HashInsert(ot, 0x2b, "sltu");
	HashInsert(ot, 0x00, "sll");
	HashInsert(ot, 0x02, "srl");
	HashInsert(ot, 0x27, "nor");
	HashInsert(ot, 0x25, "or");
	HashInsert(ot, 0x23, "subu");
}

/**
 * @brief predefined I format operation table
 * @param HashTable* ot
 */
void set_iOpTab(HashTable* ot)
{
	// total 8 operators
	HashInsert(ot, 0x09, "addiu");
	HashInsert(ot, 0x0b, "sltiu");
	HashInsert(ot, 0x0c, "andi");
	HashInsert(ot, 0x23, "lw");
	HashInsert(ot, 0x04, "beq");
	HashInsert(ot, 0x05, "bne");
	HashInsert(ot, 0x2b, "sw");
	HashInsert(ot, 0x1c, "la");
}

/**
 * @brief predefined J format operation table
 * @param HashTable* ot
 */
void set_jOpTab(HashTable* ot)
{
	// total 2 operators
	HashInsert(ot, 0x03, "jal");
	HashInsert(ot, 0x02, "j");
}
