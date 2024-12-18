#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "list.h"

int main()
{
    system("mkdir -p \"logs\"");
    logStart("logs/log.html", LOG_DEBUG_PLUS, LOG_HTML);
    logCancelBuffer();

    system("mkdir -p \"logs/dots\"");
    FILE * dot_file = fopen("logs/dots/graph.dot", "w");

    list_t mylist = {};
    listCtor(&mylist, sizeof(int), 0);
    listDump(&mylist);
    listPrint(&mylist);

    int a = 52;
    int b = 48;
    int c = 36;
    for (int i = 1; i < 5; i++){
        listInsertBack(&mylist, &i);
        listDump(&mylist);
    }
    listInsertFront(&mylist, &b);
    listDump(&mylist);

    listPrint(&mylist);

    printf("list verify: %d\n", listVerify(&mylist));
    listDtor (&mylist);
    fclose(dot_file);
    logExit();
    return 0;
}
