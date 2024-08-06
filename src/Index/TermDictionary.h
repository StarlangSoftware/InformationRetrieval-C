//
// Created by Olcay Taner YILDIZ on 31.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_TERMDICTIONARY_H
#define INFORMATIONRETRIEVAL_TERMDICTIONARY_H

#include "Dictionary/Dictionary.h"
#include "Term.h"

Dictionary_ptr create_term_dictionary();

Dictionary_ptr create_term_dictionary2(const char* file_name);

Dictionary_ptr create_term_dictionary3(Array_list_ptr terms);

Dictionary_ptr create_term_dictionary4(Hash_set_ptr words);

void update_word_map_term_dictionary(Dictionary_ptr dictionary);

void add_term(Dictionary_ptr dictionary, char* name, int term_id);

int binary_search_term_dictionary(const Dictionary* dictionary, const char* word);

void save_term_dictionary(const Dictionary* dictionary, char* file_name);

Array_list_ptr construct_n_grams(const char* word, int term_id, int k);

Array_list_ptr construct_terms_from_dictionary(const Dictionary* dictionary, int N);

Term_ptr get_word_term(const Dictionary* dictionary, const char *name);

int get_term_dictionary_word_index(const Dictionary* dictionary, const char *name);

void sort_term_dictionary(Dictionary_ptr dictionary);

void free_term_dictionary(Dictionary_ptr dictionary);

#endif //INFORMATIONRETRIEVAL_TERMDICTIONARY_H
