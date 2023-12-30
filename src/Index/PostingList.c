//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#include <stdlib.h>
#include <Dictionary/Word.h>
#include <FileUtils.h>
#include <string.h>
#include <Memory/Memory.h>
#include "PostingList.h"
#include "Posting.h"

Posting_list_ptr create_posting_list() {
    Posting_list_ptr result = malloc_(sizeof(Posting_list), "create_posting_list");
    result->postings = create_array_list();
    return result;
}

Posting_list_ptr create_posting_list2(char *line) {
    Posting_list_ptr result = malloc_(sizeof(Posting_list), "create_posting_list2");
    result->postings = create_array_list();
    Array_list_ptr ids = split(line);
    for (int i = 0; i < ids->size; i++){
        String_ptr id = array_list_get(ids, i);
        add_to_posting_list(result, atoi(id->s));
    }
    free_array_list(ids, (void (*)(void *)) free_string_ptr);
    return result;
}

Posting_list_ptr clone_posting_list(Posting_list_ptr posting_list) {
    Posting_list_ptr result = malloc_(sizeof(Posting_list), "clone_posting_list");
    result->postings = create_array_list();
    for (int i = 0; i < posting_list->postings->size; i++){
        Posting_ptr posting = array_list_get(posting_list->postings, i);
        add_to_posting_list(result, posting->id);
    }
    return result;
}

void add_to_posting_list(Posting_list_ptr posting_list, int doc_id) {
    array_list_add(posting_list->postings, create_posting(doc_id));
}

int size_of_posting_list(const Posting_list* posting_list) {
    return posting_list->postings->size;
}

Posting_list_ptr intersection_posting_list(const Posting_list *posting_list, const Posting_list *second_list) {
    int i = 0, j = 0;
    Posting_list_ptr result = create_posting_list();
    while (i < size_of_posting_list(posting_list) && j < size_of_posting_list(second_list)){
        Posting_ptr p1 = array_list_get(posting_list->postings, i);
        Posting_ptr p2 = array_list_get(second_list->postings, j);
        if (p1->id == p2->id){
            add_to_posting_list(result, p1->id);
            i++;
            j++;
        } else {
            if (p1->id < p2->id){
                i++;
            } else {
                j++;
            }
        }
    }
    return result;
}

Posting_list_ptr union_with_posting_list(const Posting_list *posting_list, const Posting_list *second_list) {
    Posting_list_ptr result = create_posting_list();
    array_list_add_all(result->postings, posting_list->postings);
    array_list_add_all(result->postings, second_list->postings);
    return result;
}

char *posting_list_to_string(const Posting_list *posting_list) {
    char tmp[MAX_LINE_LENGTH];
    for (int i = 0; i < posting_list->postings->size; i++){
        Posting_ptr posting = array_list_get(posting_list->postings, i);
        sprintf(tmp, "%s%d ", tmp, posting->id);
    }
    strcat(tmp, "\n");
    char *result = NULL;
    result = str_copy(result, tmp);
    return result;
}

void posting_list_write_to_file(const Posting_list *posting_list, FILE *output_file, int index) {
    if (posting_list->postings->size > 0){
        fprintf(output_file, "%d %d\n%s", index, posting_list->postings->size, posting_list_to_string(posting_list));
    }
}

void free_posting_list(Posting_list_ptr posting_list) {
    free_array_list(posting_list->postings, (void (*)(void *)) free_posting);
    free_(posting_list);
}

int compare_posting_list(const Posting_list *first, const Posting_list *second) {
    if (first->postings->size < second->postings->size){
        return -1;
    } else {
        if (first->postings->size > second->postings->size){
            return 1;
        } else {
            return 0;
        }
    }
}

Query_result_ptr posting_list_to_query_result(const Posting_list *posting_list) {
    Query_result_ptr result = create_query_result();
    for (int i = 0; i < size_of_posting_list(posting_list); i++){
        Posting_ptr posting = array_list_get(posting_list->postings, i);
        add_to_query_result2(result, posting->id);
    }
    return result;
}
