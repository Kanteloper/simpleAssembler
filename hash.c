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

	// create linkedList
	hTable->list = (List**)malloc(sizeof(List*) * size);
	
	// init
	for( int i = 0; i < size; i++ )
	{
		hTable->list[i] = 	
	}
	
	return hTable;
}

/**
 * @brief insert data to hash Table
 * @param HashTable* $hTable 
 * @param int k key
 * @param char* v label
 */
void HashInsert(HashTable* ht, int k, char* v)
{
	int hv = ht
	HashTable* first = ht->table[]; 
}


