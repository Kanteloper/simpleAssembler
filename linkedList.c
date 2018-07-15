/*
 * Name : kanteloper
 * Data : 2018-07-11
 * Detail : implement linked list
 */

#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief create linked list
 * @param List* plist
 */
void initList(List* plist)
{
	plist = (List*)malloc(sizeof(List));
	if(!plist)
	{
		fprintf(stderr, "memory allocation fail : linked list\n");
		exit(1);
	}
	plist->head = NULL;
	plist->tail = NULL;
}

