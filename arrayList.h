//#include <stdlib.h>
#include <limits.h>

#define DEFAULT_INITIAL_LENGTH 32 //The default initial length of an ArrayList
#define MAXIMUM_LIST_BYTES ULONG_MAX

//An arrayList element index (unsigned long because the array can, if element size is 1, contain up to 2^64 elements)
typedef unsigned long alIndex;

//A parameter referring to the length of an arrayList (in elements, not bytes)
typedef unsigned long alLength;

//An arrayList element size
typedef unsigned short alESize;


//Define the arrayList type as a struct with all of the necessary fields
typedef struct arrList {
    //16-bit unsigned integer that defines the size, in bytes, of each element in the list (e.g., 1 byte for a char, 4 bytes for an int)
    //unsigned short size;
    alESize size;

    //64-bit unsigned integer that defines the number of elements in the list. List operations must update this number manually.
    //unsigned long length;
    alLength length;

    //64-bit unsigned integer that defines the current allocated size of the arrayList, in units of element size. This number must be greater than or equal to the array length.
    //unsigned long allocatedLength;
    alLength allocatedLength;

    //Note: The maximum possible size, in bytes, of the arrayList must not exceed 2^64 (ULONG_MAX). Beyond that point, we cannot malloc sufficient memory to hold the array.
    //This limit is enforced dynamically, based on the element size parameter, by bespoke arrayList functions.
    //It may be possible to resolve this issue by using a linked list of maximum-size blocks of memory.

    //Pointer to the current head of the list. This pointer is subject to change as the list grows, so it should not be referenced statically.
    //This pointer will point to an address allocated by malloc
    void* head;
} arrayList;


//Set all bytes in an ArrayList to a set constant (including unused bytes)
void alSetList(arrayList*, int);

//Set all bytes in an ArrayList to 0 (including unused bytes)
void alSetListNull(arrayList*);


//Create a new ArrayList with the specified element size AND specified initial length. Returns NULL if the specified size * specified length exceeds MAXIMUM_LIST_BYTES or if allocation failed. Using this function directly will cause valgrind errors. To avoid them, use alNewLenBlankArrayList instead.
arrayList* alNewLenArrayList(alESize, alLength);

//Create a new ArrayList with the specified element size and default initial length. Using this function directly will cause valgrind errors. To avoid them, use alNewBlankArrayList instead.
arrayList* alNewArrayList(alESize);

//Create a new blank (zeroed out) ArrayList with the specified size and specified initial length. Returns NULL if the specified size * specified length exceeds MAXIMUM_LIST_BYTES or if allocation failed.
arrayList* alNewLenBlankArrayList(alESize, alLength);

//Create a new blank (zeroed out) ArrayList with the specified size and default initial length
arrayList* alNewBlankArrayList(alESize);


//Get a pointer to the head of an arrayList dynamically. Users should never store the head pointer statically.
void* alGetListHead(arrayList*);

//Get the length of the list, in elements
alLength alGetListLength(arrayList*);

//Compute the actual size of the USED arrayList, in bytes. The list will always be smaller than MAXIMUM_LIST_BYTES.
unsigned long alGetListSize(arrayList*);

//Compute the actual size of the ALLOCATED arrayList, in bytes. The list will always be smaller than MAXIMUM_LIST_BYTES.
unsigned long alGetAllocatedListSize(arrayList*);


//Get an element in the arrayList by index. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* alGetElement(arrayList*, alIndex);

//Get the last element in the arrayList. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* alGetLast(arrayList*);

//Get the first element in the arrayList. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* alGetFirst(arrayList*);


//Add an element to an arbitrary location in an arrayList. Takes a pointer to the new element (which is copied into the list) and the index for that element. All later elements are shifted up.
//Returns a pointer to the element in the list, or NULL if the attempt failed (either because the list is too large or because the list is more than one element shorter than the specified insertion index)
void* alInsert(arrayList*, alIndex, void*);

//Add an element to the end of an arrayList. Takes a pointer to the new element, which is copied into the list.
//Returns a pointer to the element in the list, or NULL if the attempt failed (usually because the list is too large).
void* alAppend(arrayList*, void*);

//Add an element to the beginning of an arrayList. Takes a pointer to the new element, which is copied into the list.
//Returns a pointer to the element in the list, or NULL if the attempt failed (usually because the list is too large).
void* alPrepend(arrayList*, void*);


//Insert <count> elements at index <index> in the list, copying memory from <elements> to <elements + count - 1>. Returns a pointer to the beginning of the new elements in the list, or NULL if the operation failed (including cases where count < 1)
void* alInsertMany(arrayList*, alIndex, void*, alLength);

//Insert <count> elements at the end of the list, copying memory from <elements> to <elements + count - 1>. Returns a pointer to the beginning of the new elements in the list, or NULL if the operation failed (including cases where count < 1)
void* alAppendMany(arrayList*, void*, alLength);

//Insert <count> elements at the beginning of the list, copying memory from <elements> to <elements + count - 1>. Returns a pointer to the beginning of the new elements in the list, or NULL if the operation failed (including cases where count < 1)
void* alPrependMany(arrayList*, void*, alLength);


//Remove an element from the arrayList by index. Does not return the element. Returns 0 for success, or 1 if the provided index is out of bounds or the list is bad.
int alRemove(arrayList*, alIndex);

//Remove the last element in the arrayList. Does not return the element. Returns 0 for success, or 1 if the list has no elements or the list is bad.
int alRemoveLast(arrayList*);

//Remove the first element in the arrayList. Does not return the element. Returns 0 for success, or 1 if the list has no elements or the list is bad.
int alRemoveFirst(arrayList*);


//Remove <count> list elements from index <index> to <index + count - 1>. Returns 0 for success, or 1 if the list has too few elements or the list is bad.
int alRemoveMany(arrayList*, alIndex, alLength);

//Remove <count> list elements from the end of the list. Returns 0 for success, or 1 if the list has too few elements or the list is bad.
int alRemoveLastMany(arrayList*, alLength);

//Remove <count> list elements from the start of the list. Returns 0 for success, or 1 if the list has too few elements or the list is bad.
int alRemoveFirstMany(arrayList*, alLength);


//Destroy and de-allocate an arrayList
void alFreeArrayList(arrayList*);

//Print diagnostic information for debugging and development
void alDiagnostics(arrayList*);
