Assembler: hash.o assem.o list.o
	gcc -o simAssem hash.o assem.o list.o

hash.o: slot.h hash.h hash.c  linkedList.h
	gcc -o hash.o -c hash.c
	
assem.o: assem.c hash.h linkedList.h
	gcc -o assem.o -c assem.c

list.o: linkedList.c linkedList.h
	gcc -o list.o -c linkedList.c

clean:
	rm *.o simAssem
