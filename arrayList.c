#include "arrayList.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include <stdio.h>

//True if the given size and allocatedLength would result in an unsafe list length (i.e., larger than MAXIMUM_LIST_BYTES)
#define unsafeLength(size, allocatedLength) (unsigned __int128) size * allocatedLength > MAXIMUM_LIST_BYTES

//The maximum number of elements that a list with a given element size can safely support (within the bounds of MAXIMUM_LIST_BYTES)
#define maxSafeLength(size) (listLength) MAXIMUM_LIST_BYTES/size

//If the file is compiled with `-D CHECK_NULL`, all functions will check for null arrayList input and return an error code (i.e., retVal)
#ifdef CHECK_NULL
    #define null_check(list, retVal) if(list==NULL || list->head==NULL) return retVal;
    #define void_null_check(list) if(list==NULL || list->head==NULL) return;
#else
    #define null_check(list, retVal)
    #define void_null_check(list)
#endif


//Set all bytes in an ArrayList to a set constant (including unused bytes)
void setList(arrayList* list, int setConstant){
    void_null_check(list);
    memset(list->head, setConstant, (unsigned long) list->size * list->allocatedLength);
}

//Set all bytes in an ArrayList to 0 (including unused bytes)
void zeroList(arrayList* list){
    void_null_check(list);
    setList(list, 0);
}


//Create a new ArrayList with the specified element size AND specified initial allocated length. Returns NULL if the specified size * specified length exceeds MAXIMUM_LIST_BYTES or if allocation failed.
arrayList* newLenArrayList(eSize size, listLength allocatedLength){
    //Ensure the specified length is safe
    if(unsafeLength(size, allocatedLength)) return NULL;

    //Allocate list
    arrayList* list = (arrayList*) malloc(sizeof(arrayList));

    if(list==NULL) return NULL;

    //Set list element size, initial used length (0), and allocated length
    list->size = size;
    list->length = 0;
    list->allocatedLength = allocatedLength;

    //Allocate specified initial allocated length
    list->head = (void*) malloc(size * allocatedLength);

    if(list->head == NULL) return NULL;

    return list;
}

//Create a new ArrayList with the specified element size and default initial length
arrayList* newArrayList(eSize size){
    return newLenArrayList(size, (listLength) DEFAULT_INITIAL_LENGTH);
}

//Create a new blank (zeroed out) ArrayList with the specified size and specified initial length. Returns NULL if the specified size * specified length exceeds MAXIMUM_LIST_BYTES or if allocation failed.
arrayList* newLenBlankArrayList(eSize size, listLength allocatedLength){
    arrayList* list = newLenArrayList(size, allocatedLength);
    if(list==NULL) return NULL;
    zeroList(list);
    return list;
}

//Create a new blank (zeroed out) ArrayList with the specified size and default initial length
arrayList* newBlankArrayList(eSize size){
    return newLenBlankArrayList(size, (listLength) DEFAULT_INITIAL_LENGTH);
}


//Get a pointer to the head of an arrayList dynamically. Users should never store the head pointer statically.
void* getListHead(arrayList* list){
    null_check(list, NULL);
    return list->head;
}

//Get the length of the list, in elements
listLength getListLength(arrayList* list){
    null_check(list, 0);
    return list->length;
}

//Compute the actual size of the USED arrayList, in bytes. The list will always be smaller than MAXIMUM_LIST_BYTES.
unsigned long getListSize(arrayList* list){
    null_check(list, 0);
    return list->size * list->length;
}

//Compute the actual size of the ALLOCATED arrayList, in bytes. The list will always be smaller than MAXIMUM_LIST_BYTES.
unsigned long getAllocatedListSize(arrayList* list){
    null_check(list, 0);
    return list->size * list->allocatedLength;
}


//Get an element in the arrayList by index. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* getByIndex(arrayList* list, listIndex index){
    null_check(list, NULL);
    if(list == NULL || list->head == NULL || index >= list->length) return NULL;

    //Avoid unnecessary arithmetic
    if(index == 0) return list->head;

    return (void*) ((unsigned long) list->head + (unsigned long) (list->size * index));
}

//Get the last element in the arrayList. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* getLast(arrayList* list){
    null_check(list, NULL);
    if(list->length < 1) return NULL;
    return getByIndex(list, list->length - 1);
}

//Get the first element in the arrayList. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* getFirst(arrayList* list){
    null_check(list, NULL);
    return getByIndex(list, 0);
}


//Expand the arrayList's allocated length, if possible. Returns the new allocatedLength, which may not be any larger.
//If possible, the arrayList's size doubles. If that doubled size would exceed MAXIMUM_LIST_BYTES, the new size is locked at the maximum safe size.
listLength expandList(arrayList* list){
    null_check(list, 0);

    //Check safety of list
    if(list == NULL || list->head == NULL) return 0;

    //First, get the current allocated length of the list
    listLength curAlloc = list->allocatedLength;

    //Now, compute the doubled allocated size
    listLength newAlloc = curAlloc * 2;

    //If the new size is unsafe, then reduce the size to the maximum safe size for this list
    if(unsafeLength(list->size, newAlloc)) newAlloc = maxSafeLength(list->size);

    //If there is no need to re-allocate anything, then don't bother
    if(newAlloc <= curAlloc) return curAlloc;

    //Allocate enough memory for the new size
    void* newHead = (void*) malloc(list->size * newAlloc);

    //Copy all data from the old list memory to the new list memory
    //memmove(newHead, list->head, getAllocatedListSize(list)); //This is only necessary if the memory areas could overlap, which should be impossible in this case
    memcpy(newHead, list->head, getAllocatedListSize(list));

    //Free the old allocated memory
    free(list->head);

    //Update allocatedLength
    list->allocatedLength = newAlloc;
    
    //Redirect the list's head pointer to the new allocated memory
    list->head = newHead;

    return list->allocatedLength;
}


//Add an element to an arbitrary location in an arrayList. Takes a pointer to the new element (which is copied into the list) and the index for that element. All later elements are shifted up.
//Returns a pointer to the element in the list, or NULL if the attempt failed (either because the list is too large or because the list is more than one element shorter than the specified insertion index)
void* addAtIndex(arrayList* list, listIndex index, void* element){
    null_check(list, NULL);

    //Check index and list. Index must fall within [0, length of list].
    if(index > list->length) return NULL;

    //If the index is just past the end of the list, call addToEnd instead.
    if(index == list->length) return addToEnd(list, element);

    //Expand list if necessary
    if(list->length >= list->allocatedLength){
        listLength oldLen = list->allocatedLength;
        if(expandList(list) <= oldLen) return NULL;
    }

    //Get the pointer to the location in the list
    void* pointInList = (void*) ((unsigned long) list->head + (unsigned long) (list->size * index));

    //Get the pointer to one element past pointInList
    void* pointPlusOne = (void*) ((unsigned long) pointInList + (unsigned long) list->size);

    //Get the number of bytes after pointInList
    unsigned long laterBytes = (list->length - index) * list->size;

    //Move all of the later elements in the list up by [size] bytes
    if(laterBytes > 0) memmove(pointPlusOne, pointInList, laterBytes); //We use memmove here because the memory areas involved are almost always overlapping

    //Insert the new element, which may be from overlapping memory (e.g., copying element n to slot n to duplicate a list entry)
    memmove(pointInList, element, list->size);

    //Update list length
    list->length++;

    return pointInList;
}

//Add an element to the end of an arrayList. Takes a pointer to the new element, which is copied into the list.
//Returns a pointer to the element in the list, or NULL if the attempt failed (usually because the list is too large).
void* addToEnd(arrayList* list, void* element){
    null_check(list, NULL);

    //Expand list if necessary
    if(list->length >= list->allocatedLength){
        listLength oldLen = list->allocatedLength;
        if(expandList(list) <= oldLen) return NULL;
    }

    //Get the pointer to the end of the list
    void* endOfList = list->length > 0
        ? (void*) ((unsigned long) list->head + (unsigned long) list->size * list->length)
        : list->head;

    //Now, append an element to the end of the list
    //memmove(endOfList, element, list->size); //This is only necessary if the memory areas could overlap, which should be impossible in this case if the user is using the arrayList properly
    memcpy(endOfList, element, list->size);

    //Update list length
    list->length++;

    return endOfList;
}

//Add an element to the beginning of an arrayList. Takes a pointer to the new element, which is copied into the list.
//Returns a pointer to the element in the list, or NULL if the attempt failed (usually because the list is too large).
void* addToBeginning(arrayList* list, void* element){
    return addAtIndex(list, 0, element);
}


//Add operations for many ([count]) elements (works largely the same as single-add operations)
//Returns a pointer to the beginning of the new elements in the list, or NULL if the operation failed (including cases where count < 1)

//Insert <count> elements at index <index> in the list, copying memory from <elements> to <elements + count - 1>
void* addManyAtIndex(arrayList* list, listIndex index, void* elements, listLength count){
    null_check(list, NULL);

    //Check index and list. Index must fall within [0, length of list].
    if(index > list->length || count < 1) return NULL;

    //If the index is just past the end of the list, call addToManyEnd instead.
    if(index == list->length) return addManyToEnd(list, elements, count);

    //Expand list if necessary until the list is long enough or we run out of memory
    while(list->allocatedLength - list->length < count){
        listLength oldLen = list->allocatedLength;
        if(expandList(list) <= oldLen) return NULL;
    }

    //Get the pointer to the first location in the list
    void* pointInList = (void*) ((unsigned long) list->head + (unsigned long) (list->size * index));

    //Get the pointer to [count] elements past pointInList
    void* pointPlusCount = (void*) ((unsigned long) pointInList + (unsigned long) (list->size * count));

    //Get the number of bytes after pointInList
    unsigned long laterBytes = (list->length - index) * list->size;

    //Move all of the later elements in the list up by [size * count] bytes
    memmove(pointPlusCount, pointInList, laterBytes);

    //Copy new elements into list
    memmove(pointInList, elements, count * list->size);

    //Update length
    list->length += count;

    return pointInList;
}

//Insert <count> elements at the end of the list, copying memory from <elements> to <elements + count - 1>
void* addManyToEnd(arrayList* list, void* elements, listLength count){
    null_check(list, NULL);

    if(count < 1) return NULL;

    //Expand list if necessary until the list is long enough or we run out of memory
    while(list->allocatedLength - list->length < count){
        listLength oldLen = list->allocatedLength;
        if(expandList(list) <= oldLen) return NULL;
    }

    //Get the pointer to the end of the list
    void* endOfList = list->length > 0
        ? (void*) ((unsigned long) list->head + (unsigned long) list->size * list->length)
        : list->head;
    
    //Copy new elements into list
    memmove(endOfList, elements, count * list->size);

    //Update length
    list->length += count;

    return endOfList;
}

//Insert <count> elements at the beginning of the list, copying memory from <elements> to <elements + count - 1>
void* addManyToBeginning(arrayList* list, void* elements, listLength count){
    return addManyAtIndex(list, 0, elements, count);
}


//Remove an element from the arrayList by index. Does not return the element. Returns 0 for success, or 1 if the provided index is out of bounds or the list is bad.
int removeAtIndex(arrayList* list, listIndex index){
    null_check(list, 1);

    //Do not remove from empty lists and ignore out-of-bounds indices
    if(list->length < 1 || index >= list->length) return 1;

    //Handle removal of the final element in the list (do not overwrite element)
    if(index == list->length - 1){
        list->length--;
        return 0;
    }

    //Get the pointer to the location in the list
    void* pointInList = (void*) ((unsigned long) list->head + (unsigned long) (list->size * index));

    //Get the pointer to one element past pointInList
    void* pointPlusOne = (void*) ((unsigned long) pointInList + (unsigned long) list->size);

    //Get the number of bytes after pointPlusOne (at least one, due to earlier check)
    unsigned long laterBytes = (list->length - index - 1) * list->size;

    //Copy memory from index+1 onward into index
    memmove(pointInList, pointPlusOne, laterBytes);

    //Update list length
    list->length--;

    return 0;
}

//Remove the last element in the arrayList. Does not return the element. Returns 0 for success, or 1 if the list has no elements or the list is bad.
//The last element is not overwritten; the array is simply shortened.
int removeLast(arrayList* list){
    null_check(list, 1);

    //Do not remove from empty lists
    if(list->length < 1) return 1;

    list->length--;

    return 0;
}

//Remove the first element in the arrayList. Does not return the element. Returns 0 for success, or 1 if the list has no elements or the list is bad.
int removeFirst(arrayList* list){
    null_check(list, 1);

    return removeAtIndex(list, 0);
}


//Remove operations for many ([count]) elements (works largely the same as single-remove operations). Does not return the element. Removing n elements from index i removes elements i, i+1, i+2,..., i+n-1.
//Returns 0 for success, or 1 if the list has too few elements or the list is bad.

//Remove <count> list elements from index <index> to <index + count - 1>
int removeManyAtIndex(arrayList* list, listIndex index, listLength count){
    null_check(list, 1);

    //Do not over-remove from lists that are too small and ignore out-of-bounds indices
    if(list->length < count || count < 1 || index + count > list->length) return 1;

    //If the index and count would remove only elements at the end of the list (possibly including the entire list), simply reduce the list's length
    if(index + count == list->length){
        list->length -= count;
        return 0;
    }

    //Get the pointer to the location in the list
    void* pointInList = (void*) ((unsigned long) list->head + (unsigned long) (list->size * index));

    //Get the pointer to <count> elements past pointInList
    void* pointPlusCount = (void*) ((unsigned long) pointInList + (unsigned long) (list->size * count));

    //Number of bytes after pointPlusCount
    unsigned long laterBytes = (list->length - index - count) * list->size;

    //Move all elements after pointPlusCount back by <count> elements (i.e., to pointInList)
    memmove(pointInList, pointPlusCount, laterBytes);

    //Update length
    list->length -= count;

    return 0;

}

//Remove <count> list elements from the end of the list
int removeManyLast(arrayList* list, listLength count){
    null_check(list, 1);

    //Do not over-remove from lists that are too small and ignore out-of-bounds indices
    if(list->length < count || count < 1) return 1;

    //Simply reduce the length (do not overwrite elements)
    list->length -= count;
    return 0;
}

//Remove <count> list elements from the start of the list
int removeManyFirst(arrayList* list, listLength count){
    null_check(list, 1);

    return removeManyAtIndex(list, 0, count);
}


//Destroy and de-allocate an arrayList
void freeArrayList(arrayList* list){
    void_null_check(list);

    //De-allocate list memory
    free(list->head);

    //De-allocate the list itself
    free(list);
}
