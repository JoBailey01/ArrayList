# ArrayList and ListString

These files are a personal project that I wrote to learn about the implementation of a common data structure that I had used many times before but wanted to understand on a deeper level: Array List. I first came to the idea of writing this programme entirely in C when I was fixing a strange bug in a C++ project of mine. Exploring that bug showed me that C++ implements its standard library string as an array list. I thought that sounded really neat, and I could potentially use a purely C array list in future projects, so I wrote this. Once I had a generic array list implemented, I went on to create code for the special case of strings consisting of one-byte characters: after all, C and C++ provide extensive sets of string-related functions for good reason!

The files arrayList.c and arrayList.h provide a type-generic implementation of the Array List data structure, capable of accepting elements of any size (in complete bytes) up to the 16-bit unsigned integer maximum. The arrayList can support up to 2^64 bytes, although relatively few applications require a list that large.

Users should not modify the information in the arrayList directly. The provided arrayList functions manage and track the length of the list. Any external modifications that affect the list's length will result in undefined behaviour.

The files listString.c and listString.h implement the Array List data structure for the special case where all elements are 1-byte characters. Strings in formats such as UTF-8 and UTF-16 could also be stored in a lString, but all lString functions would still treat the data as an array of single-byte characters. The various lString functions also provide string-specific functionality.

lString stores its information in a format compatible with standard C string functions, but users should not modify the information in the string directly. The provided lString functions carefully manage and track the length of the string. Any external modifications that affect the string's length will result in undefined behaviour.

The arrayList and lString functions make extensive use of custom data types: alIndex, alLength, alESize, lstrIndex, and lstrLength. These types are all defined in the arrayList.h and listString.h header files. All of these types are simply unsigned integers of various sizes. They exist to clarify the purpose of various function arguments and return values.

Further details on each function, for both arrayList and lString, can be found in the comments above each function in both the .h and .c files.

The makefile in the repository contains the flags used to compile and test all of the code in the repository. The test.c file is provided as a basic example of how to use the arrayList and listString functions. Its primary purpose is to ensure that the makefile has something to do.
