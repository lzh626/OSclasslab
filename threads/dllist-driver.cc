#include "dllist.h"
#include <cstdio>

void InsertItem(int which, DLList *dllist, int keyv) {
    fprintf(stdout, "Thread %d : Insert %d\n", which, keyv);
    dllist->SortedInsert(NULL, keyv);
}

void RemoveItem(int which, DLList *dllist, int keyv) {
    fprintf(stdout, "Thread %d : Remove %d\n", which, keyv);
    dllist->SortedRemove(keyv);
}

void PrintList(int which, DLList* dllist) {
    fprintf(stdout, "Thread %d print: ", which);
    dllist->ShowList(PRI);
}

