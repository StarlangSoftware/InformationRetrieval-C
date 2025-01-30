//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#include <StringUtils.h>
#include <HashMap/HashMap.h>
#include <Memory/Memory.h>
#include "Term.h"

/**
 * Constructor for the Term class. Sets the fields.
 * @param name Text of the term
 * @param term_id Id of the term
 */
Term_ptr create_term(const char *name, int term_id) {
    Term_ptr result = malloc_(sizeof(Term), "create_term");
    result->name = str_copy(result->name, name);
    result->term_id = term_id;
    return result;
}

void free_term(Term_ptr term) {
    free_(term->name);
    free_(term);
}

int compare_term(const Term *term1, const Term *term2) {
    return compare_string(term1->name, term2->name);
}

