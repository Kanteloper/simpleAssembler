#ifndef __HASH_H
#define __HASH_H

#include "slot.h"
#include "linkedList.h"

#define TB_MAX 20

typedef int HashFunc(Key k);

typedef struct _hash_table
{
	HashFunc* hf;
	struct List** list;
} HashTable;

HashTable* createTable(int size, HashFunc* f);
void HashInsert(HashTable* ht, char* lb);

#endif
