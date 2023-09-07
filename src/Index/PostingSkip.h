//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_POSTINGSKIP_H
#define INFORMATIONRETRIEVAL_POSTINGSKIP_H

#include <stdbool.h>

struct posting_skip{
    int id;
    bool skip_available;
    struct posting_skip* skip;
    struct posting_skip* next;
};

typedef struct posting_skip Posting_skip;

typedef Posting_skip *Posting_skip_ptr;

Posting_skip_ptr create_posting_skip(int id);

void free_posting_skip(Posting_skip_ptr posting_skip);

void add_skip(Posting_skip_ptr posting_skip, Posting_skip_ptr skip);

#endif //INFORMATIONRETRIEVAL_POSTINGSKIP_H
