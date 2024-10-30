#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "list.h"

int main()
{
    list_t mylist = {};
    listCtor (&mylist, sizeof(int), 5);
    listPrint(&mylist);
    listDtor (&mylist);
    return 0;
}
