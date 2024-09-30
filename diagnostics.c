#include "arrayList.h"
#include "listString.h"
#include <stdio.h>
#include <stdlib.h>

void alDiagnostics(arrayList* list){
    printf("Length: %ld\nBytes Used: %ld\nBytes Allocated: %ld\nHead: %p\nFirst: %p\nLast: %p\n",
        alGetListLength(list), alGetListSize(list), alGetAllocatedListSize(list), alGetListHead(list), alGetFirst(list), alGetLast(list)
        );

    printf("Contents:\n");
    char* temp = (char*) alGetListHead(list);
    for(int i = 0;i < alGetAllocatedListSize(list);i++){
        if(i == list->length * list->size) printf("||");
        printf("%2x", (int) *temp);
        //printf("%c", (int) *temp);
        temp++;
    }
    printf("\n");
}

void lstrDiagnostics(lString* lstr){
    printf("Length: %ld\nAllocated: %ld\nHead: %p\nFirst: %p\nLast: %p\n",
        lstrGetLength(lstr), lstrGetAllocatedSize(lstr), lstrGetString(lstr), lstrGetFirst(lstr), lstrGetLast(lstr)
        );
    
    printf("Contents:\n");
    for(int i = 0;i < lstrGetAllocatedSize(lstr);i++){
        if(i == lstr->length) printf("||");
        printf("%c", lstr->head[i] == '\0' ? '~' : lstr->head[i]);
    }
    printf("\n");
}