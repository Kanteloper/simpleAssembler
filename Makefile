Assembler: hash.o assem.o
	gcc -o simAssem hash.o assem.o

hash.o: slot.h hash.h hash.c
	gcc -o hash.o -c hash.c
	
assem.o: assem.c hash.h
	gcc -o assem.o -c assem.c

clean:
	rm *.o simAssem
