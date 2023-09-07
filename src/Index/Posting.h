//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_POSTING_H
#define INFORMATIONRETRIEVAL_POSTING_H

struct posting{
    int id;
};

typedef struct posting Posting;

typedef Posting* Posting_ptr;

Posting_ptr create_posting(int id);

void free_posting(Posting_ptr posting);

#endif //INFORMATIONRETRIEVAL_POSTING_H
