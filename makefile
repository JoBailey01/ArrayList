# Add -g flag to debug with gdb
# -m64 compiles the code for x86-64 architecture, with 32-bit integers and 64-bit pointers
# -std=c17 is the latest officially adopted C standard, as of September 2024
CCFlags=-Wall -Werror -std=c17 -m64 -g
CC=gcc

all: arrayList.o test.o listString.o
	$(CC) $(CCFlags) -o test $^

listString.o: listString.c listString.h
	$(CC) $(CCFlags) -c $^

arrayList.o: arrayList.c arrayList.h
	$(CC) $(CCFlags) -c $^

test.o: test.c
	$(CC) $(CCFlags) -c $^

clean:
	rm *.o
	rm *.gch