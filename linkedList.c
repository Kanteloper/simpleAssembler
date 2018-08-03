/*
 * Name : kanteloper
 * Data : 2018-07-11
 * Detail : implement linked list
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

/**
 * @brief create linked list
 * @param List* plist
 */
List* initList()
{
	List* plist = (List*)malloc(sizeof(List));
	plist->head = NULL;
	plist->tail = NULL;
	return plist;
}

/**
 * @brief search data with key in linked list
 * @param List* $pl
 * @param Value $v
 * @return Data
 */
Value searchList(List* pl, Value v)
{
	Node* sp = pl->head;
	while(sp) 
	{
		if(strcmp(sp->data.value, v) == 0) return sp->data.value;
		sp = sp->next;
	}
	return NULL;
}

/**
 * @brief search operand address 
 * @param List* $pl
 * @param char* $oprn
 * @return int
 */
int getAddr(List* pl, char* oprn)
{
	Node* sp = pl->head;
	while(sp) 
	{
		if(strcmp(sp->data.value, oprn) == 0) return sp->data.addr;
		sp = sp->next;
	}
	return -1;
}

/**
 * @brief insert data in linked list
 * @param List* $pl
 * @param Data $nd
 */
void insertList(List* pl, Data nd)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = nd;
	newNode->next = NULL;

	if(pl->head == NULL) // if first node
	{
		puts("work head");
		pl->head = pl->tail = newNode;
	}
	else // if not
	{
		puts("work tail");
		pl->tail->next = newNode;
		pl->tail = newNode;
	}
}


