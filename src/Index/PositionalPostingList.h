//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_POSITIONALPOSTINGLIST_H
#define INFORMATIONRETRIEVAL_POSITIONALPOSTINGLIST_H

#include <ArrayList.h>
#include <stdio.h>
#include "../Query/QueryResult.h"
#include "PositionalPosting.h"

struct positional_posting_list{
    Array_list_ptr postings;
};

typedef struct positional_posting_list Positional_posting_list;

typedef Positional_posting_list *Positional_posting_list_ptr;

Positional_posting_list_ptr create_positional_posting_list(FILE* input_file, int count);

Positional_posting_list_ptr create_positional_posting_list2();

Positional_posting_list_ptr clone_positional_posting_list(Positional_posting_list_ptr positional_posting_list);

void free_positional_posting_list(Positional_posting_list_ptr positional_posting_list);

int size_of_positional_posting_list(const Positional_posting_list* positional_posting_list);

int get_index(const Positional_posting_list* positional_posting_list, int doc_id);

Query_result_ptr to_query_result(const Positional_posting_list* positional_posting_list);

void add_to_positional_posting_list(Positional_posting_list_ptr positional_posting_list, int doc_id, int position);

Positional_posting_ptr get_positional_posting(const Positional_posting_list* positional_posting_list, int index);

Positional_posting_list_ptr union_with_positional_posting_list(const Positional_posting_list* first, const Positional_posting_list* second);

Positional_posting_list_ptr intersection_with_positional_posting_list(const Positional_posting_list* first, const Positional_posting_list* second);

char* positional_posting_list_to_string(const Positional_posting_list *positional_posting_list);

void positional_posting_list_write_to_file(const Positional_posting_list *positional_posting_list, FILE* output_file, int index);

#endif //INFORMATIONRETRIEVAL_POSITIONALPOSTINGLIST_H
