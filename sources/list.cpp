#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "logger.h"
#include "list.h"

const size_t MAX_FILE_NAME = 256;
const int  IMG_WIDTH_IN_PERCENTS = 95;
const int IMG_HEIGTH_IN_PERCENTS = 40;

/// @brief prints one element of the list to stdout
static list_status_t printOneElem(list_t * list, list_el_id_t index);

/// @brief writes element as a string to str
static list_status_t elemToStr(list_t * list, list_el_id_t index, char * str);

/// @brief updates list.free, reallocates memory if needed
static list_status_t updateFree(list_t * list);

/// @brief reallocates list, new capacity = capacity * CAP_MULTIPLIER
static list_status_t listRealloc(list_t * list);

list_status_t listCtor(list_t * list, size_t elem_size, list_el_id_t capacity)
{
    assert(list);
    logPrint(LOG_DEBUG_PLUS, "constructing list (elem_size = %zu, cap = %zu)\n", elem_size, capacity);
    list->capacity = capacity;
    list->size = 0;
    list->elem_size = elem_size;
    list->data = calloc(capacity, elem_size);

    list->next = (list_el_id_t *)calloc(capacity + 1, sizeof(list_el_id_t));
    list->next[0] = 0;
    for (list_el_id_t index = 1; index < capacity; index++)
        list->next[index] = index + 1;
    list->next[list->capacity] = 0;

    list->prev = (list_el_id_t *)calloc(capacity + 1, sizeof(list_el_id_t));
    list->prev[0] = 0;
    for (list_el_id_t index = 1; index < capacity + 1; index++)
        list->prev[index] = -1;

    if (list->capacity == 0)
        list->free = 0;
    else
        list->free = 1;

    logPrint(LOG_DEBUG_PLUS, "successfully constructed list (free = %d)\n", list->free);
    return LIST_SUCCESS;
}

list_status_t listDtor(list_t * list)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    logPrint(LOG_DEBUG_PLUS, "destroying list...\n");
    if (list->data == NULL || list->prev == NULL || list->next == NULL)
        return LIST_DTOR_FREE_NULL;

    free(list->data);
    list->data = NULL;

    free(list->prev);
    list->prev = NULL;

    free(list->next);
    list->next = NULL;

    logPrint(LOG_DEBUG_PLUS, "list destroyed\n");
    return LIST_SUCCESS;
}

list_el_id_t listGetHeadIndex(list_t * list)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    return list->next[0];
}

list_el_id_t listGetTailIndex(list_t * list)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    return list->prev[0];
}

list_status_t listInsertAfter(list_t * list, list_el_id_t index, void * val)
{
    assert(list);
    assert(val);
    assert(listVerify(list) == LIST_SUCCESS);
    logPrint(LOG_DEBUG_PLUS, "entered listInsertAfter after %d element\n \tfree = %d, cap = %d\n", index, list->free, list->capacity);

    if (list->free == 0){
        logPrint(LOG_DEBUG_PLUS, "need reallocation\n");
        listRealloc(list);
    }

    list_el_id_t next_index = list->next[index];
    list_el_id_t  new_index = list->free;
    updateFree(list);

    list->next[index] = new_index;
    list->prev[new_index] = index;

    list->next[new_index] = next_index;
    list->prev[next_index] = new_index;

    void * new_elem_val = listGetElem(list, new_index);
    memcpy(new_elem_val, val, list->elem_size);

    list->size++;

    logPrint(LOG_DEBUG_PLUS, "exiting listInsertAfter\n");
    return LIST_SUCCESS;
}

list_status_t listInsertBefore(list_t * list, list_el_id_t index, void * val)
{
    assert(list);
    assert(val);
    assert(listVerify(list) == LIST_SUCCESS);
    logPrint(LOG_DEBUG_PLUS, "entered listInsertBefore before %d element\n \tfree = %d, cap = %d\n", index, list->free, list->capacity);

    if (list->free == 0){
        logPrint(LOG_DEBUG_PLUS, "need reallocation\n");
        listRealloc(list);
    }

    list_el_id_t prev_index = list->prev[index];
    list_el_id_t  new_index = list->free;
    updateFree(list);

    list->prev[index] = new_index;
    list->next[new_index] = index;

    list->prev[new_index] = prev_index;
    list->next[prev_index] = new_index;

    void * new_elem_val = listGetElem(list, new_index);
    memcpy(new_elem_val, val, list->elem_size);

    list->size++;

    logPrint(LOG_DEBUG_PLUS, "exiting listInsertBefore\n");
    return LIST_SUCCESS;
}

list_status_t listInsertFront(list_t * list, void * val)
{
    assert(list);
    assert(val);
    return listInsertAfter(list, 0, val);
}

list_status_t listInsertBack (list_t * list, void * val)
{
    assert(list);
    assert(val);
    return listInsertBefore(list, 0, val);
}

list_status_t listRemove(list_t * list, list_el_id_t index)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    logPrint(LOG_DEBUG_PLUS, "entering listRemove (removing %d element)\n\tcap = %d, size = %d\n", index, list->capacity, list->size);
    if (index == 0)
        return LIST_DELETE_ZERO_ERROR;

    list_el_id_t prev_index = list->prev[index];
    list_el_id_t next_index = list->next[index];

    list->next[prev_index] = next_index;
    list->prev[next_index] = prev_index;

    list->prev[index] = -1;

    list->size--;

    list->next[index] = list->free;
    list->free = index;

    logPrint(LOG_DEBUG_PLUS, "exiting listRemove (new size = %d)\n", list->size);
    return LIST_SUCCESS;
}

list_status_t listRemoveFirst(list_t * list)
{
    assert(list);
    return listRemove(list, list->next[0]);
}

list_status_t listRemoveLast (list_t * list)
{
    assert(list);
    return listRemove(list, list->prev[0]);
}

static list_status_t updateFree(list_t * list)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    logPrint(LOG_DEBUG_PLUS, "entering updateFree function\n");
    logPrint(LOG_DEBUG_PLUS, "\tfree = %d\n", list->free);
    list->free = list->next[list->free];
    logPrint(LOG_DEBUG_PLUS, "\t\new free = %d\n", list->free);
    logPrint(LOG_DEBUG_PLUS, "exiting updateFree\n");
    return LIST_SUCCESS;
}

static list_status_t listRealloc(list_t * list)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    logPrint(LOG_DEBUG_PLUS, "started reallocating...\n");
    list_el_id_t new_capacity = (list->capacity > 0) ? list->capacity * CAP_MULTIPLIER : MIN_CAPACITY;
    list->data = realloc(list->data, new_capacity * list->elem_size);
    list->next = (list_el_id_t *)realloc(list->next, (new_capacity + 1) * sizeof(list_el_id_t));
    list->prev = (list_el_id_t *)realloc(list->prev, (new_capacity + 1) * sizeof(list_el_id_t));

    if (list->data == NULL || list->next == NULL || list->prev == NULL)
        return LIST_REALLOC_ERROR;

    for (list_el_id_t index = list->capacity + 1; index < new_capacity; index++){
        list->next[index] = index + 1;
        list->prev[index] = -1;
    }
    list->next[new_capacity] = 0;
    list->prev[new_capacity] = -1;
    list->free = list->capacity + 1;
    list->capacity = new_capacity;
    logPrint(LOG_DEBUG_PLUS, "reallocated (new free = %d, new cap = %d)\n", list->free, list->capacity);
    return LIST_SUCCESS;
}

list_status_t listPrint(list_t * list)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    printf("\nstarted printing list\n");

    list_el_id_t index = list->next[0];
    while (index != 0){
        printf("elem #%d: ", index);
        printOneElem(list, index);
        putchar('\n');
        index = list->next[index];
    }
    printf("ended printing list\n");
    return LIST_SUCCESS;
}

static list_status_t printOneElem(list_t * list, list_el_id_t index)
{
    assert(list);
    void * list_elem = listGetElem(list, index);
    for (size_t byte_index = 0; byte_index < list->elem_size; byte_index++){
        printf("%02X ", *((unsigned char *)list_elem + byte_index));
    }
    return LIST_SUCCESS;
}

static list_status_t elemToStr(list_t * list, list_el_id_t index, char * str)
{
    assert(list);
    assert(str);
    void * list_elem = listGetElem(list, index);
    for (size_t byte_index = 0; byte_index < list->elem_size; byte_index++){
        sprintf(str + byte_index * 3, "%02X ", *((unsigned char *)list_elem + byte_index));
    }
    return LIST_SUCCESS;
}

void * listGetElem(list_t * list, list_el_id_t index)
{
    assert(list);
    assert(listVerify(list) == LIST_SUCCESS);
    void * list_elem_ptr = (char *)(list->data) + (index - 1) * list->elem_size;
    return list_elem_ptr;
}

list_status_t listVerify(list_t * list)
{
    assert(list);
    if (list->capacity < 0)
        return LIST_CAPACITY_OUT_ERROR;

    if (list->size > list->capacity)
        return LIST_OVERFLOW;

    if (list->size < 0)
        return LIST_SIZE_OUT_ERROR;

    if (list->free < 0 || list->free > list->capacity)
        return LIST_FREE_OUT_ERROR;

    if (list->elem_size == 0)
        return LIST_NO_ELEM_SIZE_ERROR;

    list_el_id_t last_index = -1;
    list_el_id_t index = list->next[0];
    while (last_index != 0){
        if (index < 0 || index > list->capacity)
            return LIST_PREV_NEXT_OUT_ERROR;
        if (index != list->prev[list->next[index]])
            return LIST_PREV_NEXT_ERROR;
        last_index = index;
        index = list->next[index];
    }
    return LIST_SUCCESS;
}

list_status_t listDump(list_t * list)
{
    assert(list);
    if (logGetLevel() < LOG_DEBUG)
        return LIST_SUCCESS;
    logPrint(LOG_DEBUG, "---------LIST_DUMP---------\n\n");

    logPrint(LOG_DEBUG, "capacity = %zu\n", list->capacity);
    logPrint(LOG_DEBUG, "size     = %zu\n", list->size);
    logPrint(LOG_DEBUG, "free     = %zu\n", list->free);

    logPrint(LOG_DEBUG, "index: ");
    for (list_el_id_t index = 0; index < list->capacity + 1; index++){
        logPrint(LOG_DEBUG, "%4zu ", index);
    }
    logPrint(LOG_DEBUG, "\nprevs: ");
    for (list_el_id_t index = 0; index < list->capacity + 1; index++){
        logPrint(LOG_DEBUG, "%4d ", list->prev[index]);
    }
    logPrint(LOG_DEBUG, "\nnexts: ");
    for (list_el_id_t index = 0; index < list->capacity + 1; index++){
        logPrint(LOG_DEBUG, "%4d ", list->next[index]);
    }
    logPrint(LOG_DEBUG, "\n");

    listDumpGraph(list);

    logPrint(LOG_DEBUG, "\n-------LIST_DUMP_END-------\n");
    return LIST_SUCCESS;
}

list_status_t listDumpGraph(list_t * list)
{
    assert(list);
    static size_t dump_count = 0;

    char dot_file_name[MAX_FILE_NAME] = "";
    char img_file_name[MAX_FILE_NAME] = "";

    system("mkdir -p logs/dots/");
    system("mkdir -p logs/imgs/");
    sprintf(dot_file_name, "logs/dots/graph_%zu.dot", dump_count);
    sprintf(img_file_name, "logs/imgs/graph_%zu.svg", dump_count);

    FILE * dot_file = fopen(dot_file_name, "w");
    listMakeDot(list, dot_file);
    fclose(dot_file);

    char sys_dot_cmd[MAX_FILE_NAME] = "";
    sprintf(sys_dot_cmd, "dot %s -Tsvg -o %s", dot_file_name, img_file_name);
    system(sys_dot_cmd);

    char img_file_name_log[MAX_FILE_NAME] = "";
    sprintf(img_file_name_log, "imgs/graph_%zu.svg", dump_count);
    logPrint(LOG_DEBUG, "<img src = %s height = \"%d%%\">",
                        img_file_name_log,
                        IMG_HEIGTH_IN_PERCENTS);
    logPrint(LOG_DEBUG, "<hr>");

    dump_count++;
    return LIST_SUCCESS;
}

list_status_t listMakeDot(list_t * list, FILE * dot_file)
{
    const char * bg_color = "#fbffc9";
    const char * list_bg_color = "#EEEEFF";
    const char * font_name = "Courier New";

    const char * free_elem_color_str     = "fillcolor = \"#AAAAAA\", fontcolor=\"#000000\"";
    const char * occupied_elem_color_str = "fillcolor = \"#CCFFCC\"";

    const char * main_arrows_color_str = "color=\"#00000000\"";
    const char * next_arrows_color_str = "color=\"#00FF00\"";
    const char * prev_arrows_color_str = "color=\"#FF0000\"";
    const char * free_arrows_color_str = "color=\"#888888\"";

    const char * free_label_color      = "fillcolor=\"#FFFFAA\"";
    const char * null_element_color    = "fillcolor=\"#FFA2E8\"";

    assert(list);
    assert(dot_file);

    char * elem_str = (char *)calloc(3 * list->elem_size + 1, sizeof(char));

    fprintf(dot_file, "digraph {\n");
    fprintf(dot_file, "rankdir = LR;\n");
    // fprintf(dot_file, "splines = ortho;\n");

    fprintf(dot_file, "fontname = \"%s\";\n", font_name);
    fprintf(dot_file, "node [fontname = \"%s\", style=filled, color=\"#000000\", fillcolor=\"#FFFFFF\"];\n", font_name);
    fprintf(dot_file, "bgcolor  = \"%s\";\n", bg_color);


    fprintf(dot_file, "node_0 [shape=Mrecord,label=\"element #0 | prev = %d | next = %d\",%s];\n",
            list->prev[0], list->next[0], null_element_color);

    fprintf(dot_file, "header_node [shape=Mrecord, label=\"HEADER | cap = %d | size = %d | free = %d | elem_size = %d\"];\n",
            list->capacity, list->size, list->free, list->elem_size);
    fprintf(dot_file, "header_node->node_0 [%s];\n", main_arrows_color_str);

    list_el_id_t index = 1;

    fprintf(dot_file, "subgraph cluster_main_list {\n");
    fprintf(dot_file, "style=filled; color = \"%s\";\n", list_bg_color);
    fprintf(dot_file, "label = \"main list\";\n", list_bg_color);
    fprintf(dot_file, "fontname = \"%s\";\n", font_name);
    fprintf(dot_file, "pencolor = \"#000000\";\n");
    while (index < list->capacity + 1){
        elemToStr(list, index, elem_str);
        const char * color_str = (list->prev[index] == -1) ? free_elem_color_str : occupied_elem_color_str;
        fprintf(dot_file, "node_%d [shape=Mrecord,label=\"element #%d | prev = %d | next = %d | val = 0x %s\", %s];\n",
                index, index, list->prev[index], list->next[index], elem_str, color_str);
        index++;
    }
    fprintf(dot_file, "}\n");

    index = 0;
    while (index < list->capacity){
        fprintf(dot_file, "node_%d->node_%d [%s];\n", index, index + 1, main_arrows_color_str);
        index++;
    }

    index = list->next[0];
    list_el_id_t last_index = -1;
    size_t rec_count = 0;
    while (last_index != 0){
        if (rec_count > (size_t)list->capacity + 1)
            break;
        rec_count++;

        fprintf(dot_file, "node_%d->node_%d [%s,constraint=false];\n",
                index, list->next[index], next_arrows_color_str);
        last_index = index;
        index = list->next[index];
    }
    index = list->prev[0];
    last_index = -1;
    rec_count = 0;
    while (last_index != 0){
        if (rec_count > (size_t)list->capacity + 1)
            break;
        rec_count++;

        fprintf(dot_file, "node_%d->node_%d [%s,constraint=false];\n",
                index, list->prev[index], prev_arrows_color_str);
        last_index = index;
        index = list->prev[index];
    }

    if (list->free == 0)
        fprintf(dot_file, "node_free [shape=Mrecord, label=\"free = 0 | no free elems\", %s];\n", free_label_color);
    else {
        fprintf(dot_file, "node_free [shape=Mrecord, label=\"free = %d | exists\", %s];\n", list->free, free_label_color);
        fprintf(dot_file, "node_free->node_%d [weight=0];\n", list->free);
    }
    index = list->free;
    while (index != 0){
        fprintf(dot_file, "node_%d->node_%d [%s,constraint=false];\n",
                index, list->next[index], free_arrows_color_str);
        index = list->next[index];
    }

    fprintf(dot_file, "}\n");

    free(elem_str);
    return LIST_SUCCESS;
}
