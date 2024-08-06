//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#include <Memory/Memory.h>
#include "PostingSkip.h"

/**
 * Constructor for the PostingSkip class. Sets the document id.
 * @param Id Document id.
 */
Posting_skip_ptr create_posting_skip(int id) {
    Posting_skip_ptr result = malloc_(sizeof(Posting_skip), "create_posting_skip");
    result->id = id;
    result->skip = NULL;
    result->next = NULL;
    return result;
}

/**
 * Frees emmory allocated to posting skip
 * @param posting_skip Posting skip to be deallocated.
 */
void free_posting_skip(Posting_skip_ptr posting_skip) {
    free_(posting_skip);
}

/**
 * Adds a skip pointer to the next skip posting.
 * @param skip Next posting to jump.
 */
void add_skip(Posting_skip_ptr posting_skip, Posting_skip_ptr skip) {
    posting_skip->skip_available = true;
    posting_skip->skip = skip;
}
