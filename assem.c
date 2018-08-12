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
	char op[7];
	char rs[6];
	char rt[6];
	char rd[6];
	char shamt[6];
	char funct[7];
} format_R;

typedef struct _i_struct
{
	char op[7];
	char rs[6];
	char rt[6];
	char immd[17];
} format_I;

typedef struct _j_struct
{
	char op[7];
	char addr[27];
} format_J;

int symHashFunc(Key k);
int opHashFunc(Key k);
char* RegToBin(char* arg);
void set_rOpTab(HashTable* ot);
void set_iOpTab(HashTable* ot);
void set_jOpTab(HashTable* ot);
char* makeRformBinary(char* op, char* rs, char* rt, char* rd, char* shamt, char* func);
char* makeIformBinary(char* op, char* rs, char* rt, char* immd);
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
	binary = SizeToBin(lc_data);
	strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));

	fseek(fp, 0L, SEEK_SET); // reset file pointer 
	lc_text = 0; // reset location counter

	// start second pass
	while(fgets(line, LINE_MAX, fp) != NULL) 
	{
		int idx = -1;
		int b_target; // address of branch target
		char* tmp;
		char* tmp_immd;  
		char* tmp_reg;

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
						binary = makeRformBinary("000000", "00000", RegToBin(arg3), RegToBin(arg2), 
								RegToBin(arg4), "000000");  
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						break;

					case 2: // srl
						// only unsigned operations
						binary = makeRformBinary("000000", "00000", RegToBin(arg3),
								RegToBin(arg2), RegToBin(arg4), "000010");  
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						break;

					case 8: // jr
						binary = makeRformBinary("000000", RegToBin(arg2), "00000", "00000",
								"00000", "001000");
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;

					case 33: // addu
						// only unsigned operations
						binary = makeRformBinary("000000", RegToBin(arg3), RegToBin(arg4),
								RegToBin(arg2), "00000", "100001");  
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;

					case 35: // subu
						// only unsigned operations
						binary = makeRformBinary("000000", RegToBin(arg3), RegToBin(arg4),
								RegToBin(arg2), "00000", "100011");
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						break;

					case 36: // and
						binary = makeRformBinary("000000", RegToBin(arg3), RegToBin(arg4),
								RegToBin(arg2), "00000", "100100");  
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;

					case 37: // or
						binary = makeRformBinary("000000", RegToBin(arg3), RegToBin(arg4),
								RegToBin(arg2), "00000", "100101");
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						break;

					case 39: // nor
						binary = makeRformBinary("000000", RegToBin(arg3), RegToBin(arg4),
								RegToBin(arg2), "00000", "100111");
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1));
						lc_text += 4;
						break;

					case 43: // sltu
						// only unsigned operations
						binary = makeRformBinary("000000", RegToBin(arg3), RegToBin(arg4),
								RegToBin(arg2), "00000", "101011");  
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
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
							binary = makeIformBinary("000100", RegToBin(arg2), RegToBin(arg3), 
									OffsetToBin((b_target - lc_text - 4) / 4));
						}
						else // if not found
						{
							/* error handling when there is no matched operand in symbol table 
							 * set 0 as address of operand & alert error flag to user
							 */
							binary = makeIformBinary("000100", RegToBin(arg2), RegToBin(arg3),
									OffsetToBin(0));
							fprintf(stderr, "Error: There is no matched operand in symbol table: %s\n", arg1);
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 5: // bne
						//immediate field are sign extended to allow negative
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1)
						{
							binary = makeIformBinary("000101", RegToBin(arg2), RegToBin(arg3), 
									OffsetToBin((b_target - lc_text - 4) / 4));
						}
						else  
						{
							binary = makeIformBinary("000101", RegToBin(arg2), RegToBin(arg3), 
									OffsetToBin(0));
							fprintf(stderr, "Error: There is no matched operand in symbol table: %s\n", arg1);
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 9: // addiu
						// only unsigned operations
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) 
						{
							binary = makeIformBinary("001001", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin((b_target - lc_text - 4) / 4));
						}
						else // if constant
						{
							binary = makeIformBinary("001001", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin(strToInt(arg4)));
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 11: // sltiu
						// only unsigned operations
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) 
						{
							binary = makeIformBinary("001011", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin((b_target - lc_text - 4) / 4));
						}
						else
						{
							binary = makeIformBinary("001011", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin(strToInt(arg4)));
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 12: // andi
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) 
						{
							binary = makeIformBinary("001100", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin((b_target - lc_text - 4) / 4));
						}
						else 
						{
							binary = makeIformBinary("001100", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin(strToInt(arg4)));
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 13: // ori
						if((b_target = isOperand(symTab, TB_MAX, arg4)) != -1) 
						{
							binary = makeIformBinary("001101", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin((b_target - lc_text - 4) / 4));
						}
						else 
						{
							binary = makeIformBinary("001101", RegToBin(arg3), RegToBin(arg2), 
									OffsetToBin(strToInt(arg4)));
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 15: // lui
						if((b_target = isOperand(symTab, TB_MAX, arg3)) != -1) 
						{
							binary = makeIformBinary("001111", "00000", RegToBin(arg2), 
									OffsetToBin((b_target - lc_text - 4) / 4));
						}
						else 
						{
							binary = makeIformBinary("001111", "00000", RegToBin(arg2), 
									OffsetToBin(strToInt(arg3)));
						}
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 35: // lw
						//immediate field are sign extended to allow negative
						tmp_immd = getImmediate(arg3); // filter arg3 for immmediate
						tmp_reg = getRegister(arg3); // filter arg3 for register
						binary = makeIformBinary("100011", RegToBin(tmp_reg) , RegToBin(arg2),	OffsetToBin(strToInt(tmp_immd)));
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
					case 43: // sw
						//immediate field are sign extended to allow negative
						tmp_immd = getImmediate(arg3); // filter arg3 for immmediate
						tmp_reg = getRegister(arg3); // filter arg3 for register
						binary = makeIformBinary("101011", RegToBin(tmp_reg) , RegToBin(arg2),	OffsetToBin(strToInt(tmp_immd)));
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
						break;
				}
			} // i format table search end
			else if((idx = isJformat(jOpTab, j_key, arg1)) != -1) // if I format instruction 
			{
				int address = 0;
				switch(j_key[idx])
				{
					case 2: // j
						if((address = isOperand(symTab, TB_MAX, arg2)) != -1) // if found in symTab
						{
							binary = makeIformBinary("000010", "00000" , "10000", OffsetToBin(address >> 2));
							strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						}
						else // if not found
						{
							// error
							binary = makeIformBinary("000010", "00000" , "10000", OffsetToBin(0));
							fprintf(stderr, "Error: There is no match in symbol table: %s\n", arg1);
						}
						lc_text += 4;
						break;

					case 3: // jal
						if((address = isOperand(symTab, TB_MAX, arg2)) != -1) // if found in symTab
						{
							binary = makeIformBinary("000011", "00000" , "10000", OffsetToBin(address >> 2));
							strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						}
						else // if not found
						{
							// error
							binary = makeIformBinary("000011", "00000" , "10000", OffsetToBin(0));
							fprintf(stderr, "Error: There is no match in symbol table: %s\n", arg1);
						}
						lc_text += 4;
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
						binary = makeIformBinary("001111", "00000", RegToBin(arg2), "0001000000000000");
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						// ori
						binary = makeIformBinary("001101", RegToBin(arg2), RegToBin(arg2), OffsetToBin(b_target));
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 8;
					}
					else // if lower 16bits 0x0000
					{
						// lui
						binary = makeIformBinary("001111", "00000", RegToBin(arg2), "0001000000000000");
						strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
						lc_text += 4;
					}
				}
				else // error 
				{
					binary = makeIformBinary("000000", "00000", "00000", OffsetToBin(0));
					strncat(buffer, binary, (strlen(buffer) + strlen(binary) + 1)); 
					fprintf(stderr, "Error: There is no matched symbol or address is wrong\n");
					lc_text += 4;
				}
			}
		} // done operator found

		arg2[0] = '\0'; // flush arg2  
	} // while end

	puts(buffer);

	// convert text section size to binary
	// convert data section size to binary
	// convert initialized data value to binary
	// append to buffer
	// output object file

	fclose(fp);
	regfree(&rg_lb);
	free(rOpTab);
	free(iOpTab);
	free(jOpTab);
	free(symTab);
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
 * @param char* $op 
 * @param char* $rs
 * @param char* $rt
 * @param char* $rd
 * @param char* $shamt
 * @param char* $func 
 * @return char*
 */
char* makeRformBinary(char* op, char* rs, char* rt, char* rd, char* shamt, char* func)
{
	format_R fr; // structure for format R instruction
	char* bin = (char*)malloc(sizeof(char) * 33);
	strncpy(fr.op, op, 7);
	strncpy(bin, fr.op, 7);
	strncpy(fr.rs, rs, 6);
	strncat(bin, fr.rs, (strlen(bin) + strlen(fr.rs) + 1));
	strncpy(fr.rt, rt, 6);
	strncat(bin, fr.rt, (strlen(bin) + strlen(fr.rt) + 1));
	strncpy(fr.rd, rd, 6);
	strncat(bin, fr.rd, (strlen(bin) + strlen(fr.rd) + 1));
	strncpy(fr.shamt, shamt, 6);
	strncat(bin, fr.shamt, (strlen(bin) + strlen(fr.shamt) + 1));
	strncpy(fr.funct, func, 7);
	strncat(bin, fr.funct, (strlen(bin) + strlen(fr.funct) + 1));
	return bin;
}

/**
 * @brief make I instruction format binary code
 * @param char* $op 
 * @param char* $rs
 * @param char* $rt
 * @param char* $imme
 * @return char*
 */
char* makeIformBinary(char* op, char* rs, char* rt, char* immd)
{
	format_I fi; // structure for format I instruction
	char* bin = (char*)malloc(sizeof(char) * 33);
	strncpy(fi.op, op, 7);
	strncpy(bin, fi.op, 7);
	strncpy(fi.rs, rs, 6);
	strncat(bin, fi.rs, (strlen(bin) + strlen(fi.rs) + 1));
	strncpy(fi.rt, rt, 6);
	strncat(bin, fi.rt, (strlen(bin) + strlen(fi.rt) + 1));
	strncpy(fi.immd, immd, 17);
	strncat(bin, fi.immd, (strlen(bin) + strlen(fi.immd) + 1));
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
 * @return char*
 */
char* RegToBin(char* arg)
{
	int nReg = 0;
	char* bin = (char*)malloc(sizeof(char) * 6);
	char* tmp = (char*)malloc(sizeof(char) * 5);

	strcpy(tmp, arg);
	tmp = strtok(tmp, "$");
	tmp = strtok(tmp, ",");
	tmp = strtok(tmp, ")");
	nReg = atoi(tmp);
	for(int i = 4; i >= 0; i--)
	{
		bin[i] = (nReg & 1) + '0'; 
		nReg >>= 1;
	}
	bin[5] = '\0';
	return bin;
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
