//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_POSITIONALPOSTING_H
#define INFORMATIONRETRIEVAL_POSITIONALPOSTING_H

#include <ArrayList.h>

struct positional_posting{
    Array_list_ptr positions;
    int doc_id;
};

typedef struct positional_posting Positional_posting;

typedef Positional_posting *Positional_posting_ptr;

Positional_posting_ptr create_positional_posting(int doc_id);

Positional_posting_ptr clone_positional_posting(Positional_posting_ptr positional_posting);

void free_positional_posting(Positional_posting_ptr positional_posting);

void add_position(Positional_posting_ptr positional_posting, int position);

int size_of_positional_posting(const Positional_posting* positional_posting);

char* positional_posting_to_string(const Positional_posting *positional_posting);

#endif //INFORMATIONRETRIEVAL_POSITIONALPOSTING_H
