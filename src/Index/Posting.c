//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#include <stdlib.h>
#include "Posting.h"

Posting_ptr create_posting(int id) {
    Posting_ptr result = malloc(sizeof(Posting));
    result->id = id;
    return result;
}

void free_posting(Posting_ptr posting) {
    free(posting);
}
