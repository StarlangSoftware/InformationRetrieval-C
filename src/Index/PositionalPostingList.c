//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#include <stdlib.h>
#include <FileUtils.h>
#include <Dictionary/Word.h>
#include <string.h>
#include <Memory/Memory.h>
#include "PositionalPostingList.h"
#include "PositionalPosting.h"
#include "Posting.h"

Positional_posting_list_ptr create_positional_posting_list(FILE *input_file, int count) {
    char line[MAX_LINE_LENGTH];
    Positional_posting_list_ptr result = malloc_(sizeof(Positional_posting_list), "create_positional_posting_list");
    result->postings = create_array_list();
    for (int i = 0; i < count; i++){
        fgets(line, MAX_LINE_LENGTH, input_file);
        line[strcspn(line, "\n")] = 0;
        Array_list_ptr ids = split(line);
        int number_of_positional_postings = atoi(((String_ptr)array_list_get(ids, 1))->s);
        int doc_id = atoi(((String_ptr)array_list_get(ids, 0))->s);
        for (int j = 0; j < number_of_positional_postings; j++){
            int positionalPosting = atoi(((String_ptr)array_list_get(ids, j + 2))->s);
            add_to_positional_posting_list(result, doc_id, positionalPosting);
        }
        free_array_list(ids, (void (*)(void *)) free_string_ptr);
    }
    return result;
}

Positional_posting_list_ptr create_positional_posting_list2() {
    Positional_posting_list_ptr result = malloc_(sizeof(Positional_posting_list), "create_positional_posting_list2");
    result->postings = create_array_list();
    return result;
}

Positional_posting_list_ptr clone_positional_posting_list(Positional_posting_list_ptr positional_posting_list) {
    Positional_posting_list_ptr result = malloc_(sizeof(Positional_posting_list), "clone_positional_posting_list");
    result->postings = create_array_list();
    for (int i = 0; i < positional_posting_list->postings->size; i++){
        Positional_posting_ptr positional_posting = array_list_get(positional_posting_list->postings, i);
        array_list_add(result->postings, clone_positional_posting(positional_posting));
    }
    return result;
}

void free_positional_posting_list(Positional_posting_list_ptr positional_posting_list) {
    free_array_list(positional_posting_list->postings, (void (*)(void *)) free_positional_posting);
    free_(positional_posting_list);
}

int size_of_positional_posting_list(const Positional_posting_list* positional_posting_list) {
    return positional_posting_list->postings->size;
}

int get_index(const Positional_posting_list* positional_posting_list, int doc_id) {
    int begin = 0, end = size_of_positional_posting_list(positional_posting_list) - 1, middle;
    while (begin <= end){
        middle = (begin + end) / 2;
        Positional_posting_ptr posting = array_list_get(positional_posting_list->postings, middle);
        if (doc_id == posting->doc_id){
            return middle;
        } else {
            if (doc_id < posting->doc_id){
                end = middle - 1;
            } else {
                begin = middle + 1;
            }
        }
    }
    return -1;
}

Query_result_ptr to_query_result(const Positional_posting_list *positional_posting_list) {
    Query_result_ptr result = create_query_result();
    for (int i = 0; i < size_of_positional_posting_list(positional_posting_list); i++){
        Positional_posting_ptr posting = array_list_get(positional_posting_list->postings, i);
        add_to_query_result2(result, posting->doc_id);
    }
    return result;
}

void add_to_positional_posting_list(Positional_posting_list_ptr positional_posting_list, int doc_id, int position) {
    int index = get_index(positional_posting_list, doc_id);
    Positional_posting_ptr posting;
    if (index == -1){
        array_list_add(positional_posting_list->postings, create_positional_posting(doc_id));
        posting = array_list_get(positional_posting_list->postings, size_of_positional_posting_list(positional_posting_list) - 1);
    } else {
        posting = array_list_get(positional_posting_list->postings, index);
    }
    add_position(posting, position);
}

Positional_posting_ptr get_positional_posting(const Positional_posting_list *positional_posting_list, int index) {
    return array_list_get(positional_posting_list->postings, index);
}

Positional_posting_list_ptr union_with_positional_posting_list(const Positional_posting_list *first, const Positional_posting_list *second) {
    Positional_posting_list_ptr result = create_positional_posting_list2();
    array_list_add_all(result->postings, first->postings);
    array_list_add_all(result->postings, second->postings);
    return result;
}

Positional_posting_list_ptr intersection_with_positional_posting_list(const Positional_posting_list *first, const Positional_posting_list *second) {
    int i = 0, j = 0;
    Positional_posting_list_ptr result = create_positional_posting_list2();
    while (i < size_of_positional_posting_list(first) && j < size_of_positional_posting_list(second)){
        Positional_posting_ptr p1 = array_list_get(first->postings, i);
        Positional_posting_ptr p2 = array_list_get(second->postings, j);
        if (p1->doc_id == p2->doc_id){
            int position1 = 0;
            int position2 = 0;
            Array_list_ptr postings1 = p1->positions;
            Array_list_ptr postings2 = p2->positions;
            while (position1 < postings1->size && position2 < postings2->size){
                Posting_ptr posting1 = array_list_get(postings1, position1);
                Posting_ptr posting2 = array_list_get(postings2, position2);
                if (posting1->id + 1 == posting2->id){
                    add_to_positional_posting_list(result, p1->doc_id, posting2->id);
                    position1++;
                    position2++;
                } else {
                    if (posting1->id + 1 < posting2->id){
                        position1++;
                    } else {
                        position2++;
                    }
                }
            }
            i++;
            j++;
        } else {
            if (p1->doc_id < p2->doc_id){
                i++;
            } else {
                j++;
            }
        }
    }
    return result;
}

char *positional_posting_list_to_string(const Positional_posting_list *positional_posting_list) {
    char tmp[MAX_LINE_LENGTH];
    for (int i = 0; i < size_of_positional_posting_list(positional_posting_list); i++){
        Positional_posting_ptr positional_posting = get_positional_posting(positional_posting_list, i);
        sprintf(tmp, "%s\t%s\n", tmp, positional_posting_to_string(positional_posting));
    }
    char *result = NULL;
    result = str_copy(result, tmp);
    return result;
}

void positional_posting_list_write_to_file(const Positional_posting_list *positional_posting_list,
                                           FILE *output_file,
                                           int index) {
    if (positional_posting_list->postings->size > 0){
        fprintf(output_file, "%d %d\n%s", index, positional_posting_list->postings->size, positional_posting_list_to_string(positional_posting_list));
    }
}
