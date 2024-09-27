#include "listString.h"
#include <stdlib.h>
#include <string.h>

//If the file is compiled with `-D CHECK_NULL`, all functions will check for null lstr input and return an error code (i.e., retVal)
#ifdef CHECK_NULL
    #define null_check(lstr, retVal) if(lstr==NULL || lstr->head==NULL) return retVal;
    #define void_null_check(lstr) if(lstr==NULL || lstr->head==NULL) return;
#else
    #define null_check(lstr, retVal)
    #define void_null_check(lstr)
#endif

//Set every non-terminating character in the string (including unused ones) to a character constant
void setLString(lString* lstr, char setConstant){
    void_null_check(lstr);
    memset(lstr->head, setConstant, lstr->allocatedLength - 1);
}

//Set all characters in a lString to \0 (including unused ones and the null terminator)
void nullLString(lString* lstr){
    void_null_check(lstr);
    memset(lstr->head, '\0', lstr->allocatedLength);
}

//Create a new lString with the specified initial allocated length (including null terminator). All characters are initialised to '\0'. The minimum allowable initial length is 1 to allow for the null terminator.
//Returns NULL if allocation failed or the specified initial length is too small.
lString* newLenLString(lstrLength allocatedLength){
    //Check input length
    if(allocatedLength < 1) return NULL;

    null_check(lstr, NULL);

    //The specified length cannot possibly exceed ULONG_MAX (i.e., MAXIMUM_STRING_BYTES), so we can safely use it

    //Allocate lString
    lString* lstr = (lString*) malloc(sizeof(lString));

    if(lstr==NULL) return NULL;

    //Set string length and allocated length
    lstr->length = 0;
    lstr->allocatedLength = allocatedLength;

    //Allocate specified initial allocated length
    lstr->head = (char*) malloc(allocatedLength);

    if(lstr->head == NULL){
        free(lstr);
        return NULL;
    }

    //Zero out the string. This also effectively null-terminates the string.
    nullLString(lstr);

    return lstr;
}

//Create a new lstring that contains a copy of the input string. If the input string is blank, then so is the new string.
//Returns NULL if allocation failed.
lString* newLString(char* str){
    if(str==NULL) return NULL;

    //Determine the amount of memory to allocate
    lstrLength len = strlen(str);
    lstrLength toAlloc = len > MAXIMUM_STRING_BYTES - DEFAULT_INITIAL_STRING_LENGTH
        ? MAXIMUM_STRING_BYTES
        : len + DEFAULT_INITIAL_STRING_LENGTH;

    //Attempt to allocate a new string
    lString* lstr = newLenLString(toAlloc);

    if(lstr==NULL) return NULL;

    //Copy the input string into the new string
    strcpy(lstr->head, str);

    return lstr;
}

//Create a new lstring with the default initial allocated length (including null terminator). All characters are initialised to '\0'
//Returns NULL if allocation failed
lString* newBlankLString(){
    return newLenLString(DEFAULT_INITIAL_STRING_LENGTH);
}


//Get a pointer to the standard C string (i.e., the head of the string), even if the string is empty. May return NULL if the specified lString is NULL and CHECK_NULL is enabled.
char* getLStringHead(lString* lstr){
    null_check(lstr, NULL);
    return lstr->head;
}

//Get the length of the string (excluding null terminator) in bytes. May return MAXIMUM_STRING_BYTES if the specified lString is NULL and CHECK_NULL is enabled.
lstrLength getLStringLength(lString* lstr){
    null_check(lstr, MAXIMUM_STRING_BYTES);
    return lstr->length;
}

//Get the actual allocated size of the string, including the null terminator, in bytes
lstrLength getAllocatedLStringSize(lString* lstr){
    null_check(lstr, 0);
    return lstr->allocatedLength;
}


//Get a pointer to an arbitrary character in the string by index. Returns NULL for an invalid string, an empty string, or an invalid index
char* lstrGetChar(lString* lstr, lstrIndex index){
    null_check(lstr, NULL);
    return lstr->head + index;
}

//Get a pointer to the last character (before the null terminator) in the string. Returns NULL for an invalid or empty string.
char* lstrGetLast(lString* lstr){
    null_check(lstr, NULL);
    if(lstr->length < 1) return NULL;
    return lstr->head + lstr->length - 1;
}

//Get a pointer to the first character in the string. Returns NULL for an invalid or empty string.
char* lstrGetFirst(lString* lstr){
    null_check(lstr, NULL);
    if(lstr->length < 1) return NULL;
    return lstr->head;
}

//Get a substring by index and length. If the specified substring length is too long, then the returned substring will contain as many characters as possible before it reaches the end of the original string (this could result in an empty string). Returns NULL on a failed or invalid operation, such as a specified 0-length substring or an out-of-bounds index.
//This function dynamically allocates memory, and its return value must be freed.
char* lstrGetSubstr(lString* lstr, lstrIndex index, lstrLength length){
    null_check(lstr, NULL);
    if(index >= lstr->length || length < 1) return NULL;

    //Determine the actual allowable length of the substring
    if(length > MAXIMUM_STRING_BYTES - 1 - index) length = MAXIMUM_STRING_BYTES - 1 - index;
    else if (index + length > lstr->length) length = lstr->length - index;

    //Allocate the new substring, leaving room for the null terminator
    char* substr = (char*) malloc(length + 1);

    //Set the substring to null
    memset(substr, '\0', length + 1);

    //Copy part of the string into the substring
    memcpy(substr, lstr->head + index, length);

    return substr;
}


//Expand the lString's allocated length, if possible. Returns the new allocatedLength, which may not be any larger if the operation failed.
//If possible, the string's allocated length doubles. If that doubled size would exceed MAXIMUM_STRING_BYTES, the new size is locked at the maximum safe size.
lstrLength expandLString(lString* lstr);


//Insertion operations never overwrite existing string data

//Insert a character at an arbitrary point in the string. Returns a pointer to the element, or NULL for a failed operation (blank list, element out of bounds, string full, etc.)
char* lstrInsertChar(lString*, lstrIndex, char);

//Insert a copy of the input string at an arbitrary point in the string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation
char* lstrInsertString(lString*, lstrIndex, char*);

//Insert a <count>-length fragment of the input string at an arbitrary point in the string. If the specified fragment length exceeds the total length of the input string, the operation reduces the fragment length to the length of the input string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation.
char* lstrInsertPartial(lString*, lstrIndex, char*, unsigned long);

//Insert a character at the end of the string. Returns a pointer to the element, or NULL for a failed operation
char* lstrAppendChar(lString*, char);

//Insert a copy of the input string at the end of the string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation
char* lstrAppendString(lString*, char*);

//Insert a <count>-length fragment of the input string at the end of the string. If the specified fragment length exceeds the total length of the input string, the operation reduces the fragment length to the length of the input string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation.
char* lstrAppendPartial(lString*, char*, unsigned long);

//Insert a character at the start of the string. Returns a pointer to the element, or NULL for a failed operation
char* lstrPrependChar(lString*, char);

//Insert a copy of the input string at the start of the string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation
char* lstrPrependString(lString*, char*);

//Insert a <count>-length fragment of the input string at the start of the string. If the specified fragment length exceeds the total length of the input string, the operation reduces the fragment length to the length of the input string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation.
char* lstrPrependPartial(lString*, char*, unsigned long);


//Removing functions never affect the null terminator

//Remove the character at the specified index. Returns 0 for success, or 1 if the operation fails (e.g., because the list is empty)
int lstrRemoveChar(lString*, lstrIndex);

//Remove the last character. Returns 0 for success, or 1 if the operation fails
int lstrRemoveLastChar(lString*);

//Remove the first character. Returns 0 for success, or 1 if the operation fails
int lstrRemoveFirstChar(lString*);

//Remove <count> characters, starting at the specified index and going forward. Returns 0 for success, or 1 if the operation fails (e.g., because the list has too few elements to remove)
int lstrRemoveString(lString*, lstrIndex, lstrLength);

//Remove <count> characters from the end of the string. Returns 0 for success, or 1 if the operation fails
int lstrRemoveLastString(lString*, lstrLength);

//Remove <count> characters from the start of the string. Returns 0 for success, or 1 if the operation fails
int lstrRemoveFirstString(lString*, lstrLength);


//Find the first instance of a given character in the lString. Returns the index of the character, or MAXIMUM_STRING_BYTES on a failed operation. Note that MAXIMUM_STRING_BYTES, as an index, will always either be unused or contain the null terminator, never an actual member of the string.
lstrIndex lstrFindChar(lString*, char);

//Find the first instance of a given string in the lString. Returns the index of the start of the matching string, or MAXIMUM_STRING_BYTES on a failed operation (including cases where the input string is empty).
lstrIndex lstrFindString(lString*, char*);


//Replace all instances of one character in the string with a new character. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_BYTES if the operation fails (but not if the operation simply makes no replacements).
lstrLength lstrReplaceChar(lString*, char, char);

//Replace all instances of one substring with a new substring. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_BYTES if the operation fails. If the replacements would cause the string to exceed the maximum length, the operation fails and the original string is not altered.
lstrLength lstrReplaceString(lString*, char*, char*);


//Overwrite the contents of the string with a new string. The new string may be empty. Returns 0 for success, or 1 if the operation fails
int lstrOverwrite(lString*, char*);

//Return a reversed copy of the string. Returns 0 for success, or 1 if the operation fails (including cases where the original string is empty)
//This function dynamically allocates memory, and its return value must be freed
char* lstrReverse(lString*);


//Destroy and de-allocate the lString
void freeLString(lString*);
