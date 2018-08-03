/*
 * Date : 2018-07-08
 * Name : kanteloper
 * Detail : implement Chaining Hash Table
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "hash.h"
#include "linkedList.h"

/**
 * @brief initiate and create hash Table
 * @param int $size hash table size
 * @param HashFunc* $f hash function
 * @return HashTable* 
 */
HashTable* createTable(int size, HashFunc* f)
{
	// create hash table
	HashTable* hTable = (HashTable*)malloc(sizeof(HashTable));

	// init hash table
	hTable->list = (List**)malloc(sizeof(List*) * size);
	for( int i = 0; i < size; i++ )
		// create linked list
		hTable->list[i] = initList();

	// register hash function
	hTable->hf = f;
	return hTable;
}

/**
 * @brief insert data to hash Table
 * @param HashTable* $ht 
 * @param Key $k key
 * @param Value $v label
 */
void HashInsert(HashTable* ht, Key k, Value v)
{
	int hv = ht->hf(k); // get hash Value

	// init slot
	Data newData;
	newData.addr = k;
	newData.value = (char*)malloc(sizeof(char) * (strlen(v) + 1));
	strncpy(newData.value, v, strlen(v) + 1);
	
	if(isDuplicated(ht, k, v))  //if duplicated
	{
		fprintf(stderr, "Duplicated data = %s, %d\n", v, k);
		return;
	}
	else //if not 
	{
		puts("insert");
		insertList(ht->list[hv], newData); // insert data
	}

}

/**
 * @brief search data in hash Table
 * @param HashTable* $ht 
 * @param Key $k 
 * @param Value $v 
 * @return Value
 */
Value HashSearch(HashTable* ht, Key k, Value v)
{
	int hv = ht->hf(k);
	Value val;
	if((val = searchList((ht->list[hv]), v)) != NULL)
	{
		return val;
	}
	return NULL;
}

/**
 * @brief check duplicated data
 * @param HashTable* $ht 
 * @param Key $k
 * @param Value $v
 * return bool
 */
bool isDuplicated(HashTable* ht, Key k, Value v)
{
	int hv = ht->hf(k);
	Value val;
	if((val = searchList((ht->list[hv]), v)) != NULL)
	{
		return true;
	}
	return false;
}

/**
 * @brief get an address of Value
 * @param HashTable* $ht 
 * @param int $hk total_key 
 * @param char* $oprn operand
 * @return int
 */
int getHashAddr(HashTable* ht, int hk, char* oprn)
{
	int addr;
	if((addr = getAddr(ht->list[hk], oprn)) != -1)
	{
		return addr;
	}
	return -1;
}
