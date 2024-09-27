#include <limits.h>

#define DEFAULT_INITIAL_STRING_LENGTH 64
#define MAXIMUM_STRING_LENGTH ULONG_MAX

//TODO: Write arrayList functions but specific to the string (size 1) case
//TODO: Write string-specific functions that take strings as input
//TODO: Write output functions that copy regions of a string?

//A string character index (unsigned long because the string can contain up to 2^64 characters)
typedef unsigned long lstrIndex;

//A parameter referring to the length of a string (in characters)
typedef unsigned long lstrLength;


//Define lString as a struct with all the necessary fields
typedef struct listString {
    //Length of the string (unsigned long), excluding the null terminator
    lstrLength length;

    //Number of bytes allocated to the string (unsigned long), including the null terminator
    lstrLength allocatedLength;

    //Pointer to the head of the string
    //This pointer can be accessed like a normal string, since lStrings are null-terminated if accessed properly
    char* head;
} lString;


//Set every non-terminating character in the string (including unused ones) to a character constant
void setLString(lString*, char);

//Set all characters in a lString to \0 (including unused ones and the terminator)
void nullLString(lString*);

//Create a new lString with the specified initial allocated length (including null terminator). All characters are initialised to '\0'. The minimum allowable initial length is 1 to allow for the null terminator.
//Returns NULL if allocation failed or the specified initial length is too small.
lString* newLenLString(lstrLength);

//Create a new lstring that contains a copy of the input string. If the input string is blank, then so is the new string.
//Returns NULL if allocation failed.
lString* newLString(char*);

//Create a new lstring with the default initial allocated length (including null terminator). All characters are initialised to '\0'
//Returns NULL if allocation failed
lString* newBlankLString();


//Get a pointer to the standard C string (i.e., the head of the string), even if the string is empty
char* getLStringHead(lString*);

//Get the length of the string (excluding null terminator) in bytes
lstrLength getLStringLength(lString*);

//Get the actual allocated size of the string, including the null terminator, in bytes
lstrLength getAllocatedLStringSize(lString);


//Get a pointer to an arbitrary character in the string by index. Returns NULL for an invalid string, an empty string, or an invalid index
char* lstrGetChar(lString*, lstrIndex);

//Get a pointer to the last character (before the null terminator) in the string. Returns NULL for an invalid or empty string.
char* lstrGetLast(lString*);

//Get a pointer to the first character in the string. Returns NULL for an invalid or empty string.
char* lstrGetFirst(lString*);

//Get a substring by index and length. If the specified substring length is too long, then the returned substring will contain as many characters as possible before it reaches the end of the original string.
//This function dynamically allocates memory, and its return value must be freed.
char* lstrGetSubstr(lString*, lstrIndex, lstrLength);


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


//Find the first instance of a given character in the lString. Returns the index of the character, or MAXIMUM_STRING_LENGTH on a failed operation. Note that MAXIMUM_STRING_LENGTH, as an index, will always either be unused or contain the null terminator, never an actual member of the string.
lstrIndex lstrFindChar(lString*, char);

//Find the first instance of a given string in the lString. Returns the index of the start of the matching string, or MAXIMUM_STRING_LENGTH on a failed operation (including cases where the input string is empty).
lstrIndex lstrFindString(lString*, char*);


//Replace all instances of one character in the string with a new character. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_LENGTH if the operation fails (but not if the operation simply makes no replacements).
lstrLength lstrReplaceChar(lString*, char, char);

//Replace all instances of one substring with a new substring. Returns the number of replacements, which may be 0. Returns MAXIMUM_STRING_LENGTH if the operation fails. If the replacements would cause the string to exceed the maximum length, the operation fails and the original string is not altered.
lstrLength lstrReplaceString(lString*, char*, char*);


//Overwrite the contents of the string with a new string. The new string may be empty. Returns 0 for success, or 1 if the operation fails
int lstrOverwrite(lString*, char*);

//Return a reversed copy of the string. Returns 0 for success, or 1 if the operation fails (including cases where the original string is empty)
//This function dynamically allocates memory, and its return value must be freed
char* lstrReverse(lString*);


//Destroy and de-allocate the lString
void freeLString(lString*);
