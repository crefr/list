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

    int dump_count;
} list_t;

typedef enum
{
    LIST_SUCCESS = 0,
    LIST_CTOR_CALLOC_ERROR,
    LIST_PRINT_EMPTY_LIST,
    LIST_DTOR_FREE_NULL,
    LIST_NO_FREE_SPACE,
    LIST_DELETE_ZERO_ERROR
} list_status_t;

list_status_t listCtor(list_t * list, size_t elem_size, int size);

list_status_t listDtor(list_t * list);

list_status_t listPrint(list_t * list);

void * listGetElem(list_t * list, int index);

list_status_t listInsertAfter(list_t * list, int index, void * val);

list_status_t listRemoveFirst(list_t * list);

list_status_t listRemove(list_t * list, int index);

list_status_t listDumpDot(list_t * list, FILE * dot_file);

list_status_t listDump(list_t * list);

#endif
