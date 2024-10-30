#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "list.h"

int main()
{
    list_t mylist = {};
    listCtor (&mylist, sizeof(int), 16);
    listPrint(&mylist);

    int a = 52;
    int b = 48;
    int c = 36;
    listInsertAfter(&mylist, 0, &a);
    listPrint(&mylist);

    listInsertAfter(&mylist, 1, &b);
    listPrint(&mylist);

    listInsertAfter(&mylist, 1, &c);
    listPrint(&mylist);

    listInsertAfter(&mylist, 1, &c);
    listPrint(&mylist);

    listInsertAfter(&mylist, 1, &c);
    listPrint(&mylist);

    listRemove(&mylist, 4);
    listPrint(&mylist);

    listDtor (&mylist);
    return 0;
}
