#include "arrayList.h"
#include "listString.h"

int main(int argc, char** argv){

    lString* lstr = lstrNewString("Hamlet: To be, or not to be. That is the question. Whether 'tis nobler in the mind to suffer the slings and arrows...");

    lstrDiagnostics(lstr);

    lstrFreeString(lstr);

    arrayList* list = alNewBlankArrayList(2);

    char* dummyData = "ABCDEFGHIJKLMNOP";

    alInsertMany(list, 0, dummyData, 8);

    alDiagnostics(list);

    alFreeArrayList(list);

    return 0;
}