//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#include <Memory/Memory.h>
#include "Posting.h"

/**
 * Constructor for the Posting class. Sets the document id attribute.
 * @param id Document id.
 */
Posting_ptr create_posting(int id) {
    Posting_ptr result = malloc_(sizeof(Posting));
    result->id = id;
    return result;
}

/**
 * Frees memory allocated for a posting
 * @param posting Posting to be deallocated.
 */
void free_posting(Posting_ptr posting) {
    free_(posting);
}
