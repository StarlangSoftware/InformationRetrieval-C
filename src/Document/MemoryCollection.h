//
// Created by Olcay Taner YILDIZ on 5.09.2023.
//

#ifndef INFORMATIONRETRIEVAL_MEMORYCOLLECTION_H
#define INFORMATIONRETRIEVAL_MEMORYCOLLECTION_H

#include <Dictionary/Dictionary.h>
#include "../Index/IncidenceMatrix.h"
#include "../Index/InvertedIndex.h"
#include "../Index/PositionalIndex.h"
#include "Parameter.h"
#include "../Index/CategoryTree.h"
#include "../Index/TermType.h"

struct memory_collection{
    Dictionary_ptr dictionary;
    Dictionary_ptr phrase_dictionary;
    Dictionary_ptr bi_gram_dictionary;
    Dictionary_ptr tri_gram_dictionary;
    Array_list_ptr documents;
    Incidence_matrix_ptr incidence_matrix;
    Inverted_index_ptr inverted_index;
    Inverted_index_ptr bi_gram_index;
    Inverted_index_ptr tri_gram_index;
    Positional_index_ptr positional_index;
    Inverted_index_ptr phrase_index;
    Positional_index_ptr phrase_positional_index;
    char* name;
    Parameter_ptr parameter;
    Category_tree_ptr category_tree;
    Hash_set_ptr attribute_list;
    Index_type index_type;
};

typedef struct memory_collection Memory_collection;

typedef Memory_collection *Memory_collection_ptr;

void load_attribute_list(Memory_collection_ptr memory_collection);

int collection_size(Memory_collection_ptr memory_collection);

int vocabulary_size(Memory_collection_ptr memory_collection);

void load_categories(Memory_collection_ptr memory_collection);

void construct_n_gram_index(Memory_collection_ptr memory_collection);

Memory_collection_ptr create_memory_collection(char* directory, Parameter_ptr parameter);

void read_documents(Memory_collection_ptr memory_collection, char* directory, Parameter_ptr parameter);

void free_memory_collection(Memory_collection_ptr memory_collection);

void load_indexes_from_file(Memory_collection_ptr memory_collection, char* directory);

Array_list_ptr construct_terms(Memory_collection_ptr memory_collection, Term_type term_type);

void construct_indexes_in_memory(Memory_collection_ptr memory_collection);

void save_memory_collection(Memory_collection_ptr memory_collection);

void save_categories(Memory_collection_ptr memory_collection);

Query_result_ptr filter_according_to_categories(Memory_collection_ptr memory_collection,
                                                Query_result_ptr current_result,
                                                Array_list_ptr categories);

Query_result_ptr search_with_inverted_index(Memory_collection_ptr memory_collection,
                                            Query_ptr query,
                                            Search_parameter_ptr parameter);

Query_result_ptr attribute_search(Memory_collection_ptr memory_collection,
                                  Query_ptr query,
                                  Search_parameter_ptr parameter);

Query_result_ptr search_collection(Memory_collection_ptr memory_collection,
                                   Query_ptr query,
                                   Search_parameter_ptr search_parameter);

#endif //INFORMATIONRETRIEVAL_MEMORYCOLLECTION_H
