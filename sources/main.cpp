#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "list.h"

int main()
{
    system("mkdir -p \"logs\"");
    logStart("logs/log.html", LOG_DEBUG_PLUS, LOG_HTML);

    system("mkdir -p \"logs/dots\"");
    FILE * dot_file = fopen("logs/dots/graph.dot", "w");

    list_t mylist = {};
    listCtor (&mylist, sizeof(int), 8);
    listPrint(&mylist);

    int a = 52;
    int b = 48;
    int c = 36;
    listInsertAfter(&mylist, 0, &a);
    listDump(&mylist);

    listInsertAfter(&mylist, 1, &b);
    listDump(&mylist);

    listInsertAfter(&mylist, 1, &c);
    listDump(&mylist);

    listInsertAfter(&mylist, 1, &c);
    listDump(&mylist);

    listInsertAfter(&mylist, 1, &c);
    listDump(&mylist);

    listRemove(&mylist, 2);
    listPrint(&mylist);

    listDump(&mylist);
    printf("list verify: %d\n", listVerify(&mylist));
    listDtor (&mylist);
    fclose(dot_file);
    logExit();
    return 0;
}
