#ifndef LIST_INCLUDED
#define LIST_INCLUDED

typedef struct
{
    void * data;

    int * next;
    int * prev;

    size_t elem_size;

    int size;
    int free;

    int head;
} list_t;

typedef enum
{
    LIST_SUCCESS = 0,
    LIST_CTOR_CALLOC_ERROR,
    LIST_PRINT_EMPTY_LIST,
    LIST_DTOR_FREE_NULL
} list_status_t;

list_status_t listCtor(list_t * list, size_t elem_size, int size);

list_status_t listDtor(list_t * list);

list_status_t listPrint(list_t * list);

void * listGetElem(list_t * list, int index);

#endif
