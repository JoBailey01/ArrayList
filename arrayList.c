#include "arrayList.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

//True if the given size and allocatedLength would result in an unsafe list length (i.e., larger than MAXIMUM_LIST_BYTES)
#define unsafeLength(size, allocatedLength) (unsigned __int128) size * allocatedLength > MAXIMUM_LIST_BYTES

//The maximum number of elements that a list with a given element size can safely support (within the bounds of MAXIMUM_LIST_BYTES)
#define maxSafeLength(size) (listLength) MAXIMUM_LIST_BYTES/size


//Set all bytes in an ArrayList to a set constant (including unused bytes)
void setList(arrayList* list, int setConstant){
    memset(list->head, setConstant, (unsigned long) list->size * list->allocatedLength);
}

//Set all bytes in an ArrayList to 0 (including unused bytes)
void zeroList(arrayList* list){
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
arrayList* newLenBlankList(eSize size, listLength allocatedLength){
    arrayList* list = newLenArrayList(size, allocatedLength);
    if(list==NULL) return NULL;
    zeroList(list);
    return list;
}

//Create a new blank (zeroed out) ArrayList with the specified size and default initial length
arrayList* newBlankList(eSize size){
    return newLenBlankList(size, (listLength) DEFAULT_INITIAL_LENGTH);
}


//Get a pointer to the head of an arrayList dynamically. Users should never store the head pointer statically.
void* getListHead(arrayList* list){
    return list->head;
}

//Get the length of the list, in elements
listLength getListLength(arrayList* list){
    return list->length;
}

//Compute the actual size of the USED arrayList, in bytes. The list will always be smaller than MAXIMUM_LIST_BYTES.
unsigned long getListSize(arrayList* list){
    return list->size * list->length;
}

//Compute the actual size of the ALLOCATED arrayList, in bytes. The list will always be smaller than MAXIMUM_LIST_BYTES.
unsigned long getAllocatedListSize(arrayList* list){
    return list->size * list->allocatedLength;
}


//Get an element in the arrayList by index. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* getByIndex(arrayList* list, listIndex index){
    if(list == NULL || list->head == NULL || index >= list->length) return NULL;

    //Avoid unnecessary arithmetic
    if(index == 0) return list->head;

    return (void*) ((unsigned long) list->head + (unsigned long) (list->size * index));
}

//Get the last element in the arrayList. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* getLast(arrayList* list){
    if(list->length < 1) return NULL;
    return getByIndex(list, list->length-1);
}

//Get the first element in the arrayList. Returns a pointer to the element, or NULL for invalid inputs (blank list, element out of bounds, etc.).
void* getFirst(arrayList* list){
    return getByIndex(list, 0);
}


//Expand the arrayList's allocated length, if possible. Returns the new allocatedLength, which may not be any larger.
//If possible, the arrayList's size doubles. If that doubled size would exceed MAXIMUM_LIST_BYTES, the new size is locked at the maximum safe size.
listLength expandList(arrayList* list){
    //Check safety of list
    if(list == NULL || list->head == NULL) return 0;

    //First, get the current allocated length of the list
    listLength curAlloc = list->allocatedLength;

    //Now, compute the doubled allocated size
    listLength newAlloc = curAlloc*2;

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
    
    //Redirect the list's head pointer to the new allocated memory
    list->head = newHead;

    return list->allocatedLength;
}


//Add an element to an arbitrary location in an arrayList. Takes a pointer to the new element (which is copied into the list) and the index for that element. All later elements are shifted up.
//Returns a pointer to the element in the list, or NULL if the attempt failed (either because the list is too large or because the list is more than one element shorter than the specified insertion index)
void* addAtIndex(arrayList* list, void* element, listIndex index){
    //Check safety of list
    //if(list == NULL || list->head == NULL) return NULL;

    //Check index and list. Index must fall within [0, length of list].
    if(index < 0 || index > list->length) return NULL;

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
    //Check safety of list
    //if(list == NULL || list->head == NULL) return NULL;

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
    return addAtIndex(list, element, 0);
}


//Add operations for many ([count]) elements (works largely the same as single-add operations)
//Returns a pointer to the beginning of the new elements in the list

//Insert <count> elements at index <index> in the list, copying memory from <elements> to <elements + count - 1>
void* addManyAtIndex(arrayList* list, void* elements, listIndex index, listLength count){
    //Check index and list. Index must fall within [0, length of list].
    if(index < 0 || index > list->length) return NULL;

    //If the index is just past the end of the list, call addToEnd instead.
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

    //Get the number of bytes after pointPlusCount
    unsigned long laterBytes = (list->length - index) * list->size;

    //Move all of the later elements in the list up by [size * count] bytes
    memmove(pointPlusCount, pointInList, laterBytes);

    //Copy new elements into list
    memmove(pointInList, elements, count * list->size);

    //Update length
    list->length += count;

    return pointInList;
}
//void* addManyToEnd(arrayList*, void*, listLength);
//void* addManyToBeginning(arrayList*, void*, listLength);

//TODO: Macro to compile all list operations with null-checking
//TODO: Complete addMany and removeMany operations
//TODO: Testing
//TODO: listString implementation (using arrayList for strings)


//Remove an element from the arrayList by index. Does not return the element. Returns 0 for success, or 1 if the provided index is out of bounds or the list is bad.
int removeAtIndex(arrayList* list, listIndex index){
    //Check safety of list
    //if(list == NULL || list->head == NULL) return 1;

    //Do not remove from empty lists
    if(list->length < 1) return 1;

    //Ignore out-of-bounds indices
    if(index < 0 || index >= list->length) return 1;

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
    //Do not remove from empty lists
    if(list->length < 1) return 1;

    list->length--;

    return 0;
}

//Remove the first element in the arrayList. Does not return the element. Returns 0 for success, or 1 if the list has no elements or the list is bad.
int removeFirst(arrayList* list){
    return removeAtIndex(list, 0);
}


//Remove operations for many ([count]) elements (works largely the same as single-remove operations). Does not return the element. Removing n elements from index i removes elements i, i+1, i+2,..., i+n-1.
//Returns 0 for success, or 1 if the list has too few elements or the list is bad.
//int removeManyAtIndex(arrayList*, listIndex, listLength);
//int removeManyLast(arrayList*, listLength);
//int removeManyFirst(arrayList*, listLength);


//Destroy and de-allocate an arrayList
void freeArrayList(arrayList* list){
    //De-allocate list memory
    free(list->head);

    //De-allocate the list itself
    free(list);
}
