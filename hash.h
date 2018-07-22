#ifndef __HASH_H
#define __HASH_H

#include <stdbool.h>
#include "slot.h"
#include "linkedList.h"

#define TB_MAX 50

typedef int HashFunc(Key k);

typedef struct _hash_table
{
	HashFunc* hf;
	List** list;
} HashTable;

HashTable* createTable(int size, HashFunc* f);
void HashInsert(HashTable* ht, Key k, Value v);
Value HashSearch(HashTable* ht, Key k, Value v);
bool isDuplicated(HashTable* ht, Key k, Value v);

#endif
