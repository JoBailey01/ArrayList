# Add -g flag to debug with gdb
# -m64 compiles the code for x86-64 architecture, with 32-bit integers and 64-bit pointers
CCFlags=-Wall -Werror -std=c17 -m64
CC=gcc

all: arrayList.o test.o
	$(CC) $(CCFlags) -o test $^

arrayList.o: arrayList.c arrayList.h
	$(CC) $(CCFlags) -c $^

test.o: test.c
	$(CC) $(CCFlags) -c $^

clean:
	rm *.o