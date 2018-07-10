/*
 * Date : 2018-07-08
 * Name : kanteloper
 * Detail : implement Chaining Hash Table
 */

#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

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
	
	// init
	hTable->num = 0; 
	hTable->table = (HashNode**)malloc(sizeof(HashNode*) * size); 
	
	for(int i = 0; i < size; i++) // allocate memory for hash table
	{
		hTable->table[i] = (HashNode*)malloc(sizeof(HashNode));
		if(!hTable->table[i])
		{
			fprintf(stderr, "memory allocation fail : Hash node\n");
			exit(1);
		}
		hTable->table[i]->hf = f;  // register hash function
		hTable->table[i]->list = NULL;
	}
	return hTable;
}

/**
 * @brief insert data to hash Table
 * @param HashTable* $hTable 
 * @param char* lb label
 */
void HashInsert(HashTable* ht, char* lb)
{

}


