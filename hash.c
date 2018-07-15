/*
 * Date : 2018-07-08
 * Name : kanteloper
 * Detail : implement Chaining Hash Table
 */

#include <stdlib.h>
#include <stdio.h>
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
	if(!hTable)
	{
		fprintf(stderr, "memory allocation fail : Hash table\n");
		exit(1);
	}

	// init hash table
	hTable->list = (List**)malloc(sizeof(List*) * size);
	for( int i = 0; i < size; i++ )
		// create linked list
		initList(&hTable->list[i]);

	// register hash function
	hTable->hf = f;
	return hTable;
}

/**
 * @brief insert data to hash Table
 * @param HashTable* $ht 
 * @param Key $k key
 * @param char* $v label
 */
void HashInsert(HashTable* ht, Key k, Value v)
{
	int hv = ht->hf(k); // get hash value
	Data newData = { k, v }; 

	// check dup
	if(HashSearch(ht, k) != NULL) // if duplicated
	{
		// display error
	}
	else // if not 
	{
		// insert data in list

	}
}

/**
 * @brief search data in hash Table
 * @param HashTable* $ht 
 * @param Key $k 
 * @param Value $v 
 * @return Value
 */
Value HashSearch(HashTable* ht, Key k)
{
	int hv = ht->hf(k);
	Value val;
	if((val = searchList(&(ht->list[hv]))) != NULL)
	{
		return val;
	}

	return NULL;
}
