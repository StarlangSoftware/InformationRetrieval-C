//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#include <HashMap/HashMap.h>
#include <string.h>
#include <Memory/Memory.h>
#include "TermOccurrence.h"

/**
 * Constructor for the TermOccurrence class. Sets the attributes.
 * @param term Term for this occurrence.
 * @param doc_id Document id of the term occurrence.
 * @param position Position of the term in the document for this occurrence.
 */
Term_occurrence_ptr create_term_occurrence(char* term, int doc_id, int position) {
    Term_occurrence_ptr result = malloc_(sizeof(Term_occurrence), "create_term_occurrence");
    result->term = term;
    result->doc_id = doc_id;
    result->position = position;
    return result;
}

void free_term_occurrence(Term_occurrence_ptr term_occurrence) {
    free_(term_occurrence->term);
    free_(term_occurrence);
}

bool is_different(const Term_occurrence *term_occurrence, const Term_occurrence *current_term) {
    return compare_string(term_occurrence->term, current_term->term) != 0;
}

/**
 * Compares two term occurrences.
 * @param term1 the first term occurrence to be compared.
 * @param term2 the second term occurrence to be compared.
 * @return If the term of the first term occurrence is different from the term of the second term occurrence then
 * the method returns the comparison result between those two terms lexicographically. If the term of the first term
 * occurrence is same as the term of the second term occurrence then the term occurrences are compared with respect
 * to their document ids. If the first has smaller document id, the method returns -1; if the second has smaller
 * document id, the method returns +1.  As the third comparison criteria, if also the document ids are the same,
 * the method compares term occurrences with respect to the position. If the first has smaller position, the method
 * returns -1; if the second has smaller position id, the method returns +1, and if all three features are the same,
 * the method returns 0.
 */
int compare_term_occurrence(const Term_occurrence *term1, const Term_occurrence *term2) {
    if (strcmp(term1->term, term2->term) != 0){
        return compare_string(term1->term, term2->term);
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
