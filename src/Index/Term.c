//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#include <stdlib.h>
#include <StringUtils.h>
#include "Term.h"

Term_ptr create_term(const char *name, int term_id) {
    Term_ptr result = malloc(sizeof(Term));
    result->name = str_copy(result->name, name);
    result->term_id = term_id;
    return result;
}

void free_term(Term_ptr term) {
    free(term->name);
    free(term);
}
