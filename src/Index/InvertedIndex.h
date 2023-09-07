//
// Created by Olcay Taner YILDIZ on 2.09.2023.
//

#ifndef INFORMATIONRETRIEVAL_INVERTEDINDEX_H
#define INFORMATIONRETRIEVAL_INVERTEDINDEX_H

#include <HashMap/HashMap.h>
#include <Dictionary/Dictionary.h>
#include "../Query/QueryResult.h"
#include "../Query/Query.h"

struct inverted_index{
    Hash_map_ptr index;
};

typedef struct inverted_index Inverted_index;

typedef struct inverted_index *Inverted_index_ptr;

Inverted_index_ptr create_inverted_index(const char* file_name);

Inverted_index_ptr create_inverted_index2(Dictionary_ptr dictionary, Array_list_ptr terms);

void free_inverted_index(Inverted_index_ptr inverted_index);

void read_posting_list(Inverted_index_ptr inverted_index, const char* file_name);

void add_term_to_inverted_index(Inverted_index_ptr inverted_index, int term_id, int doc_id);

void save_inverted_index(Inverted_index_ptr inverted_index, char* file_name);

Query_result_ptr search_inverted_index(Inverted_index_ptr inverted_index, Query_ptr query, Dictionary_ptr dictionary);

void auto_complete_word(Inverted_index_ptr inverted_index, Array_list_ptr word_list, Dictionary_ptr dictionary);

#endif //INFORMATIONRETRIEVAL_INVERTEDINDEX_H
