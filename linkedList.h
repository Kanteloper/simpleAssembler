#ifndef __LIST_H
#define __LIST_H

#include "slot.h"

typedef Slot Data;

typedef struct _node
{
	Data data;
	struct _node* next;
} Node;

typedef struct _linked_list
{
	Node* head;
	Node* tail;
} LinkedList;

typedef LinkedList List;

List* initList();
Value searchList(List* pl, Value v);
void insertList(List* pl, Data nd);
int getAddr(List* pl, char* oprn);
void freeLinkedList(List* pl);

#endif
