/*
 * Date : 2018-07-04
 * Name : Kanteloper
 * Brief : implement simple assembler
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include "linkedList.h"
#include "hash.h"

#define LINE_MAX 30
#define STR_MAX 20
#define BUF_MAX 1000 

typedef struct _r_struct
{
	unsigned int op;
	unsigned int rs;
	unsigned int rt;
	unsigned int rd;
	unsigned int shamt;
	unsigned int funct;
} format_R;

typedef struct _i_struct
{
	unsigned int op;
	unsigned int rs;
	unsigned int rt;
	short immd;
} format_I;

typedef struct _j_struct
{
	char op[7];
	char addr[27];
} format_J;

int symHashFunc(Key k);
int opHashFunc(Key k);
int convert_to_int(char* arg);
void set_rOpTab(HashTable* ot);
void set_iOpTab(HashTable* ot);
void set_jOpTab(HashTable* ot);
char* make_r_format_binary(format_R* fr);
char* make_i_format_binary(format_I* fi);
char* OffsetToBin(int arg);
char* SizeToBin(int arg);
int isRformat(HashTable* ht, int* rk, char* arg);
int isIformat(HashTable* ht, int* ik, char* arg);
int isJformat(HashTable* ht, int* jk, char* arg);
int isOperand(HashTable* ht, int num, char* oprn);
int strToInt(char* arg);
char* getImmediate(char* arg);
char* getRegister(char* arg);

int main(int argc, char** argv)
{
	FILE *fp;
	char line[STR_MAX];
	char arg1[STR_MAX] = {};
	char arg2[STR_MAX] = {}; 
	char arg3[STR_MAX] = {}; 
	char arg4[STR_MAX] = {};
	int r_key[9] = { 0x21, 0x08, 0x24, 0x2b, 0x00, 0x02, 0x27, 0x25, 0x23 };
	int i_key[9] = { 0x09, 0x0b, 0x0c, 0x23, 0x04, 0x05, 0x2b, 0x0d, 0x0f };
	int j_key[2] = { 0x03, 0x02 };
	regex_t rg_lb; // pointer for label regex
	int rt_lb; // regcomp return value for label
	int lc_data = 0; // location counter for data section
	int lc_text = 0; // location counter for text section
	HashTable* rOpTab; // operator table for R format instruction
	HashTable* iOpTab; // operator table for I format instruction
	HashTable* jOpTab; // operator table for J format instruction
	HashTable* symTab; // symbol table
	char buffer[BUF_MAX] = {}; // buffer for file output
	char* binary; // for binary code
	format_R fr;
	format_I fi;
	format_J fj;

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
		if(!rt_lb) // if arg1 label 
		{
			if(strcmp(arg1, ".data") > 0 && strcmp(arg1, ".text") > 0)
			{
				label = strtok(arg1, ":");
				if(strcmp(arg2, ".word") == 0 || strcmp(arg1, ".word") == 0) // if label in data section 
				{	
					//printf("%s, %d\n", arg1, lc_data);

					//convert default value of data to binary 
					//save those to another buffer


					HashInsert(symTab, lc_data, label); // store label
					lc_data += 4;
				}
				else // if label in text section
				{
					//printf("%s, %d\n", arg1, lc_text);
					HashInsert(symTab, lc_text, label); // store label
				}
			}
		}
		else // if operator
		{
			// if the lower 16bits address is not 0x0000
			if(strcmp(arg1, "la") == 0 && getHashAddr(symTab, 0, arg3) != 0)
			{
				//printf("%s, %d\n", arg1, lc_text);
				lc_text += 8;
			}
			else
			{
				//printf("%s, %d\n", arg1, lc_text);
				lc_text += 4;
			}
		}

		arg2[0] = '\0'; // flush arg2  
	}

	// convert data and text section size to binary
	// And, save those to buffer
	binary = SizeToBin(lc_text);
	strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
	free(binary);
	binary = SizeToBin(lc_data);
	strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
	free(binary);

	fseek(fp, 0L, SEEK_SET); // reset file pointer 
	lc_text = 0; // reset location counter

	// start second pass
	while(fgets(line, LINE_MAX, fp) != NULL) 
	{
		int idx = -1;
		int b_target; // address of branch target
		//char* tmp;
		//char* tmp_immd;  
		//char* tmp_reg;

		sscanf(line, "%s%s%s%s", arg1, arg2, arg3, arg4 );
		//printf("%s, %d\n", arg1, lc_text);
		rt_lb = regexec(&rg_lb, arg1, 0, NULL, 0); // execute regexec
		if(rt_lb) // if arg1 label
		{
			// search each opTab
			if((idx = isRformat(rOpTab, r_key, arg1)) != -1) // if R format instruction
			{
				switch(r_key[idx])
				{
					//convert each instruction to binary
					case 0: // sll
						// only unsigned operations
						fr.op = 0;
						fr.rs = 0;
						fr.rt = convert_to_int(arg3);
						fr.rd = convert_to_int(arg2);
						fr.shamt = convert_to_int(arg4);
						fr.funct = 0;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						free(binary);
						break;

					case 2: // srl
						// only unsigned operations
						fr.op = 0;
						fr.rs = 0;
						fr.rt = convert_to_int(arg3);
						fr.rd = convert_to_int(arg2);
						fr.shamt = convert_to_int(arg4);
						fr.funct = 2;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						free(binary);
						break;

					case 8: // jr
						fr.op = 0;
						fr.rs = convert_to_int(arg2);
						fr.rt = 0;
						fr.rd = 0;
						fr.shamt = 0;
						fr.funct = 8;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						free(binary);
						break;

					case 33: // addu
						// only unsigned operations
						fr.op = 0;
						fr.rs = convert_to_int(arg3);
						fr.rt = convert_to_int(arg4);
						fr.rd = convert_to_int(arg2);
						fr.shamt = 0;
						fr.funct = 33;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						free(binary);
						break;

					case 35: // subu
						//// only unsigned operations
						fr.op = 0;
						fr.rs = convert_to_int(arg3);
						fr.rt = convert_to_int(arg4);
						fr.rd = convert_to_int(arg2);
						fr.shamt = 0;
						fr.funct = 35;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						free(binary);
						break;

					case 36: // and
						fr.op = 0;
						fr.rs = convert_to_int(arg3);
						fr.rt = convert_to_int(arg4);
						fr.rd = convert_to_int(arg2);
						fr.shamt = 0;
						fr.funct = 36;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						free(binary);
						break;

					case 37: // or
						fr.op = 0;
						fr.rs = convert_to_int(arg3);
						fr.rt = convert_to_int(arg4);
						fr.rd = convert_to_int(arg2);
						fr.shamt = 0;
						fr.funct = 37;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						free(binary);
						break;

					case 39: // nor
						fr.op = 0;
						fr.rs = convert_to_int(arg3);
						fr.rt = convert_to_int(arg4);
						fr.rd = convert_to_int(arg2);
						fr.shamt = 0;
						fr.funct = 39;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						free(binary);
						break;

					case 43: // sltu
						// only unsigned operations
						fr.op = 0;
						fr.rs = convert_to_int(arg3);
						fr.rt = convert_to_int(arg4);
						fr.rd = convert_to_int(arg2);
						fr.shamt = 0;
						fr.funct = 43;
						binary = make_r_format_binary(&fr);
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						free(binary);
						break;
				}
			} // r format opTab search end
			else if((idx = isIformat(iOpTab, i_key, arg1)) != -1) // if I format instruction 
			{
				switch(i_key[idx])
				{
					case 4: // beq
						//immediate field are sign extended to allow negative
						// search symbol as operand
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) // if found
						{
							fi.op = 4;
							fi.rs = convert_to_int(arg2);
							fi.rt = convert_to_int(arg3);
							fi.immd = ((b_target - lc_text - 4) / 4);
							binary = make_i_format_binary(&fi);
						}
						else // if not found
						{
							/** 
							 * error handling when there is no matched operand in symbol table 
							 * set 0 as address of operand & alert error flag to user
							 */
							fi.op = 4;
							fi.rs = convert_to_int(arg2);
							fi.rt = convert_to_int(arg3);
							fi.immd = 0;
							binary = make_i_format_binary(&fi);
							fprintf(stderr, "Error: There is no matched operand in symbol table: %s\n", arg1);
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						free(binary);
						break;

					case 5: // bne
						//immediate field are sign extended to allow negative
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1)
						{
							fi.op = 5;
							fi.rs = convert_to_int(arg2);
							fi.rt = convert_to_int(arg3);
							fi.immd = ((b_target - lc_text - 4) / 4);
							binary = make_i_format_binary(&fi);
						}
						else  
						{
							fi.op = 5;
							fi.rs = convert_to_int(arg2);
							fi.rt = convert_to_int(arg3);
							fi.immd = 0;
							binary = make_i_format_binary(&fi);
							fprintf(stderr, "Error: There is no matched operand in symbol table: %s\n", arg1);
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						free(binary);
						break;
					case 9: // addiu
						// only unsigned operations
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1)
						{
							fi.op = 9;
							fi.rs = convert_to_int(arg3);
							fi.rt = convert_to_int(arg2);
							fi.immd = ((b_target - lc_text - 4) / 4);
							binary = make_i_format_binary(&fi);
						}
						else // if constant 
						{
							fi.op = 9;
							fi.rs = convert_to_int(arg3);
							fi.rt = convert_to_int(arg2);
							fi.immd = strToInt(arg4);
							binary = make_i_format_binary(&fi);
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						free(binary);
						break;
					case 11: // sltiu
						// only unsigned operations
						//if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) 
						//{
						//binary = makeIformBinary("001011", RegToBin(arg3), RegToBin(arg2), 
						//OffsetToBin((b_target - lc_text - 4) / 4));
						//}
						//else
						//{
						//binary = makeIformBinary("001011", RegToBin(arg3), RegToBin(arg2), 
						//OffsetToBin(strToInt(arg4)));
						//}
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						//free(binary);
						break;
					case 12: // andi
						//if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) 
						//{
							//binary = makeIformBinary("001100", RegToBin(arg3), RegToBin(arg2), 
									//OffsetToBin((b_target - lc_text - 4) / 4));
						//}
						//else 
						//{
							//binary = makeIformBinary("001100", RegToBin(arg3), RegToBin(arg2), 
									//OffsetToBin(strToInt(arg4)));
						//}
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						//free(binary);
						break;
					case 13: // ori
						//if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) 
						//{
							//binary = makeIformBinary("001101", RegToBin(arg3), RegToBin(arg2), 
									//OffsetToBin((b_target - lc_text - 4) / 4));
						//}
						//else 
						//{
							//binary = makeIformBinary("001101", RegToBin(arg3), RegToBin(arg2), 
									//OffsetToBin(strToInt(arg4)));
						//}
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						//free(binary);
						break;
					case 15: // lui
						//if((b_target = isOperand(symTab, TB_MAX, arg3)) != -1) 
						//{
							//binary = makeIformBinary("001111", "00000", RegToBin(arg2), 
									//OffsetToBin((b_target - lc_text - 4) / 4));
						//}
						//else 
						//{
							//binary = makeIformBinary("001111", "00000", RegToBin(arg2), 
									//OffsetToBin(strToInt(arg3)));
						//}
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						//free(binary);
						break;
					case 35: // lw
						//immediate field are sign extended to allow negative
						//tmp_immd = getImmediate(arg3); // filter arg3 for immmediate
						//tmp_reg = getRegister(arg3); // filter arg3 for register
						//binary = makeIformBinary("100011", RegToBin(tmp_reg) , RegToBin(arg2),	OffsetToBin(strToInt(tmp_immd)));
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						//free(binary);
						break;
					case 43: // sw
						//immediate field are sign extended to allow negative
						//tmp_immd = getImmediate(arg3);  filter arg3 for immmediate
							//tmp_reg = getRegister(arg3);  filter arg3 for register
							//binary = makeIformBinary("101011", RegToBin(tmp_reg) , RegToBin(arg2),	OffsetToBin(strToInt(tmp_immd)));
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						//free(binary);
						break;
				}
			} // i format table search end
			else if((idx = isJformat(jOpTab, j_key, arg1)) != -1) // if I format instruction 
			{
				int address = 0;
				switch(j_key[idx])
				{
					case 2: // j
						//if((address = isOperand(symTab, TB_MAX, arg2)) != -1) // if found in symTab
						//{
							//binary = makeIformBinary("000010", "00000" , "10000", OffsetToBin(address >> 2));
							//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						//}
						//else // if not found
						//{
							//// error
							//binary = makeIformBinary("000010", "00000" , "10000", OffsetToBin(0));
							//fprintf(stderr, "Error: There is no match in symbol table: %s\n", arg1);
						//}
						lc_text += 4;
						//free(binary);
						break;

					case 3: // jal
						//if((address = isOperand(symTab, TB_MAX, arg2)) != -1) // if found in symTab
						//{
							//binary = makeIformBinary("000011", "00000" , "10000", OffsetToBin(address >> 2));
							//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						//}
						//else // if not found
						//{
							//// error
							//binary = makeIformBinary("000011", "00000" , "10000", OffsetToBin(0));
							//fprintf(stderr, "Error: There is no match in symbol table: %s\n", arg1);
						//}
						lc_text += 4;
						//free(binary);
						break;
				}
			} // j format table search end
			else // la
			{
				if((b_target = isOperand(symTab, TB_MAX, arg3)) != -1) // if operand is symbol
				{
					if(b_target != 0) // if lower 16bits not 0x0000
					{
						// lui
						//binary = makeIformBinary("001111", "00000", RegToBin(arg2), "0001000000000000");
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						//// ori
						//binary = makeIformBinary("001101", RegToBin(arg2), RegToBin(arg2), OffsetToBin(b_target));
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 8;
						//free(binary);
					}
					else // if lower 16bits 0x0000
					{
						// lui
						//binary = makeIformBinary("001111", "00000", RegToBin(arg2), "0001000000000000");
						//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						//free(binary);
					}
				}
				else // error 
				{
					//binary = makeIformBinary("000000", "00000", "00000", OffsetToBin(0));
					//strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
					//fprintf(stderr, "Error: There is no matched symbol or address is wrong\n");
					lc_text += 4;
					//free(binary);
				}
			}
		} // done operator found

		arg2[0] = '\0'; // flush arg2  
	} // while end


	// append to default data value  buffer

	puts(buffer);

	// output object file

	fclose(fp);
	regfree(&rg_lb); // free regex
	freeHashList(TB_MAX, rOpTab->list); // free linkedList in rOpTab
	free(rOpTab); // free rOpTab HashTable
	freeHashList(TB_MAX, iOpTab->list); // free linkedList in iOpTab
	free(iOpTab);
	freeHashList(TB_MAX, jOpTab->list); // free linkedList in jOpTab
	free(jOpTab);
	freeHashList(TB_MAX, symTab->list); // free linkedList in symTab
	free(symTab); // free symTab HashTable
	return 0;
}

/**
 * @brief get register string from argument
 * @param char* $arg 
 * @return char*
 */
char* getRegister(char* arg)
{
	char* tmp = malloc(sizeof(char) * (strlen(arg) + 1));
	strncpy(tmp, arg, (strlen(arg) + 1));
	tmp = strtok(tmp, "(");
	tmp = strtok(NULL, "(");
	return tmp;
}

/**
 * @brief get immediate string from argument
 * @param char* $arg
 * @return char*
 */
char* getImmediate(char* arg)
{
	char* tmp = malloc(sizeof(char) * (strlen(arg) + 1));
	strncpy(tmp, arg, (strlen(arg) + 1));
	tmp = strtok(tmp, "(");
	return tmp;
}

/**
 * @brief make R instruction format binary code 
 * @param format_R* $fr 
 * @return char*
 */
char* make_r_format_binary(format_R* fr)
{
	char* bin = (char*)malloc(sizeof(char) * 33);
	unsigned int tmp = fr->op << 26 | fr->rs << 21 | fr->rt << 16 |
		fr->rd << 11 | fr->shamt << 6 | fr->funct;
	for(int i = 31; i >= 0; i--)
	{
		bin[i] = (tmp & 1) + '0'; 
		tmp >>= 1;
	}
	bin[32] = '\0';
	return bin;
}

/**
 * @brief make I instruction format binary code
 * @param format_I* $fi 
 * @return char*
 */
char* make_i_format_binary(format_I* fi)
{
	char* bin = (char*)malloc(sizeof(char) * 33);
	unsigned int tmp = fi->op << 26 | fi->rs << 21 | fi->rt << 16 | fi->immd & 0xffff;

	for(int i = 31; i >= 0; i--)
	{
		bin[i] = (tmp & 1) + '0'; 
		tmp >>= 1;
	}
	bin[32] = '\0';
	return bin;
}

/**
 * @brief check whether the operand is in symbol table
 * @param HashTable* $ht
 * @param int $num
 * @param char* oprn
 * @return int
 */
int isOperand(HashTable* ht, int num, char* oprn)
{
	int target = 0;
	for(int i = 0; i < num; i++)
	{
		if((target = getHashAddr(ht, i, oprn)) != -1) return target; // if found
	}
	return -1;
}

/**
 * @brief check the operator is R format
 * @param HashTable* $ht rOpTab
 * @param int* $rk r_key array
 * @param char* $arg
 * @return int
 */
int isRformat(HashTable* ht, int* rk, char* arg)
{
	for(int i = 0; i < 9; i++)
	{
		if(HashSearch(ht, rk[i], arg) != NULL) return i;
	}
	return -1;
}

/**
 * @brief check the operator is I format
 * @param HashTable* $ht iOpTab
 * @param int* $ik i_key array
 * @param char* arg
 * @return int
 */
int isIformat(HashTable* ht, int* ik, char* arg)
{
	for(int i = 0; i < 9; i++)
	{
		if(HashSearch(ht, ik[i], arg) != NULL) return i;
	}
	return -1;
}

/**
 * @brief check the operator is J format
 * @param HashTable* $ht jOpTab
 * @param int* $jk j_key array
 * @param char* arg
 * @return int
 */
int isJformat(HashTable* ht, int* jk, char* arg)
{
	for(int i = 0; i < 2; i++)
	{
		if(HashSearch(ht, jk[i], arg) != NULL) return i;
	}
	return -1;
}

/**
 * @brief convert string to decimal or hexadecimal integer
 * @param char* arg
 * @return int
 */
int strToInt(char* arg)
{
	regex_t rg_hex;
	int rt = regcomp(&rg_hex, "[0][x]", 0);
	rt = regexec(&rg_hex, arg, 0, NULL, 0); // execute regexec
	if(!rt) // if hex
		return (int)strtol(arg, NULL, 16);
	else // if dec
		return (int)strtol(arg, NULL, 10);
}

/**
 * @brief return integer input to string represented binary 
 *		  for offset
 * @param int $tl target location counter
 * @return void
 */
char* OffsetToBin(int arg)
{
	char* tmp = (char*)malloc(sizeof(char) * 17);
	for(int i = 15; i >= 0; i--)
	{
		tmp[i] = (arg & 1) + '0'; 
		arg >>= 1;
	}
	tmp[16] = '\0';
	return tmp;
}

/**
 * @brief return integer input to string represented binary 
 *		  for the size of data and text section
 * @param int $tl target location counter
 * @return void
 */
char* SizeToBin(int arg)
{
	char* tmp = (char*)malloc(sizeof(char) * 33);
	for(int i = 31; i >= 0; i--)
	{
		tmp[i] = (arg & 1) + '0'; 
		arg >>= 1;
	}
	tmp[32] = '\0';
	return tmp;
}


/** 
 * @brief convert register number to binary
 * @param char** $rg_array for binary value of register
 * @param char* $arg
 * @return int
 */
int convert_to_int(char* arg)
{
	int nReg = 0;
	arg = strtok(arg, "$");
	arg = strtok(arg, ",");
	arg = strtok(arg, ")");
	nReg = atoi(arg);
	return nReg;
}

/**
 * @brief hash function for symbol table 
 * @param int $k location counter 
 * @return int
 */
int symHashFunc(Key k)
{
	return k % 21;
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
	// total 10 operators
	HashInsert(ot, 0x09, "addiu");
	HashInsert(ot, 0x0b, "sltiu");
	HashInsert(ot, 0x0c, "andi");
	HashInsert(ot, 0x23, "lw");
	HashInsert(ot, 0x04, "beq");
	HashInsert(ot, 0x05, "bne");
	HashInsert(ot, 0x2b, "sw");
	HashInsert(ot, 0x0d, "ori");
	HashInsert(ot, 0x0f, "lui");
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
