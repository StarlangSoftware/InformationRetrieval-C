//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#include <FileUtils.h>
#include <StringUtils.h>
#include <Memory/Memory.h>
#include "PositionalPosting.h"
#include "Posting.h"

/**
 * Constructor for the PositionalPosting class. Sets the document id and initializes the position array.
 * @param doc_id document id of the posting.
 */
Positional_posting_ptr create_positional_posting(int doc_id) {
    Positional_posting_ptr result = malloc_(sizeof(Positional_posting));
    result->positions = create_array_list();
    result->doc_id = doc_id;
    return result;
}

Positional_posting_ptr clone_positional_posting(Positional_posting_ptr positional_posting) {
    Positional_posting_ptr result = malloc_(sizeof(Positional_posting));
    result->positions = create_array_list();
    for (int i = 0; i < positional_posting->positions->size; i++){
        Posting_ptr posting = array_list_get(positional_posting->positions, i);
        add_position(result, posting->id);
    }
    result->doc_id = positional_posting->doc_id;
    return result;
}

void free_positional_posting(Positional_posting_ptr positional_posting) {
    free_array_list(positional_posting->positions, (void (*)(void *)) free_posting);
    free_(positional_posting);
}

/**
 * Adds a position to the position list.
 * @param positional_posting Positional posting object.
 * @param position Position added to the position list.
 */
void add_position(Positional_posting_ptr positional_posting, int position) {
    array_list_add(positional_posting->positions, create_posting(position));
}

/**
 * Returns size of the position list.
 * @param positional_posting Positional posting object.
 * @return Size of the position list.
 */
int size_of_positional_posting(const Positional_posting* positional_posting) {
    return positional_posting->positions->size;
}

/**
 * Converts the positional posting to a string. String is of the form, document id, number of positions, and all
 * positions separated via space.
 * @param positional_posting Positional posting object.
 * @return String form of the positional posting.
 */
char *positional_posting_to_string(const Positional_posting *positional_posting) {
    char tmp[MAX_LINE_LENGTH];
    sprintf(tmp, "%d %d", positional_posting->doc_id, size_of_positional_posting(positional_posting));
    for (int i = 0; i < positional_posting->positions->size; i++){
        Posting_ptr posting = array_list_get(positional_posting->positions, i);
        sprintf(tmp, "%s%d ", tmp, posting->id);
    }
    char *result = NULL;
    result = str_copy(result, tmp);
    return result;
}
