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

/**
 * Reads a positional posting list from a file. Reads N lines, where each line stores a positional posting. The
 * first item in the line shows document id. The second item in the line shows the number of positional postings.
 * Other items show the positional postings.
 * @param input_file Input stream to read from.
 * @param count Number of positional postings for this positional posting list.
 */
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

/**
 * Constructor of the PositionalPostingList class. Initializes the list.
 */
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

/**
 * Returns the number of positional postings in the posting list.
 * @return Number of positional postings in the posting list.
 */
int size_of_positional_posting_list(const Positional_posting_list* positional_posting_list) {
    return positional_posting_list->postings->size;
}

/**
 * Does a binary search on the positional postings list for a specific document id.
 * @param doc_id Document id to be searched.
 * @return The position of the document id in the positional posting list. If it does not exist, the method returns
 * -1.
 */
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

/**
 * Converts the positional postings list to a query result object. Simply adds all positional postings one by one
 * to the result.
 * @return QueryResult object containing the positional postings in this object.
 */
Query_result_ptr to_query_result(const Positional_posting_list *positional_posting_list) {
    Query_result_ptr result = create_query_result();
    for (int i = 0; i < size_of_positional_posting_list(positional_posting_list); i++){
        Positional_posting_ptr posting = array_list_get(positional_posting_list->postings, i);
        add_to_query_result2(result, posting->doc_id);
    }
    return result;
}

/**
 * Adds a new positional posting (document id and position) to the posting list.
 * @param doc_id New document id to be added to the positional posting list.
 * @param position New position to be added to the positional posting list.
 */
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

/**
 * Gets the positional posting at position index.
 * @param index Position of the positional posting.
 * @return The positional posting at position index.
 */
Positional_posting_ptr get_positional_posting(const Positional_posting_list *positional_posting_list, int index) {
    return array_list_get(positional_posting_list->postings, index);
}

/**
 * Returns simple union of two positional postings list p1 and p2. The algorithm assumes the intersection of two
 * positional postings list is empty, therefore the union is just concatenation of two positional postings lists.
 * @param second p2
 * @return Union of two positional postings lists.
 */
Positional_posting_list_ptr union_with_positional_posting_list(const Positional_posting_list *first, const Positional_posting_list *second) {
    Positional_posting_list_ptr result = create_positional_posting_list2();
    array_list_add_all(result->postings, first->postings);
    array_list_add_all(result->postings, second->postings);
    return result;
}

/**
 * Algorithm for the intersection of two positional postings lists p1 and p2. We maintain pointers into both lists
 * and walk through the two positional postings lists simultaneously, in time linear in the total number of postings
 * entries. At each step, we compare the docID pointed to by both pointers. If they are not the same, we advance the
 * pointer pointing to the smaller docID. Otherwise, we advance both pointers and do the same intersection search on
 * the positional lists of two documents. Similarly, we compare the positions pointed to by both position pointers.
 * If they are successive, we add the position to the result and advance both position pointers. Otherwise, we
 * advance the pointer pointing to the smaller position.
 * @param second p2, second posting list.
 * @return Intersection of two postings lists p1 and p2.
 */
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

/**
 * Converts the positional posting list to a string. String is of the form all postings separated via space.
 * @return String form of the positional posting list.
 */
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

/**
 * Prints this object into a file with the given index.
 * @param output_file Output stream to write the file.
 * @param index Position of this positional posting list in the inverted index.
 */
void positional_posting_list_write_to_file(const Positional_posting_list *positional_posting_list,
                                           FILE *output_file,
                                           int index) {
    if (positional_posting_list->postings->size > 0){
        fprintf(output_file, "%d %d\n%s", index, positional_posting_list->postings->size, positional_posting_list_to_string(positional_posting_list));
    }
}
