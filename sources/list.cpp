#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "logger.h"
#include "list.h"

const size_t MAX_FILE_NAME = 100;

static list_status_t printOneElem(list_t * list, int index);

static list_status_t elemToStr(list_t * list, int index, char * str);

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

    list->dump_count = 0;

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
    list->prev[list->free] = index;

    list->next[list->free] = next_index;
    list->prev[next_index] = list->free;

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
    if (index == list->head){
        listRemoveFirst(list);
        return LIST_SUCCESS;
    }
    int prev_index = list->prev[index];
    int next_index = list->next[index];

    list->next[prev_index] = next_index;
    list->prev[next_index] = prev_index;
    return LIST_SUCCESS;
}

list_status_t listRemoveFirst(list_t * list)
{
    assert(list);
    int index = list->head;
    int prev_index = list->prev[index];
    int next_index = list->next[index];

    list->prev[next_index] = prev_index;
    list->head = next_index;
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

static list_status_t elemToStr(list_t * list, int index, char * str)
{
    assert(list);
    void * list_elem = listGetElem(list, index);
    for (size_t byte_index = 0; byte_index < list->elem_size; byte_index++){
        sprintf(str + byte_index * 3, "%02X ", *((char *)list_elem + byte_index));
    }
    return LIST_SUCCESS;
}

void * listGetElem(list_t * list, int index)
{
    assert(list);
    void * list_elem_ptr = (char *)(list->data) + index * list->elem_size;
    return list_elem_ptr;
}

list_status_t listDumpDot(list_t * list, FILE * dot_file)
{
    assert(list);
    assert(dot_file);

    char * elem_str = (char *)calloc(3 * list->elem_size + 1, sizeof(char));

    fprintf(dot_file, "digraph {\n");
    fprintf(dot_file, "rankdir = LR;\n");

    fprintf(dot_file, "node_0 [shape=record,label=\"element #0 | prev = 0 | next = 0 \"];\n");

    int index = list->head;
    while (index != 0){
        elemToStr(list, index, elem_str);
        fprintf(dot_file, "node_%d [shape=record,label=\"element #%d | prev = %d | next = %d | val = 0x %s\"];\n",
                index, index, list->prev[index], list->next[index], elem_str);
        index = list->next[index];
    }

    index = list->head;
    while (index != 0){
        fprintf(dot_file, "node_%d->node_%d [color=\"#00FF00\"];\n", index, list->next[index]);
        index = list->next[index];
    }

    index = list->head;
    while (index != 0){
        fprintf(dot_file, "node_%d->node_%d [color=\"#FF0000\"];\n", list->prev[index], index);
        index = list->next[index];
    }
    fprintf(dot_file, "}\n");

    free(elem_str);
    return LIST_SUCCESS;
}

list_status_t listDump(list_t * list)
{
    logPrint(LOG_DEBUG, "---------LIST_DUMP---------\n\n");
    char dot_file_name[MAX_FILE_NAME] = "";
    char img_file_name[MAX_FILE_NAME] = "";

    sprintf(dot_file_name, "logs/dots/graph_%d.dot", list->dump_count);
    sprintf(img_file_name, "logs/imgs/graph_%d.png", list->dump_count);

    FILE * dot_file = fopen(dot_file_name, "w");
    listDumpDot(list, dot_file);
    fclose(dot_file);

    char sys_dot_cmd[MAX_FILE_NAME] = "";
    sprintf(sys_dot_cmd, "dot %s -Tpng -o %s", dot_file_name, img_file_name);
    system(sys_dot_cmd);

    char img_file_name_log[MAX_FILE_NAME] = "";
    sprintf(img_file_name_log, "imgs/graph_%d.png", list->dump_count);
    // logPrint(LOG_DEBUG, "<img src = %s>", img_file_name_log, 50);
    logPrint(LOG_DEBUG, "<img src = %s width = \"%d%%\">", img_file_name_log, 50);

    logPrint(LOG_DEBUG, "\n-------LIST_DUMP_END-------\n");
    logPrint(LOG_DEBUG, "<hr>");

    list->dump_count++;
    return LIST_SUCCESS;
}
