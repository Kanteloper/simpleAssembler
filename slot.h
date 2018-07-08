#ifndef __SLOT_H
#define __SLOT_H

typedef int Key;

typedef struct _slot
{
	char* label;
	int addr; // location counter
} Slot;

#endif
