#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "logger.h"
#include "list.h"

const size_t MAX_FILE_NAME = 100;
const size_t IMG_SIZE_IN_PERCENTS = 85;

static list_status_t printOneElem(list_t * list, int index);

static list_status_t elemToStr(list_t * list, int index, char * str);

static list_status_t updateFree(list_t * list);

list_status_t listCtor(list_t * list, size_t elem_size, size_t capacity)
{
    assert(list);
    list->capacity = capacity;
    list->size = 0;
    list->elem_size = elem_size;
    list->free = 1;
    list->data = calloc(capacity, elem_size);

    list->next = (int *)calloc(capacity + 1, sizeof(int));
    list->next[0] = 0;
    for (int index = 1; index < capacity; index++)
        list->next[index] = index + 1;
    list->next[list->capacity] = 0;

    list->prev = (int *)calloc(capacity + 1, sizeof(int));
    list->prev[0] = 0;
    for (int index = 1; index < capacity + 1; index++)
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
    int new_index = list->free;
    updateFree(list);

    list->next[index] = new_index;
    list->prev[new_index] = index;

    list->next[new_index] = next_index;
    list->prev[next_index] = new_index;

    void * new_elem_val = listGetElem(list, new_index);
    memcpy(new_elem_val, val, list->elem_size);

    list->size++;

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

    list->prev[index] = -1;

    list->size--;

    list->next[index] = list->free;
    list->free = index;

    return LIST_SUCCESS;
}

list_status_t listRemoveFirst(list_t * list)
{
    assert(list);
    listRemove(list, list->next[0]);
    return LIST_SUCCESS;
}

static list_status_t updateFree(list_t * list)
{
    list->free = list->next[list->free];
    return LIST_SUCCESS;
}

list_status_t listPrint(list_t * list)
{
    assert(list);
    printf("\nstarted printing list\n");

    int index = list->next[0];
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
    void * list_elem_ptr = (char *)(list->data) + (index - 1) * list->elem_size;
    return list_elem_ptr;
}

list_status_t listVerify(list_t * list)
{
    assert(list);

    int last_index = -1;
    int index = list->next[0];
    while (last_index != 0){
        if (index != list->prev[list->next[index]])
            return LIST_PREV_NEXT_ERROR;
        last_index = index;
        index = list->next[index];
    }
    if (list->size > list->capacity)
        return LIST_OVERFLOW;
    return LIST_SUCCESS;
}

list_status_t listDump(list_t * list)
{
    if (logGetLevel() < LOG_DEBUG)
        return LIST_SUCCESS;
    logPrint(LOG_DEBUG, "---------LIST_DUMP---------\n\n");

    logPrint(LOG_DEBUG, "capacity = %zu\n", list->capacity);
    logPrint(LOG_DEBUG, "size     = %zu\n", list->size);
    logPrint(LOG_DEBUG, "free     = %zu\n", list->free);

    logPrint(LOG_DEBUG, "index: ");
    for (size_t index = 0; index < list->capacity + 1; index++){
        logPrint(LOG_DEBUG, "%4zu ", index);
    }
    logPrint(LOG_DEBUG, "\nprevs: ");
    for (size_t index = 0; index < list->capacity + 1; index++){
        logPrint(LOG_DEBUG, "%4d ", list->prev[index]);
    }
    logPrint(LOG_DEBUG, "\nnexts: ");
    for (size_t index = 0; index < list->capacity + 1; index++){
        logPrint(LOG_DEBUG, "%4d ", list->next[index]);
    }
    logPrint(LOG_DEBUG, "\n");

    listDumpGraph(list);

    logPrint(LOG_DEBUG, "\n-------LIST_DUMP_END-------\n");
    return LIST_SUCCESS;
}

list_status_t listDumpGraph(list_t * list)
{
    char dot_file_name[MAX_FILE_NAME] = "";
    char img_file_name[MAX_FILE_NAME] = "";

    sprintf(dot_file_name, "logs/dots/graph_%d.dot", list->dump_count);
    sprintf(img_file_name, "logs/imgs/graph_%d.png", list->dump_count);

    FILE * dot_file = fopen(dot_file_name, "w");
    listMakeDot(list, dot_file);
    fclose(dot_file);

    char sys_dot_cmd[MAX_FILE_NAME] = "";
    sprintf(sys_dot_cmd, "dot %s -Tpng -o %s", dot_file_name, img_file_name);
    system(sys_dot_cmd);

    char img_file_name_log[MAX_FILE_NAME] = "";
    sprintf(img_file_name_log, "imgs/graph_%d.png", list->dump_count);
    logPrint(LOG_DEBUG, "<img src = %s width = \"%d%%\">", img_file_name_log, IMG_SIZE_IN_PERCENTS);

    logPrint(LOG_DEBUG, "<hr>");

    list->dump_count++;
    return LIST_SUCCESS;
}

list_status_t listMakeDot(list_t * list, FILE * dot_file)
{
    assert(list);
    assert(dot_file);

    char * elem_str = (char *)calloc(3 * list->elem_size + 1, sizeof(char));

    fprintf(dot_file, "digraph {\n");
    fprintf(dot_file, "rankdir = LR;\n");

    fprintf(dot_file, "node_0 [shape=Mrecord,label=\"element #0 | prev = %d | next = %d\",color=\"#7229c4\"];\n",
            list->prev[0], list->next[0]);

    int index = 1;
    while (index < list->capacity + 1){
        elemToStr(list, index, elem_str);
        const char * color_str = (list->prev[index] == -1) ? ",color=\"#969696\",fontcolor=\"#969696\"" : "";
        fprintf(dot_file, "node_%d [shape=Mrecord,label=\"element #%d | prev = %d | next = %d | val = 0x %s\"%s];\n",
                index, index, list->prev[index], list->next[index], elem_str, color_str);
        fprintf(dot_file, "node_%d->node_%d [color=\"#00000000\"];\n", index-1, index);
        index++;
    }

    index = list->next[0];
    int last_index = -1;
    while (last_index != 0){
        fprintf(dot_file, "node_%d->node_%d [color=\"#00FF00\",constraint=false];\n",
                index, list->next[index]);
        last_index = index;
        index = list->next[index];
    }
    index = list->prev[0];
    last_index = -1;
    while (last_index != 0){
        fprintf(dot_file, "node_%d->node_%d [color=\"#FF0000\",constraint=false];\n",
                index, list->prev[index]);
        last_index = index;
        index = list->prev[index];
    }
    index = list->free;
    while (index != 0){
        fprintf(dot_file, "node_%d->node_%d [color=\"#888888\",constraint=false];\n",
                index, list->next[index]);
        index = list->next[index];
    }

    fprintf(dot_file, "}\n");

    free(elem_str);
    return LIST_SUCCESS;
}
