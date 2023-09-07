//
// Created by Olcay Taner YILDIZ on 2.09.2023.
//

#ifndef INFORMATIONRETRIEVAL_POSITIONALINDEX_H
#define INFORMATIONRETRIEVAL_POSITIONALINDEX_H

#include <HashMap/HashMap.h>
#include <Dictionary/Dictionary.h>
#include "../Query/QueryResult.h"
#include "../Query/Query.h"
#include "../Query/SearchParameter.h"

struct positional_index{
    Hash_map_ptr positional_index;
};

typedef struct positional_index Positional_index;

typedef Positional_index *Positional_index_ptr;

Positional_index_ptr create_positional_index(const char* file_name);

Positional_index_ptr create_positional_index2(Dictionary_ptr dictionary, Array_list_ptr terms);

void free_positional_index(Positional_index_ptr positional_index);

void read_positional_posting_list(Positional_index_ptr positional_index, const char* file_name);

void add_position_to_positional_index(Positional_index_ptr positional_index, int term_id, int doc_id, int position);

void save_positional_index(Positional_index_ptr positional_index, char* file_name);

Query_result_ptr positional_search(Positional_index_ptr positional_index, Query_ptr query, Dictionary_ptr dictionary);

Query_result_ptr ranked_search(Positional_index_ptr positional_index, Query_ptr query, Dictionary_ptr dictionary, Array_list_ptr documents, const Search_parameter* search_parameter);

int* get_term_frequencies(Positional_index_ptr positional_index, int doc_id);

int* get_document_frequencies(Positional_index_ptr positional_index);

int* get_document_sizes(Positional_index_ptr positional_index, int document_size);

void set_category_counts(Positional_index_ptr positional_index, Array_list_ptr documents);

#endif //INFORMATIONRETRIEVAL_POSITIONALINDEX_H
