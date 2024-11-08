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

    list_el_id_t capacity;
    list_el_id_t size;
    list_el_id_t free;
} list_t;

/// @brief type for status of list in some situations
typedef enum
{
    LIST_SUCCESS = 0,
    LIST_CTOR_CALLOC_ERROR,
    LIST_REALLOC_ERROR,
    LIST_PRINT_EMPTY_LIST,
    LIST_DTOR_FREE_NULL,
    LIST_NO_FREE_SPACE,
    LIST_DELETE_ZERO_ERROR,
    LIST_PREV_NEXT_ERROR,
    LIST_OVERFLOW
} list_status_t;

/// @brief constructs list
list_status_t listCtor(list_t * list, size_t elem_size, list_el_id_t capacity);

/// @brief destructs list
list_status_t listDtor(list_t * list);

/// @brief prints list data to stdout
list_status_t listPrint(list_t * list);

/// @brief get element data pointer by its index
void * listGetElem(list_t * list, list_el_id_t index);

/*--------------------INDEXES--------------------*/
/// @brief returns index of head element of the list
list_el_id_t listGetHeadIndex(list_t * list);

/// @brief returns index of tail element of the list
list_el_id_t listGetTailIndex(list_t * list);
/*-----------------------------------------------*/

/*--------------------INSERTS--------------------*/
/// @brief inserts new list element after element with index
list_status_t listInsertAfter(list_t * list, list_el_id_t index, void * val);

/// @brief inserts new list element before element with index
list_status_t listInsertBefore(list_t * list, list_el_id_t index, void * val);

/// @brief inserts head element
list_status_t listInsertFront(list_t * list, void * val);

/// @brief inserts tail element
list_status_t listInsertBack (list_t * list, void * val);
/*-----------------------------------------------*/

/*--------------------REMOVES--------------------*/
/// @brief removes element from list by its index
list_status_t listRemove(list_t * list, list_el_id_t index);

/// @brief removes head element from the list
list_status_t listRemoveFirst(list_t * list);

/// @brief removes tail element from the list
list_status_t listRemoveLast (list_t * list);
/*-----------------------------------------------*/

/// @brief checks list for some errors
list_status_t listVerify(list_t * list);

/// @brief makes dot file for dump
list_status_t listMakeDot(list_t * list, FILE * dot_file);

/// @brief handles img dumps
list_status_t listDumpGraph(list_t * list);

/// @brief makes dump to log file
list_status_t listDump(list_t * list);

const size_t CAP_MULTIPLIER = 2;
const size_t MIN_CAPACITY = 4;

#endif
