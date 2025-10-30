//
// Created by Olcay Taner YILDIZ on 31.08.2023.
//

#include <stdio.h>
#include <FileUtils.h>
#include <string.h>
#include <stdlib.h>
#include <Memory/Memory.h>
#include "TermDictionary.h"
#include "Term.h"
#include "TermOccurrence.h"

/**
 * Constructor of the TermDictionary. Initializes the comparator for terms and the hasp map.
 */
Dictionary_ptr create_term_dictionary() {
    return create_dictionary();
}

/**
 * Constructor of the TermDictionary. Reads the terms and their ids from the given dictionary file. Each line stores
 * the term id and the term name separated via space.
 * @param file_name Dictionary file name
 */
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

/**
 * Frees memory allocated for the dictionary. Frees memory allocated for the words and word hash map.
 * @param dictionary Dictionary to be freed.
 */
void free_term_dictionary(Dictionary_ptr dictionary) {
    free_array_list(dictionary->words, (void (*)(void *)) free_term);
    free_hash_map(dictionary->word_map, free_);
    free_(dictionary);
}

/**
 * Updates word map so that word index at i is in the hash map with key word and value i.
 * @param dictionary Current dictionary
 */
void update_word_map_term_dictionary(Dictionary_ptr dictionary) {
    for (int i = 0; i < dictionary->words->size; i++) {
        Term_ptr word = array_list_get(dictionary->words, i);
        int *index = malloc_(sizeof(int));
        *index = i;
        hash_map_insert(dictionary->word_map, word->name, index);
    }
}

/**
 * The getWordIndex method takes a String name as an input and performs binary search within words vector and assigns the result
 * to integer variable middle. If the middle is greater than 0, it returns the index middle, -1 otherwise.
 *
 * @param dictionary Current dictionary.
 * @param name String input.
 * @return found index of words vector, -1 if cannot be found.
 */
int get_term_dictionary_word_index(const Dictionary* dictionary, const char *name) {
    if (hash_map_contains(dictionary->word_map, name)) {
        return *(int *) (hash_map_get(dictionary->word_map, name));
    } else {
        return -1;
    }
}

/**
 * Constructs the TermDictionary from a list of tokens (term occurrences). The terms array should be sorted
 * before calling this method. Constructs the distinct terms and their corresponding term ids.
 * @param terms Sorted list of tokens in the memory collection.
 */
Dictionary_ptr create_term_dictionary3(Array_list_ptr terms) {
    Dictionary_ptr result = create_term_dictionary();
    int term_id = 0;
    if (!is_array_list_empty(terms)){
        Term_occurrence_ptr term = array_list_get(terms, 0);
        add_term(result, term->term, term_id);
        term_id++;
        Term_occurrence_ptr previous_term = term;
        int i = 1;
        while (i < terms->size){
            term = array_list_get(terms, i);
            if (is_different(term, previous_term)){
                add_term(result, term->term, term_id);
                term_id++;
            }
            i++;
            previous_term = term;
        }
    }
    sort_term_dictionary(result);
    return result;
}

/**
 * Constructs the TermDictionary from a hash set of tokens (strings). Constructs sorted dictinct terms array and
 * their corresponding term ids.
 * @param words Hash set of tokens in the memory collection.
 */
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
    free_array_list(word_list, (void (*)(void *)) free_);
    sort_term_dictionary(result);
    return result;
}

/**
 * The getWord method takes a String name as an input and performs binary search within words vector and assigns the result
 * to integer variable middle. If the middle is greater than 0, it returns the item at index middle of words vector, null otherwise.
 *
 * @param dictionary Current dictionary
 * @param name String input.
 * @return the item at found index of words vector, NULL if cannot be found.
 */
Term_ptr get_word_term(const Dictionary* dictionary, const char *name) {
    if (word_exists(dictionary, name)) {
        int index = *(int *) (hash_map_get(dictionary->word_map, name));
        return array_list_get(dictionary->words, index);
    }
    return NULL;
}

/**
 * Adds a new term to the sorted words array. First the term is searched in the words array using binary search,
 * then the word is added into the correct place.
 * @param dictionary Current dictionary.
 * @param name Lemma of the term
 * @param term_id Id of the term
 */
void add_term(Dictionary_ptr dictionary, const char *name, int term_id) {
    int middle = binary_search_term_dictionary(dictionary, name);
    if (middle < 0){
        array_list_insert(dictionary->words, -middle -1, create_term(name, term_id));
    }
}

/**
 * Checks if a given word exists in the dictionary by performing a binary search on the words array.
 * @param dictionary Dictionary to be searched.
 * @param word Searched word
 * @return the index of the search word, if it is contained in the words array; otherwise, (-(insertion point) - 1). The
 * insertion point is defined as the point at which the word would be inserted into the words array.
 */
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

/**
 * Saves the term dictionary into the dictionary file. Each line stores the term id and the term name separated via
 * space.
 * @param dictionary Current dictionary.
 * @param file_name Dictionary file name. Real dictionary file name is created by attaching -dictionary.txt to this
 *                 file name
 */
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

/**
 * Constructs all NGrams from a given word. For example, 3 grams for word "term" are "$te", "ter", "erm", "rm$".
 * @param word Word for which NGrams will b created.
 * @param term_id Term id to add into the posting list.
 * @param N N in NGram.
 * @return An array of NGrams for a given word.
 */
Array_list_ptr construct_n_grams(const char *word, int term_id, int N) {
    Array_list_ptr n_grams = create_array_list();
    if (strlen(word) >= N - 1){
        for (int l = - 1; l < word_size(word) - N + 2; l++){
            char term[MAX_LINE_LENGTH];
            if (l == -1){
                String_ptr s = substring(word, 0, N - 1);
                sprintf(term, "$%s", s->s);
                free_string_ptr(s);
            } else {
                if (l == word_size(word) - N + 1){
                    String_ptr s = substring(word, l, N - 1);
                    sprintf(term, "%s$", s->s);
                    free_string_ptr(s);
                } else {
                    String_ptr s = substring(word, l, N);
                    sprintf(term, "%s", s->s);
                    free_string_ptr(s);
                }
            }
            char* tmp = str_copy(tmp, term);
            array_list_add(n_grams, create_term_occurrence(tmp, term_id, l));
        }
    }
    return n_grams;
}

/**
 * Constructs all NGrams for all words in the dictionary. For example, 3 grams for word "term" are "$te", "ter",
 * "erm", "rm$".
 * @param dictionary Current dictionary.
 * @param N N in NGram.
 * @return A sorted array of NGrams for all words in the dictionary.
 */
Array_list_ptr construct_terms_from_dictionary(const Dictionary *dictionary, int N) {
    Array_list_ptr terms = create_array_list();
    for (int i = 0; i < size(dictionary); i++){
        Term_ptr term = array_list_get(dictionary->words, i);
        char* word = term->name;
        Array_list_ptr term_list = construct_n_grams(word, i, N);
        array_list_add_all(terms, term_list);
        free_array_list(term_list, NULL);
    }
    array_list_sort(terms, (int (*)(const void *, const void *)) compare_term_occurrence);
    return terms;
}

/**
 * Sorts the words array according to the comparator function.
 * @param dictionary Current dictionary
 */
void sort_term_dictionary(Dictionary_ptr dictionary) {
    array_list_sort(dictionary->words, (int (*)(const void *, const void *)) compare_term);
    update_word_map_term_dictionary(dictionary);
}
