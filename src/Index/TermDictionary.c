//
// Created by Olcay Taner YILDIZ on 31.08.2023.
//

#include <stdio.h>
#include <FileUtils.h>
#include <string.h>
#include <stdlib.h>
#include "TermDictionary.h"
#include "Term.h"
#include "TermOccurrence.h"

Dictionary_ptr create_term_dictionary() {
    return create_dictionary();
}

Dictionary_ptr create_term_dictionary2(const char *file_name) {
    char name[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    Dictionary_ptr result = create_term_dictionary();
    sprintf(name, "%s-dictionary.txt", file_name);
    FILE* input_file = fopen(name, "r");
    char* input = fgets(line, MAX_LINE_LENGTH, input_file);
    while (input != NULL){
        if (strlen(line) != 0){
            line[strcspn(line, "\n")] = 0;
            String_ptr first = substring(line, 0, str_find_c(line, " "));
            String_ptr second = substring2(line, str_find_c(line, " ") + 1);
            int term_id = atoi(first->s);
            char* word = second->s;
            Term_ptr term = create_term(word, term_id);
            array_list_add(result->words, term);
            free_string_ptr(first);
            free_string_ptr(second);
        }
        input = fgets(line, MAX_LINE_LENGTH, input_file);
    }
    fclose(input_file);
    update_word_map_term_dictionary(result);
    return result;
}

void update_word_map_term_dictionary(Dictionary_ptr dictionary) {
    for (int i = 0; i < dictionary->words->size; i++) {
        Term_ptr word = array_list_get(dictionary->words, i);
        int *index = malloc(sizeof(int));
        *index = i;
        hash_map_insert(dictionary->word_map, word->name, index);
    }
}

Dictionary_ptr create_term_dictionary3(Array_list_ptr terms) {
    Dictionary_ptr result = create_term_dictionary();
    int term_id = 0;
    if (!is_array_list_empty(terms)){
        Term_occurrence_ptr term = array_list_get(terms, 0);
        add_term(result, term->term->name, term_id);
        term_id++;
        Term_occurrence_ptr previous_term = term;
        int i = 1;
        while (i < terms->size){
            term = array_list_get(terms, i);
            if (is_different(term, previous_term)){
                add_term(result, term->term->name, term_id);
                term_id++;
            }
            i++;
            previous_term = term;
        }
    }
    sort(result);
    return result;
}

Dictionary_ptr create_term_dictionary4(Hash_set_ptr words) {
    Dictionary_ptr result = create_term_dictionary();
    Array_list_ptr word_list = hash_set_key_list(words);
    array_list_sort(word_list, (int (*)(const void *, const void *)) compare_string);
    int term_id = 0;
    for (int i = 0; i < word_list->size; i++){
        char* name = array_list_get(word_list, i);
        add_term(result, name, term_id);
        term_id++;
    }
    free_array_list(word_list, (void (*)(void *)) free_word);
    sort(result);
    return result;
}

Term_ptr get_word_term(const Dictionary* dictionary, const char *name) {
    if (word_exists(dictionary, name)) {
        int index = *(int *) (hash_map_get(dictionary->word_map, name));
        return array_list_get(dictionary->words, index);
    }
    return NULL;
}

void add_term(Dictionary_ptr dictionary, char *name, int term_id) {
    int middle = binary_search_term_dictionary(dictionary, name);
    if (middle < 0){
        array_list_insert(dictionary->words, -middle -1, create_term(name, term_id));
    }
}

int binary_search_term_dictionary(const Dictionary *dictionary, const char *word) {
    int lo = 0;
    int hi = dictionary->words->size - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (strcmp(((Term_ptr) array_list_get(dictionary->words, mid))->name, word) == 0) {
            return mid;
        }
        if (strcmp(((Term_ptr) array_list_get(dictionary->words, mid))->name, word) < 0) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return -(lo + 1);
}

void save_term_dictionary(const Dictionary *dictionary, char *file_name) {
    char name[MAX_LINE_LENGTH];
    sprintf(name, "%s-dictionary.txt", file_name);
    FILE* output_file = fopen(name, "w");
    for (int i = 0; i < dictionary->words->size; i++){
        Term_ptr term = array_list_get(dictionary->words, i);
        fprintf(output_file, "%d %s\n", term->term_id, term->name);
    }
    fclose(output_file);
}

Array_list_ptr construct_n_grams(const char *word, int term_id, int k) {
    Array_list_ptr n_grams = create_array_list();
    if (strlen(word) >= k - 1){
        for (int l = - 1; l < word_size(word) - k + 2; l++){
            char term[MAX_LINE_LENGTH];
            if (l == -1){
                String_ptr s = substring(word, 0, k - 1);
                sprintf(term, "$%s", s->s);
                free_string_ptr(s);
            } else {
                if (l == word_size(word) - k + 1){
                    String_ptr s = substring(word, l, k - 1);
                    sprintf(term, "%s$", s->s);
                    free_string_ptr(s);
                } else {
                    String_ptr s = substring(word, l, k);
                    sprintf(term, "%s", s->s);
                    free_string_ptr(s);
                }
            }
            array_list_add(n_grams, create_term_occurrence(create_word(term), term_id, l));
        }
    }
    return n_grams;
}

Array_list_ptr construct_terms_from_dictionary(const Dictionary *dictionary, int k) {
    Array_list_ptr terms = create_array_list();
    for (int i = 0; i < size(dictionary); i++){
        Term_ptr term = array_list_get(dictionary->words, i);
        char* word = term->name;
        Array_list_ptr term_list = construct_n_grams(word, i, k);
        array_list_add_all(terms, term_list);
    }
    array_list_sort(terms, (int (*)(const void *, const void *)) compare_term_occurrence2);
    return terms;
}

void sort_term_dictionary(Dictionary_ptr dictionary) {
    array_list_sort(dictionary->words, (int (*)(const void *, const void *)) compare_term_occurrence);
    update_word_map_term_dictionary(dictionary);
}
