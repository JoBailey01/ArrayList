#include "arrayList.h"
#include "listString.h"
#include "diagnostics.h"

int main(int argc, char** argv){

    lString* lstr = lstrNewString("Hamlet: To be, or not to be. That is the question. Whether 'tis nobler in the mind to suffer the slings and arrows...");

    lstrDiagnostics(lstr);

    lstrFreeString(lstr);

    arrayList list = alNewArrayList(2);

    char* dummyData = "ABCDEFGHIJKLMNOP";

    alInsertMany(list, dummyData, 9);

    alDiagnostics(list);

    alFreeArrayList(list);

    return 0;
}