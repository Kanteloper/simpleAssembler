#ifndef __SLOT_H
#define __SLOT_H

typedef int Key;

typedef struct _slot
{
	int addr; // location counter
	char* value;
} Slot;

#endif
