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

//Set all post-terminator (i.e., unused) characters in a lString to \0
void nullUnusedLString(lString* lstr){
    void_null_check(lstr);
    memset(lstr->head + lstr->length, '\0', lstr->allocatedLength - lstr->length);
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
        : DEFAULT_INITIAL_STRING_LENGTH;

    while(toAlloc < len && toAlloc < MAXIMUM_STRING_BYTES){
        toAlloc *= 2;
    }

    if(toAlloc < len && toAlloc < MAXIMUM_STRING_BYTES) toAlloc = MAXIMUM_STRING_BYTES;

    //Attempt to allocate a new string
    lString* lstr = newLenLString(toAlloc);

    if(lstr==NULL) return NULL;

    //Copy the input string into the new string
    strcpy(lstr->head, str);
    //memcpy(lstr->head, str, len);

    //Update string length
    lstr->length = len;

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
lstrLength expandLString(lString* lstr){
    null_check(lstr, 0);

    //Get the current allocated length of the string
    lstrLength curAlloc = lstr->allocatedLength;

    //Computer the new size as ceiling(double size, maximum size)
    lstrLength newAlloc = (curAlloc >= MAXIMUM_STRING_BYTES / 2)
        ? MAXIMUM_STRING_BYTES
        : curAlloc * 2;
    
    //If we are at maximum size, don't re-allocate anything
    if(newAlloc <= curAlloc) return curAlloc;

    //Allocate and zero out new memory
    char* newHead = (char*) malloc(newAlloc);
    memset(newHead, '\0', newAlloc);

    //Copy data from old to new memory
    memcpy(newHead, lstr->head, lstr->allocatedLength);

    //Free old memory
    free(lstr->head);

    //Update allocatedLength and head in lstr
    lstr->allocatedLength = newAlloc;
    lstr->head = newHead;

    //Return new allocated size
    return newAlloc;
}


//Insertion operations never overwrite existing string data

//Insert a character at an arbitrary point in the string. Returns a pointer to the element, or NULL for a failed operation (blank list, element out of bounds, string full, etc.)
char* lstrInsertChar(lString* lstr, lstrIndex index, char c){
    null_check(lstr, NULL);

    //Check index to ensure it falls within [0, lstr->length]
    if(index > lstr->length) return NULL;

    //If the insertion index falls just after the end of the list, append the char instead
    if(index == lstr->length) return lstrAppendChar(lstr, c);

    //Expand list if necessary
    if(lstr->length + 1 >= lstr->allocatedLength){
        lstrLength oldLen = lstr->allocatedLength;
        if(expandLString(lstr) <= oldLen) return NULL;
    }

    //Shift characters up (including the null terminator)
    memmove(lstr->head + index + 1, lstr->head + index, lstr->length + 1 - index);

    //Insert the new char
    *(lstr->head + index) = c;

    //Update string length
    lstr->length++;

    return lstr->head + index;
}

//Insert a copy of the input string at an arbitrary point in the string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation
//Note: This could be implemented by calling lstrInsertPartial, but doing so would require a second strlen() call. Since strlen() is an O(n) operation, I have chosen to copy code in the interest of speed at the cost of maintainability.
char* lstrInsertString(lString* lstr, lstrIndex index, char* str){
    null_check(lstr, NULL);

    //Check index to ensure it falls within [0, lstr->length]
    if(index > lstr->length) return NULL;

    //Get length of str. Do nothing if the string is empty.
    lstrLength len = strlen(str);
    if(len < 1) return NULL;

    //If the insertion index falls just after the end of the list, append the string instead
    if(index == lstr->length) return lstrAppendString(lstr, str);

    //Expand list if necessary
    while(lstr->allocatedLength - (lstr->length + 1) < len){
        lstrLength oldLen = lstr->allocatedLength;
        if(expandLString(lstr) <= oldLen) return NULL;
    }

    //Get the pointer to the insertion address in the string
    char* insertAddr = lstr->head + index;

    //Shift characters up (including the null terminator)
    memmove(insertAddr + len, insertAddr, lstr->length + 1 - index);

    //Copy the new string into position (ignoring the null terminator)
    memmove(insertAddr, str, len); //We use memmove here because the function may be used to duplicate the end of a lstring

    //Update string length
    lstr->length += len;

    return insertAddr;
}

//Insert a <len>-length fragment of the input string at an arbitrary point in the string. If the specified fragment length exceeds the total length of the input string, the operation reduces the fragment length to the length of the input string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation.
char* lstrInsertPartial(lString* lstr, lstrIndex index, char* str, unsigned long len){
    null_check(lstr, NULL);

    //Check index to ensure it falls within [0, lstr->length]
    if(index > lstr->length) return NULL;

    //Get real length of str. Do nothing if the input fragment is empty.
    lstrLength realLen = strlen(str);
    if(realLen < len) len = realLen;
    if(len < 1) return NULL;

    //If the insertion index falls just after the end of the list, append the fragment instead
    if(index == lstr->length) return lstrAppendPartial(lstr, str, len);

    //Expand list if necessary
    while(lstr->allocatedLength - (lstr->length + 1) < len){
        lstrLength oldLen = lstr->allocatedLength;
        if(expandLString(lstr) <= oldLen) return NULL;
    }

    //Get the pointer to the insertion address in the string
    char* insertAddr = lstr->head + index;

    //Shift characters up (including the null terminator)
    memmove(insertAddr + len, insertAddr, lstr->length + 1 - index);

    //Copy the new string into position (ignoring the null terminator)
    memmove(insertAddr, str, len); //We use memmove here because the function may be used to duplicate the end of a lstring

    //Update string length
    lstr->length += len;

    return insertAddr;
}

//Insert a character at the end of the string. Returns a pointer to the element, or NULL for a failed operation
char* lstrAppendChar(lString* lstr, char c){
    null_check(lstr, NULL);

    //Expand list if necessary
    if(lstr->length + 1 >= lstr->allocatedLength){
        lstrLength oldLen = lstr->allocatedLength;
        if(expandLString(lstr) <= oldLen) return NULL;
    }

    //Get the insertion address
    char* insertAddr = lstr->head + lstr->length;

    //Insert the new char
    *insertAddr = c;

    //Manually null-terminating the string should be unnecessary because unused characters are always '\0'
    // *(insertAddr + 1) = '\0';

    //Update string length
    lstr->length++;

    return insertAddr;
}

//Insert a copy of the input string at the end of the string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation
char* lstrAppendString(lString* lstr, char* str){
    null_check(lstr, NULL);

    //Get real length of str. Do nothing if the string is empty.
    lstrLength len = strlen(str);
    if(len < 1) return NULL;

    //Expand list if necessary
    while(lstr->allocatedLength - (lstr->length + 1) < len){
        lstrLength oldLen = lstr->allocatedLength;
        if(expandLString(lstr) <= oldLen) return NULL;
    }

    //Get the insertion address
    char* insertAddr = lstr->head + lstr->length;

    //Copy the new string into place
    memcpy(insertAddr, str, len);

    //Manually null-terminating the string should be unnecessary because unused characters are always '\0'
    // *(insertAddr + len) = '\0';

    //Update string length
    lstr->length += len;

    return insertAddr;
}

//Insert a <len>-length fragment of the input string at the end of the string. If the specified fragment length exceeds the total length of the input string, the operation reduces the fragment length to the length of the input string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation.
char* lstrAppendPartial(lString* lstr, char* str, unsigned long len){
    null_check(lstr, NULL);

    //Get real length of str. Do nothing if the string is empty.
    lstrLength realLen = strlen(str);
    if(realLen < len) len = realLen;
    if(len < 1) return NULL;

    //Expand list if necessary
    while(lstr->allocatedLength - (lstr->length + 1) < len){
        lstrLength oldLen = lstr->allocatedLength;
        if(expandLString(lstr) <= oldLen) return NULL;
    }

    //Get the insertion address
    char* insertAddr = lstr->head + lstr->length;

    //Copy the new string into place
    memcpy(insertAddr, str, len);

    //Manually null-terminating the string should be unnecessary because unused characters are always '\0'
    // *(insertAddr + len) = '\0';

    //Update string length
    lstr->length += len;

    return insertAddr;
}

//Insert a character at the start of the string. Returns a pointer to the element, or NULL for a failed operation
char* lstrPrependChar(lString* lstr, char c){
    return lstrInsertChar(lstr, 0, c);
}

//Insert a copy of the input string at the start of the string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation
char* lstrPrependString(lString* lstr, char* str){
    return lstrInsertString(lstr, 0, str);
}

//Insert a <len>-length fragment of the input string at the start of the string. If the specified fragment length exceeds the total length of the input string, the operation reduces the fragment length to the length of the input string. Returns a pointer to the start of the copy of the input string, or NULL for a failed operation.
char* lstrPrependPartial(lString* lstr, char* str, unsigned long len){
    return lstrInsertPartial(lstr, 0, str, len);
}


//Removal operations never affect the null terminator

//Remove the character at the specified index. Returns 0 for success, or 1 if the operation fails (e.g., because the list is empty)
int lstrRemoveChar(lString* lstr, lstrIndex index){
    null_check(lstr, 1);

    //Do not remove out of bounds or from an empty list
    if(index >= lstr->length) return 1;

    //To remove the final element, simply call removeLastChar
    if(index == lstr->length - 1) return lstrRemoveLastChar(lstr);

    //Get the removal address
    char* removeAddr = lstr->head + index;

    //Shift characters down (including the null terminator)
    memmove(removeAddr, removeAddr + 1, lstr->length + 1 - index);

    //Update string length
    lstr->length--;

    return 0;
}

//Remove the last character. Returns 0 for success, or 1 if the operation fails
int lstrRemoveLastChar(lString* lstr){
    null_check(lstr, 1);

    //Do not remove from an empty string
    if(lstr->length < 1) return 1;

    //Get the last character's address
    char* removeAddr = lstr->head + lstr->length - 1;

    //Simply overwrite the last character with a null terminator
    *removeAddr = '\0';

    //Update string length
    lstr->length--;

    return 0;

}

//Remove the first character. Returns 0 for success, or 1 if the operation fails
int lstrRemoveFirstChar(lString* lstr){
    return lstrRemoveChar(lstr, 0);
}

//Remove <len> characters, starting at the specified index and going forward. Returns 0 for success, or 1 if the operation fails (e.g., because the list has too few elements to remove)
int lstrRemoveString(lString* lstr, lstrIndex index, lstrLength len){
    null_check(lstr, 1);

    //Make sure index and len are valid
    if(index + len > lstr->length || len < 1) return 1;

    //Call removeLastString if applicable
    if(index + len == lstr->length && index > 0) return lstrRemoveLastString(lstr, len);

    //Pointer to the beginning of the area to be removed
    char* removeAddr = lstr->head + index;

    //Shift characters down
    memmove(removeAddr, removeAddr + len, lstr->length + 1 - (index + len));

    //Overwrite now-unused characters with '\0'
    memset(lstr->head + lstr->length - len, '\0', len);
    
    //Update string length
    lstr->length -= len;

    return 0;
}

//Remove <len> characters from the end of the string. Returns 0 for success, or 1 if the operation fails
int lstrRemoveLastString(lString* lstr, lstrLength len){
    null_check(lstr, 1);

    //Ensure len is valid
    if(len > lstr->length || len < 1) return 1;

    //Overwrite the last <len> characters with '\0'
    memset(lstr->head + lstr->length - len, '\0', len);

    //Update string length
    lstr->length -= len;

    return 0;
}

//Remove <len> characters from the start of the string. Returns 0 for success, or 1 if the operation fails
int lstrRemoveFirstString(lString* lstr, lstrLength len){
    return lstrRemoveString(lstr, 0, len);
}


//Find the first instance of a given character in the lString. Returns the index of the character, or MAXIMUM_STRING_BYTES on a failed operation. Note that MAXIMUM_STRING_BYTES, as an index, will always either be unused or contain the null terminator, never an actual member of the string.
lstrIndex lstrFindChar(lString* lstr, char c){
    null_check(lstr, MAXIMUM_STRING_BYTES);

    //Iterate over the string
    for(lstrIndex i = 0;i < lstr->length;i++){
        if(lstr->head[i] == c) return i;
    }

    return MAXIMUM_STRING_BYTES;
}

//Find the first instance of a given string in the lString. Returns the index of the start of the matching string, or MAXIMUM_STRING_BYTES on a failed operation (including cases where the input string is empty).
lstrIndex lstrFindString(lString* lstr, char* str){
    null_check(lstr, MAXIMUM_STRING_BYTES);

    //Get length of search string
    lstrLength len = strlen(str);

    //Sanity check
    if(len > lstr->length || len < 1) return MAXIMUM_STRING_BYTES;

    //Search lstr for str
    for(lstrIndex i = 0;i <= lstr->length - len;i++){
        for(lstrIndex j = 0;j < len;j++){
            if(lstr->head[i + j] != str[j]) break;
            else if(j == len-1) return i;
        }
    }

    return MAXIMUM_STRING_BYTES;
}


//Replace the first instance of one character in the string with a new character. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_BYTES if the operation fails (but not if the operation simply makes no replacements).
lstrLength lstrReplaceChar(lString* lstr, char old, char new){
    null_check(lstr, MAXIMUM_STRING_BYTES);

    //Iterate over the string and replace the first instance of old with new
    for(lstrIndex i = 0;i < lstr->length;i++){
        if(lstr->head[i] == old){
            lstr->head[i] = new;
            return 1;
        }
    }

    return 0;
}

//Replace all instances of one character in the string with a new character. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_BYTES if the operation fails (but not if the operation simply makes no replacements).
lstrLength lstrReplaceAllChar(lString* lstr, char old, char new){
    null_check(lstr, MAXIMUM_STRING_BYTES);

    //Ignore redundant replacement
    if(old == new) return 0;

    //Iterate over the string and replace all instances of old with new
    lstrLength replaced = 0;
    for(lstrIndex i = 0;i < lstr->length;i++){
        if(lstr->head[i] == old){
            lstr->head[i] = new;
            replaced++;
        }
    }

    return replaced;
}

//Replace the first instance of one substring with a new substring. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_BYTES if the operation fails. If the replacements would cause the string to exceed the maximum length, the operation fails and the original string is not altered.
lstrLength lstrReplaceString(lString* lstr, char* old, char* new){
    null_check(lstr, MAXIMUM_STRING_BYTES);

    //Get string lengths
    lstrLength oldLen = strlen(old);
    lstrLength newLen = strlen(new);

    //Ignore empty old strings
    if(oldLen < 1) return 0;

    //Find the first instance of the old string, if applicable
    lstrIndex index = lstrFindString(lstr, old);

    //If the old string is even present, find it
    if(index != MAXIMUM_STRING_BYTES){
        //Compute useful values
        char* indexAddr = lstr->head + index;
        lstrLength laterBytes = lstr->length + 1 - (index + oldLen);

        //Replace strings, with possible string expansion
        if(newLen == 0){
            return lstrRemoveString(lstr, index, oldLen) == 0 ? 1 : MAXIMUM_STRING_BYTES;
        } else if(newLen > oldLen){
            //Allocate new space as needed
            while(lstr->allocatedLength - (lstr->length + 1) < newLen - oldLen){
                lstrLength oldLen = lstr->allocatedLength;
                if(expandLString(lstr) <= oldLen) return MAXIMUM_STRING_BYTES;
            }
            
            //Update string length
            lstr->length += (newLen - oldLen);

        } else {
            //Update string length
            lstr->length -= (oldLen - newLen);
        }

        //Move up all bytes that fall after the old string
        memmove(indexAddr + newLen, indexAddr + oldLen, laterBytes);

        //Copy the new string into position
        memmove(indexAddr, new, newLen);

        return 1;
    //If we make no replacement, return 0
    } else {
        return 0;
    }
}

//Replace all instances of one substring with a new substring. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_BYTES if the operation fails. If the replacements would cause the string to exceed the maximum length, the operation fails and the original string is not altered.
lstrLength lstrReplaceAllString(lString* lstr, char* old, char* new){
    null_check(lstr, MAXIMUM_STRING_BYTES);

    //Get string lengths
    lstrLength oldLen = strlen(old);
    lstrLength newLen = strlen(new);

    //Ignore empty old strings
    if(oldLen < 1) return 0;

    //Track replacements
    lstrLength replaced = 0;

    //Make a copy of the original lString. We will perform operations on this new string.
    lString* copy = newLString(lstr->head);

    //Find (as a pointer) the first instance of the old string, if applicable
    char* indexAddr = strstr(copy->head, old);

    //If the old string is even present, find it
    while(indexAddr != NULL){
        //Compute useful values
        lstrIndex index = (lstrIndex) (indexAddr - copy->head);
        lstrLength laterBytes = copy->length + 1 - (index + oldLen);

        //Replace strings, with possible string expansion
        if(newLen == 0){
            int rem = lstrRemoveString(copy, index, oldLen);

            //If removal fails, free everything and return an error
            if(rem == 1){
                freeLString(copy);
                return MAXIMUM_STRING_BYTES;
            }

            //Otherwise, skip extraneous remaining steps
            replaced++;
            
            //Search forward for the old string
            indexAddr = strstr(indexAddr, old);
            continue;

        } else if(newLen > oldLen){
            //Allocate new space as needed
            while(copy->allocatedLength - (copy->length + 1) < newLen - oldLen){
                lstrLength oldAllocLen = copy->allocatedLength;
                if(expandLString(copy) <= oldAllocLen){
                    //If we ran out of space, free everything and return the error code
                    freeLString(copy);
                    return MAXIMUM_STRING_BYTES;
                }
            }
            //Move indexAddr to the corresponding location in the new string
            indexAddr = copy->head + index;
            
            //Update string length
            copy->length += (newLen - oldLen);

        } else {
            //Update string length
            copy->length -= (oldLen - newLen);
        }

        //Move up/back all bytes that fall after the old string
        memmove(indexAddr + newLen, indexAddr + oldLen, laterBytes);

        //Copy the new string into position
        memmove(indexAddr, new, newLen);

        replaced++;

        //Search for the old string, starting from the end of the area we have already searched
        //This is necessary for overlapping new/old strings, such as replacing "." with ".."
        indexAddr = strstr(indexAddr + newLen, old);
    }

    //At this point, we have performed all the replacements, and the copy has all of the right data. Overwrite the original.
    lstrOverwrite(lstr, copy->head);

    //Delete the copy
    freeLString(copy);

    return replaced;
}


//For ASCII characters only, return a copy of the string where all alphabetical characters are UPPERCASE. Returns a pointer to the new string, or NULL if the operation fails. The returned string may be empty.
//This function dynamically allocates memory, and its return value must be freed.
char* lstrToUpper(lString* lstr){
    null_check(lstr, NULL);

    //Make a copy of the string
    char* copy = malloc(lstr->length + 1);
    strcpy(copy, lstr->head);

    //Do not perform unnecessary loop operations
    if(lstr->length < 1) return copy;

    //Iterate over the string and modify characters appropriately
    for(lstrLength i = 0;i < lstr->length;i++){
        if(copy[i] >= 'a' && copy[i] <= 'z'){
            copy[i] -= 32; //Those ASCII guys might've ignored non-English languages, but making 'a' - 'A' = 32 was ingenious.
        }
    }

    return copy;
}

//For ASCII characters only, return a copy of the string where all alphabetical characters are lowercase. Returns a pointer to the new string, or NULL if the operation fails. The returned string may be empty.
//This function dynamically allocates memory, and its return value must be freed.
char* lstrToLower(lString* lstr){
    null_check(lstr, NULL);

    //Make a copy of the string
    char* copy = malloc(lstr->length + 1);
    strcpy(copy, lstr->head);

    //Do not perform unnecessary loop operations
    if(lstr->length < 1) return copy;

    //Iterate over the string and modify characters appropriately
    for(lstrLength i = 0;i < lstr->length;i++){
        if(copy[i] >= 'A' && copy[i] <= 'Z'){
            copy[i] += 32;
        }
    }

    return copy;
}


//Overwrite the contents of the string with a new string. The new string may be empty. Returns 0 for success, or 1 if the operation fails
int lstrOverwrite(lString* lstr, char* str){
    null_check(lstr, 1);

    //Get the new string's length
    lstrLength len = strlen(str);

    //Expand the string, if necessary
    while(lstr->allocatedLength < len + 1){
        lstrLength oldLen = lstr->allocatedLength;
        if(expandLString(lstr) <= oldLen) return 1;
    }

    //If the new string is not empty, copy it into the lString
    if(len > 0){
        //Using memmove lets the user overwrite the whole original string with the end of the original string
        memmove(lstr->head, str, len);
    }

    //Set the rest of the string to null
    memset(lstr->head + len, '\0', lstr->allocatedLength - len);

    //Update string length
    lstr->length = len;

    return 0;
}

//Return a reversed copy of the string. Returns NULL if the operation fails (including cases where the original string is empty).
//This function dynamically allocates memory, and its return value must be freed.
char* lstrReverse(lString* lstr){
    null_check(lstr, NULL);

    if(lstr->length < 1) return NULL;

    //Allocate the new string
    char* output = (char*) malloc(lstr->length + 1);

    //Initialise the new string to '\0' (also solves null termination)
    memset(output, '\0', lstr->length + 1);

    //Copy the original string, character by character, into the new one
    char* i = lstr->head;
    char* j = output + lstr->length - 1;
    while(*i != '\0'){
        *j = *i;
        i++;
        j--;
    }

    return output;

}


//Destroy and de-allocate the lString
void freeLString(lString* lstr){
    void_null_check(lstr);
    free(lstr->head);
    free(lstr);
}
