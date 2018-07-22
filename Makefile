Assembler: hash.o assem.o list.o
	gcc -O0 -g -o simAssem hash.o assem.o list.o

hash.o: slot.h hash.h hash.c  linkedList.h
	gcc -O0 -g -o hash.o -c hash.c
	
assem.o: assem.c hash.h linkedList.h
	gcc -O0 -g -o assem.o -c assem.c

list.o: slot.h linkedList.c linkedList.h
	gcc -O0 -g -o list.o -c linkedList.c

clean:
	rm *.o simAssem
