//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#include <stdlib.h>
#include "PostingSkip.h"

Posting_skip_ptr create_posting_skip(int id) {
    Posting_skip_ptr result = malloc(sizeof(Posting_skip));
    result->id = id;
    result->skip = NULL;
    result->next = NULL;
    return result;
}

void free_posting_skip(Posting_skip_ptr posting_skip) {
    free(posting_skip);
}

void add_skip(Posting_skip_ptr posting_skip, Posting_skip_ptr skip) {
    posting_skip->skip_available = true;
    posting_skip->skip = skip;
}
