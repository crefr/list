#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "logger.h"
#include "list.h"

static list_status_t printOneElem(list_t * list, int index);

list_status_t listCtor(list_t * list, size_t elem_size, int size)
{
    assert(list);
    list->size = size;
    list->elem_size = elem_size;

    list->head = 0;

    list->data = calloc(size, elem_size);

    list->next = (int *)calloc(size, sizeof(int));
    list->next[0] = 0;
    for (int index = 1; index < size; index++)
        list->next[index] = -1;

    list->prev = (int *)calloc(size, sizeof(int));
    list->prev[0] = 0;
    for (int index = 1; index < size; index++)
        list->prev[index] = -1;

    return LIST_SUCCESS;
}

list_status_t listDtor(list_t * list)
{
    assert(list);

    if (list->data == NULL || list->prev == NULL || list->next == NULL)
        return LIST_DTOR_FREE_NULL;

    free(list->data);
    list->data = NULL;

    free(list->prev);
    list->prev = NULL;

    free(list->next);
    list->next = NULL;

    return LIST_SUCCESS;
}

list_status_t listPrint(list_t * list)
{
    assert(list);
    printf("\nstarted printing list\n");

    int index = list->head;
    while (index != 0){
        printf("elem #%d: ", index);
        printOneElem(list, index);
        putchar('\n');
        index = list->next[index];
    }
    printf("ended printing list\n");
    return LIST_SUCCESS;
}

static list_status_t printOneElem(list_t * list, int index)
{
    assert(list);
    void * list_elem = listGetElem(list, index);
    for (size_t byte_index = 0; byte_index < list->elem_size; byte_index++){
        printf("%02X ", *((char *)list_elem + byte_index));
    }
    return LIST_SUCCESS;
}

void * listGetElem(list_t * list, int index)
{
    assert(list);
    void * list_elem_ptr = (char *)(list->data) + index * list->elem_size;
    return list_elem_ptr;
}
