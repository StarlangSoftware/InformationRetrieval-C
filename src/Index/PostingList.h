//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_POSTINGLIST_H
#define INFORMATIONRETRIEVAL_POSTINGLIST_H

#include <ArrayList.h>
#include "../Query/QueryResult.h"

struct posting_list{
    Array_list_ptr postings;
};

typedef struct posting_list Posting_list;

typedef Posting_list *Posting_list_ptr;

Posting_list_ptr create_posting_list();

Posting_list_ptr create_posting_list2(char* line);

Posting_list_ptr clone_posting_list(Posting_list_ptr posting_list);

void free_posting_list(Posting_list_ptr posting_list);

void add_to_posting_list(Posting_list_ptr posting_list, int doc_id);

int size_of_posting_list(const Posting_list* posting_list);

Posting_list_ptr intersection_posting_list(const Posting_list *posting_list, const Posting_list *second_list);

Posting_list_ptr union_with_posting_list(const Posting_list *posting_list, const Posting_list *second_list);

char* posting_list_to_string(const Posting_list *posting_list);

void posting_list_write_to_file(const Posting_list *posting_list, FILE* output_file, int index);

int compare_posting_list(const Posting_list *first, const Posting_list *second);

Query_result_ptr posting_list_to_query_result(const Posting_list *posting_list);

#endif //INFORMATIONRETRIEVAL_POSTINGLIST_H
