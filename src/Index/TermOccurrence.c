//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#include <stdlib.h>
#include <HashMap/HashMap.h>
#include <string.h>
#include "TermOccurrence.h"

Term_occurrence_ptr create_term_occurrence(Word_ptr term, int doc_id, int position) {
    Term_occurrence_ptr result = malloc(sizeof(Term_occurrence));
    result->term = term;
    result->doc_id = doc_id;
    result->position = position;
    return result;
}

void free_term_occurrence(Term_occurrence_ptr term_occurrence) {
    free_word(term_occurrence->term);
    free(term_occurrence);
}

bool is_different(const Term_occurrence *term_occurrence, const Term_occurrence *current_term) {
    return compare_word(term_occurrence->term, current_term->term) != 0;
}

int compare_term_occurrence(const Term_occurrence *term1, const Term_occurrence *term2) {
    return compare_string(term1->term->name, term2->term->name);
}

int compare_term_occurrence2(const Term_occurrence *term1, const Term_occurrence *term2) {
    if (strcmp(term1->term->name, term2->term->name) != 0){
        return compare_string(term1->term->name, term2->term->name);
    } else {
        if (term1->doc_id == term2->doc_id){
            if (term1->position == term2->position){
                return 0;
            } else {
                if (term1->position < term2->position){
                    return -1;
                } else {
                    return 1;
                }
            }
        } else {
            if (term1->doc_id < term2->doc_id){
                return -1;
            } else {
                return 1;
            }
        }
    }
}
