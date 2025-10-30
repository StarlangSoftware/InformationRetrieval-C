//
// Created by Olcay Taner YILDIZ on 2.09.2023.
//

#include <stdlib.h>
#include <FileUtils.h>
#include <string.h>
#include <StringUtils.h>
#include <Memory/Memory.h>
#include "InvertedIndex.h"
#include "PostingList.h"
#include "TermOccurrence.h"
#include "TermDictionary.h"

/**
 * Reads the inverted index from an input file.
 * @param file_name Input file name for the inverted index.
 */
Inverted_index_ptr create_inverted_index(const char *file_name) {
    Inverted_index_ptr result = malloc_(sizeof(Inverted_index));
    result->index = create_integer_hash_map();
    read_posting_list(result, file_name);
    return result;
}

/**
 * Reads the postings list of the inverted index from an input file. The postings are stored in two lines. The first
 * line contains the term id and the number of postings for that term. The second line contains the postings
 * list for that term.
 * @param inverted_index Inverted index object.
 * @param file_name Inverted index file.
 */
void read_posting_list(Inverted_index_ptr inverted_index, const char *file_name) {
    char name[MAX_LINE_LENGTH];
    sprintf(name, "%s-postings.txt", file_name);
    Array_list_ptr lines = read_lines(name);
    for (int i = 0; i < lines->size; i++){
        char* line = array_list_get(lines, i);
        if (strlen(line) != 0){
            Array_list_ptr items = str_split(line, ' ');
            int* word_id = malloc_(sizeof(int));
            *word_id = atoi(array_list_get(items, 0));
            free_array_list(items, free_);
            i++;
            line = array_list_get(lines, i);
            hash_map_insert(inverted_index->index, word_id, create_posting_list2(line));
        }
    }
    free_array_list(lines, free_);
}

void free_inverted_index(Inverted_index_ptr inverted_index) {
    free_hash_map2(inverted_index->index, free_, (void (*)(void *)) free_posting_list);
    free_(inverted_index);
}

/**
 * Constructs an inverted index from a list of sorted tokens. The terms array should be sorted before calling this
 * method. Multiple occurrences of the same term from the same document are merged in the index. Instances of the
 * same term are then grouped, and the result is split into a postings list.
 * @param dictionary Term dictionary
 * @param terms Sorted list of tokens in the memory collection.
 */
Inverted_index_ptr create_inverted_index2(Dictionary_ptr dictionary, Array_list_ptr terms) {
    Inverted_index_ptr result = malloc_(sizeof(Inverted_index));
    result->index = create_integer_hash_map();
    if (!is_array_list_empty(terms)){
        Term_occurrence_ptr term = array_list_get(terms, 0);
        int i = 1;
        Term_occurrence_ptr previous_term = term;
        int term_id = get_term_dictionary_word_index(dictionary, term->term);
        add_term_to_inverted_index(result, term_id, term->doc_id);
        int prev_doc_id = term->doc_id;
        while (i < terms->size){
            term = array_list_get(terms, i);
            term_id = get_term_dictionary_word_index(dictionary, term->term);
            if (term_id != -1){
                if (is_different(term, previous_term)){
                    add_term_to_inverted_index(result, term_id, term->doc_id);
                    prev_doc_id = term->doc_id;
                } else {
                    if (prev_doc_id != term->doc_id){
                        add_term_to_inverted_index(result, term_id, term->doc_id);
                        prev_doc_id = term->doc_id;
                    }
                }
            }
            i++;
            previous_term = term;
        }
    }
    return result;
}

/**
 * Adds a possible new term with a document id to the inverted index. First the term is searched in the hash map,
 * then the document id is put into the correct postings list.
 * @param inverted_index Inverted index object.
 * @param term_id Id of the term
 * @param doc_id Document id in which the term exists
 */
void add_term_to_inverted_index(Inverted_index_ptr inverted_index, int term_id, int doc_id) {
    Posting_list_ptr posting_list;
    if (!hash_map_contains(inverted_index->index, &term_id)){
        posting_list = create_posting_list();
        add_to_posting_list(posting_list, doc_id);
        int* id = malloc_(sizeof(int));
        *id = term_id;
        hash_map_insert(inverted_index->index, id, posting_list);
    } else {
        posting_list = hash_map_get(inverted_index->index, &term_id);
        add_to_posting_list(posting_list, doc_id);
    }
}

/**
 * Saves the inverted index into the index file. The postings are stored in two lines. The first
 * line contains the term id and the number of postings for that term. The second line contains the postings
 * list for that term.
 * @param inverted_index Inverted index object.
 * @param file_name Index file name. Real index file name is created by attaching -postings.txt to this
 *                 file name
 */
void save_inverted_index(Inverted_index_ptr inverted_index, char *file_name) {
    FILE* output_file;
    char name[MAX_LINE_LENGTH];
    sprintf(name, "%s-postings.txt", file_name);
    output_file = fopen(name, "w");
    Array_list_ptr keys = key_list(inverted_index->index);
    for (int i = 0; i < keys->size; i++){
        int* key = array_list_get(keys, i);
        Posting_list_ptr posting_list = hash_map_get(inverted_index->index, key);
        posting_list_write_to_file(posting_list, output_file, *key);
    }
    free_array_list(keys, NULL);
    fclose(output_file);
}

/**
 * Searches a given query in the document collection using inverted index boolean search.
 * @param inverted_index Inverted index object.
 * @param query Query string
 * @param dictionary Term dictionary
 * @return The result of the query obtained by doing inverted index boolean search in the collection.
 */
Query_result_ptr search_inverted_index(Inverted_index_ptr inverted_index, Query_ptr query, Dictionary_ptr dictionary) {
    Array_list_ptr query_terms = create_array_list();
    for (int i = 0; i < size_of_query(query); i++){
        int term_index = get_term_dictionary_word_index(dictionary, get_term(query, i));
        if (term_index != -1){
            array_list_add(query_terms, hash_map_get(inverted_index->index, &term_index));
        } else {
            free_array_list(query_terms, NULL);
            return create_query_result();
        }
    }
    array_list_sort(query_terms, (int (*)(const void *, const void *)) compare_posting_list);
    Posting_list_ptr result = clone_posting_list(array_list_get(query_terms, 0));
    for (int i = 1; i < query_terms->size; i++){
        Posting_list_ptr tmp = intersection_posting_list(result, array_list_get(query_terms, i));
        free_posting_list(result);
        result = tmp;
    }
    free_array_list(query_terms, NULL);
    Query_result_ptr query_result = posting_list_to_query_result(result);
    free_posting_list(result);
    return query_result;
}

/**
 * Constructs a sorted array list of frequency counts for a word list and also sorts the word list according to
 * those frequencies.
 * @param inverted_index Inverted index object.
 * @param word_list Word list for which frequency array is constructed.
 * @param dictionary Term dictionary
 */
void auto_complete_word(Inverted_index_ptr inverted_index, Array_list_ptr word_list, Dictionary_ptr dictionary) {
    Array_list_ptr counts = create_array_list();
    for (int i = 0; i < word_list->size; i++){
        char* word = array_list_get(word_list, i);
        int word_index = get_term_dictionary_word_index(dictionary, word);
        Posting_list_ptr posting_list = hash_map_get(inverted_index->index, &word_index);
        array_list_add_int(counts, size_of_posting_list(posting_list));
    }
    for (int i = 0; i < word_list->size; i++) {
        int* count1 = array_list_get(counts, i);
        for (int j = i + 1; j < word_list->size; j++){
            int* count2 = array_list_get(counts, j);
            if (*count1 < *count2){
                array_list_swap(counts, i, j);
                array_list_swap(word_list, i, j);
            }
        }
    }
    free_array_list(counts, free_);
}
