#ifndef __HASH_H
#define __HASH_H

#include "slot.h"

#define TB_MAX 20

typedef Slot Data;

typedef struct _node
{
	Data data;
	struct _node* next;
} Node;

typedef int HashFunc(Key k);

typedef struct _hash_node
{
	HashFunc* hf;
	struct _node* list;
} HashNode;

typedef struct _hash_table
{
	int num; // number of Data
	struct _hash_node** table;
} HashTable;

HashTable* createTable(int size, HashFunc* f);

#endif
