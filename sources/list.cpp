#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "logger.h"
#include "list.h"

static list_status_t printOneElem(list_t * list, int index);

static list_status_t updateFree(list_t * list);

list_status_t listCtor(list_t * list, size_t elem_size, int size)
{
    assert(list);
    list->size = size;
    list->elem_size = elem_size;

    list->head = 0;
    list->free = 1;

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

list_status_t listInsertAfter(list_t * list, int index, void * val)
{
    assert(list);
    assert(val);
    int next_index = list->next[index];

    list->next[index] = list->free;

    if (index == 0)
        list->head = list->free;
    else
        list->prev[list->free] = index;

    list->next[list->free] = next_index;

    void * free_elem_val = listGetElem(list, list->free);
    memcpy(free_elem_val, val, list->elem_size);
    updateFree(list);

    return LIST_SUCCESS;
}

list_status_t listRemove(list_t * list, int index)
{
    assert(list);
    if (index == 0)
        return LIST_DELETE_ZERO_ERROR;
    int prev_index = list->prev[index];
    int next_index = list->next[index];

    list->next[prev_index] = next_index;
    list->prev[next_index] = prev_index;
    return LIST_SUCCESS;
}

static list_status_t updateFree(list_t * list)
{
    int search_index = 1;
    while (list->next[search_index] != -1){
        search_index++;
        if (search_index > list->size)
            return LIST_NO_FREE_SPACE;
    }
    list->free = search_index;
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
