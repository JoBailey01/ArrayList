#include <stdlib.h>

//Define the arrayList type as a struct with all of the necessary fields
typedef struct arrList {
    //16-bit unsigned integer that defines the size, in bytes, of each element in the list (e.g., 1 byte for a char)
    //Maximum size: 65 535
    unsigned short size;

    //64-bit unsigned integer that defines the number of elements in the list. List operations must update this number manually.
    //Maximum length: 18 446 744 073 709 551 615
    unsigned long length;

    //Pointer to the current head of the list. This pointer is subject to change as the list grows, so it should not be referenced statically.
    //This pointer will point to an address allocated by malloc
    void* head;
} arrayList;

//Create a new ArrayList. Do not zero out contents.
//Caller specifies the size of each element and the initial length of the array
arrayList* newArrayList(unsigned short, unsigned long);

//Zero out the contents of a new ArrayList
void clearList(arrayList*);
