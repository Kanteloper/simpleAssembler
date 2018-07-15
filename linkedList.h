#ifndef __LIST_H
#define __LIST_H

#include "slot.h"

typedef Slot Data;

typedef struct _node
{
	Data data;
	struct _node* next;
} Node;

typedef int HashFunc(Key k);

typedef struct _linked_list
{
	Node* head;
	Node* tail;
} LinkedList;

typedef LinkedList List;

void creatList(List* plist);

#endif
