//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_TERMOCCURRENCE_H
#define INFORMATIONRETRIEVAL_TERMOCCURRENCE_H

#include <Dictionary/Word.h>

struct term_occurrence{
    char* term;
    int doc_id;
    int position;
};

typedef struct term_occurrence Term_occurrence;

typedef Term_occurrence *Term_occurrence_ptr;

Term_occurrence_ptr create_term_occurrence(char* term, int doc_id, int position);

void free_term_occurrence(Term_occurrence_ptr term_occurrence);

bool is_different(const Term_occurrence* term_occurrence, const Term_occurrence* current_term);

int compare_term_occurrence(const Term_occurrence* term1, const Term_occurrence* term2);

#endif //INFORMATIONRETRIEVAL_TERMOCCURRENCE_H
