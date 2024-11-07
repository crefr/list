#ifndef LIST_INCLUDED
#define LIST_INCLUDED

/// @brief type for element index in list
typedef int list_el_id_t;

/// @brief type for list
typedef struct
{
    void * data;

    list_el_id_t * next;
    list_el_id_t * prev;

    size_t elem_size;

    size_t capacity;
    size_t size;
    list_el_id_t free;
} list_t;

/// @brief type for status of list in some situations
typedef enum
{
    LIST_SUCCESS = 0,
    LIST_CTOR_CALLOC_ERROR,
    LIST_PRINT_EMPTY_LIST,
    LIST_DTOR_FREE_NULL,
    LIST_NO_FREE_SPACE,
    LIST_DELETE_ZERO_ERROR,
    LIST_PREV_NEXT_ERROR,
    LIST_OVERFLOW
} list_status_t;

/// @brief constructs list
list_status_t listCtor(list_t * list, size_t elem_size, size_t capacity);

/// @brief destructs list
list_status_t listDtor(list_t * list);

/// @brief prints list data to stdout
list_status_t listPrint(list_t * list);

/// @brief get element data pointer by its index
void * listGetElem(list_t * list, list_el_id_t index);

/// @brief inserts new list element after element with index
list_status_t listInsertAfter(list_t * list, list_el_id_t index, void * val);

/// @brief removes element from list by its index
list_status_t listRemove(list_t * list, list_el_id_t index);

/// @brief removes head element from the list
list_status_t listRemoveFirst(list_t * list);

/// @brief checks list for some errors
list_status_t listVerify(list_t * list);

/// @brief makes dot file for dump
list_status_t listMakeDot(list_t * list, FILE * dot_file);

/// @brief handles img dumps
list_status_t listDumpGraph(list_t * list);

/// @brief makes dump to log file
list_status_t listDump(list_t * list);


#endif
