//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_TERM_H
#define INFORMATIONRETRIEVAL_TERM_H

struct term{
    char* name;
    int term_id;
};

typedef struct term Term;

typedef Term *Term_ptr;

Term_ptr create_term(const char* name, int term_id);

void free_term(Term_ptr term);

#endif //INFORMATIONRETRIEVAL_TERM_H
