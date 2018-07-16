/*
 * Name : kanteloper
 * Data : 2018-07-11
 * Detail : implement linked list
 */

#include "linkedList.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief create linked list
 * @param List* plist
 */
List* initList()
{
	List* plist = (List*)malloc(sizeof(List));
	if(!plist)
	{
		fprintf(stderr, "memory allocation fail : linked list\n");
		exit(1);
	}
	plist->head = NULL;
	plist->tail = NULL;

	return plist;
}

/**
 * @brief search data with key in linked list
 * @param List* $pl
 * @param Key k
 * @return Value
 */
Value searchList(List* pl, Key k)
{
	Node* sp = pl->head;
	while(sp)
	{
		if(sp->data.addr == k) return sp->data.value; // if found
		sp = sp->next;
	}
	return NULL;
}

/**
 * @brief insert data in linked list
 * @param HashTable* $ht
 * @param Data $new
 */
void insertList(HashTable* ht, Data new)
{
	
}


