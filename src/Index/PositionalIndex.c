//
// Created by Olcay Taner YILDIZ on 2.09.2023.
//

#include <stdlib.h>
#include <FileUtils.h>
#include <string.h>
#include <Memory/Memory.h>
#include "PositionalIndex.h"
#include "TermOccurrence.h"
#include "PositionalPostingList.h"
#include "../Query/VectorSpaceModel.h"
#include "../Document/Document.h"
#include "TermDictionary.h"

/**
 * Reads the positional inverted index from an input file.
 * @param file_name Input file name for the positional inverted index.
 */
Positional_index_ptr create_positional_index(const char *file_name) {
    Positional_index_ptr result = malloc_(sizeof(Positional_index), "create_positional_index");
    result->positional_index = create_integer_hash_map();
    read_positional_posting_list(result, file_name);
    return result;
}

/**
 * Constructs a positional inverted index from a list of sorted tokens. The terms array should be sorted before
 * calling this method. Multiple occurrences of the same term from the same document are enlisted separately in the
 * index.
 * @param dictionary Term dictionary
 * @param terms Sorted list of tokens in the memory collection.
 */
Positional_index_ptr create_positional_index2(Dictionary_ptr dictionary, Array_list_ptr terms) {
    Positional_index_ptr result = malloc_(sizeof(Positional_index), "create_positional_index2");
    result->positional_index = create_integer_hash_map();
    if (!is_array_list_empty(terms)){
        Term_occurrence_ptr term = array_list_get(terms, 0);
        int i = 1;
        Term_occurrence_ptr previous_term = term;
        int term_id = get_term_dictionary_word_index(dictionary, term->term);
        add_position_to_positional_index(result, term_id, term->doc_id, term->position);
        int prev_doc_id = term->doc_id;
        while (i < terms->size){
            term = array_list_get(terms, i);
            term_id = get_term_dictionary_word_index(dictionary, term->term);
            if (term_id != -1){
                if (is_different(term, previous_term)){
                    add_position_to_positional_index(result, term_id, term->doc_id, term->position);
                    prev_doc_id = term->doc_id;
                } else {
                    if (prev_doc_id != term->doc_id){
                        add_position_to_positional_index(result, term_id, term->doc_id, term->position);
                        prev_doc_id = term->doc_id;
                    } else {
                        add_position_to_positional_index(result, term_id, term->doc_id, term->position);
                    }
                }
            }
            i++;
            previous_term = term;
        }
    }
    return result;
}

void free_positional_index(Positional_index_ptr positional_index) {
    free_hash_map2(positional_index->positional_index, free_, (void (*)(void *)) free_positional_posting_list);
    free_(positional_index);
}

/**
 * Reads the positional postings list of the positional index from an input file. The postings are stored in n
 * lines. The first line contains the term id and the number of documents that term occurs. Other n - 1 lines
 * contain the postings list for that term for a separate document.
 * @param positional_index Positional index object.
 * @param file_name Positional index file.
 */
void read_positional_posting_list(Positional_index_ptr positional_index, const char *file_name) {
    char name[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    sprintf(name, "%s-positionalPostings.txt", file_name);
    FILE* input_file = fopen(name, "r");
    char* input = fgets(line, MAX_LINE_LENGTH, input_file);
    while (input != NULL){
        if (strlen(line) != 0){
            line[strcspn(line, "\n")] = 0;
            Array_list_ptr items = str_split(line, ' ');
            int* word_id = malloc_(sizeof(int), "read_positional_posting_list");
            *word_id = atoi(array_list_get(items, 0));
            int count = atoi(array_list_get(items, 1));
            hash_map_insert(positional_index->positional_index, word_id, create_positional_posting_list(input_file, count));
            free_array_list(items, free_);
        }
        input = fgets(line, MAX_LINE_LENGTH, input_file);
    }
    fclose(input_file);
}

/**
 * Adds a possible new term with a position and document id to the positional index. First the term is searched in
 * the hash map, then the position and the document id is put into the correct postings list.
 * @param positional_index Positional index object.
 * @param term_id Id of the term
 * @param doc_id Document id in which the term exists
 * @param position Position of the term in the document with id docId
 */
void add_position_to_positional_index(Positional_index_ptr positional_index, int term_id, int doc_id, int position) {
    Positional_posting_list_ptr positional_posting_list;
    if (!hash_map_contains(positional_index->positional_index, &term_id)){
        positional_posting_list = create_positional_posting_list2();
        add_to_positional_posting_list(positional_posting_list, doc_id, position);
        int* id = malloc_(sizeof(int), "add_position_to_positional_index");
        *id = term_id;
        hash_map_insert(positional_index->positional_index, id, positional_posting_list);
    } else {
        positional_posting_list = hash_map_get(positional_index->positional_index, &term_id);
        add_to_positional_posting_list(positional_posting_list, doc_id, position);
    }
}

/**
 * Saves the positional index into the index file. The postings are stored in n lines. The first line contains the
 * term id and the number of documents that term occurs. Other n - 1 lines contain the postings list for that term
 * for a separate document.
 * @param positional_index Positional index object.
 * @param file_name Index file name. Real index file name is created by attaching -positionalPostings.txt to this
 *                 file name
 */
void save_positional_index(Positional_index_ptr positional_index, char *file_name) {
    FILE* output_file;
    char name[MAX_LINE_LENGTH];
    sprintf(name, "%s-positionalPostings.txt", file_name);
    output_file = fopen(name, "w");
    Array_list_ptr keys = key_list(positional_index->positional_index);
    for (int i = 0; i < keys->size; i++){
        int* key = array_list_get(keys, i);
        Positional_posting_list_ptr posting_list = hash_map_get(positional_index->positional_index, key);
        positional_posting_list_write_to_file(posting_list, output_file, *key);
    }
    free_array_list(keys, NULL);
    fclose(output_file);
}

/**
 * Searches a given query in the document collection using positional index boolean search.
 * @param positional_index Positional index object.
 * @param query Query string
 * @param dictionary Term dictionary
 * @return The result of the query obtained by doing positional index boolean search in the collection.
 */
Query_result_ptr positional_search(Positional_index_ptr positional_index, Query_ptr query, Dictionary_ptr dictionary) {
    Positional_posting_list_ptr posting_result = NULL;
    for (int i = 0; i < size_of_query(query); i++){
        int term = get_term_dictionary_word_index(dictionary, get_term(query, i));
        if (term != -1){
            if (i == 0){
                posting_result = clone_positional_posting_list(hash_map_get(positional_index->positional_index, &term));
            } else {
                if (size_of_positional_posting_list(posting_result) != 0){
                    Positional_posting_list_ptr tmp = intersection_with_positional_posting_list(posting_result, hash_map_get(positional_index->positional_index, &term));
                    free_positional_posting_list(posting_result);
                    posting_result = tmp;
                } else {
                    return create_query_result();
                }
            }
        } else {
            return create_query_result();
        }
    }
    if (posting_result != NULL && size_of_positional_posting_list(posting_result) != 0){
        Query_result_ptr query_result = to_query_result(posting_result);
        free_positional_posting_list(posting_result);
        return query_result;
    } else {
        return create_query_result();
    }
}

/**
 * Searches a given query in the document collection using inverted index ranked search.
 * @param positional_index Positional index object.
 * @param query Query string
 * @param dictionary Term dictionary
 * @param documents Document collection
 * @param search_parameter Search parameter
 * @return The result of the query obtained by doing inverted index ranked search in the collection.
 */
Query_result_ptr ranked_search(Positional_index_ptr positional_index,
                               Query_ptr query,
                               Dictionary_ptr dictionary,
                               Array_list_ptr documents,
                               const Search_parameter *search_parameter) {
    int N = documents->size;
    Hash_map_ptr scores = create_integer_hash_map();
    Query_result_ptr result = create_query_result();
    Positional_posting_list_ptr positional_posting_list;
    for (int i = 0; i < size_of_query(query); i++) {
        int term = get_term_dictionary_word_index(dictionary, get_term(query, i));
        if (term != -1) {
            positional_posting_list = hash_map_get(positional_index->positional_index, &term);
            for (int j = 0; j < size_of_positional_posting_list(positional_posting_list); j++){
                Positional_posting_ptr positional_posting = array_list_get(positional_posting_list->postings, j);
                int doc_id = positional_posting->doc_id;
                int tf = size_of_positional_posting(positional_posting);
                int df = positional_index->positional_index->count;
                if (tf > 0 && df > 0){
                    double score = weighting(tf,
                                             df,
                                             N,
                                             search_parameter->term_weighting,
                                             search_parameter->document_weighting);
                    int* id;
                    double* current_score;
                    if (hash_map_contains(scores, &doc_id)){
                        current_score = hash_map_get(scores, &doc_id);
                        *current_score += score;
                    } else {
                        id = malloc_(sizeof(int), "ranked_search_1");
                        *id = doc_id;
                        current_score = malloc_(sizeof(double), "ranked_search_2");
                        *current_score = score;
                        hash_map_insert(scores, id, current_score);
                    }
                }
            }
        }
    }
    Array_list_ptr keys = key_list(scores);
    for (int i = 0; i < keys->size; i++){
        int* doc_id = array_list_get(keys, i);
        double* score = hash_map_get(scores, doc_id);
        Document_ptr document = array_list_get(documents, *doc_id);
        add_to_query_result(result, *doc_id, (*score) / document->size);
    }
    free_array_list(keys, NULL);
    free_hash_map2(scores, free_, free_);
    return result;
}

/**
 * Returns the term frequencies  in a given document.
 * @param positional_index Positional index object.
 * @param doc_id Id of the document
 * @return Term frequencies of the given document.
 */
int *get_term_frequencies(Positional_index_ptr positional_index, int doc_id) {
    int* tf = malloc_(positional_index->positional_index->count * sizeof(int), "get_term_frequencies");
    int i = 0;
    Array_list_ptr values = value_list(positional_index->positional_index);
    for (int j = 0; j < values->size; j++){
        Positional_posting_list_ptr positional_posting_list = array_list_get(values, j);
        int index = get_index(positional_posting_list, doc_id);
        if (index != -1){
            tf[i] = size_of_positional_posting(get_positional_posting(positional_posting_list, index));
        } else {
            tf[i] = 0;
        }
        i++;
    }
    free_array_list(values, NULL);
    return tf;
}

/**
 * Returns the document frequencies of the terms in the collection.
 * @param positional_index Positional index object.
 * @return The document frequencies of the terms in the collection.
 */
int *get_document_frequencies(Positional_index_ptr positional_index) {
    int* df = malloc_(positional_index->positional_index->count * sizeof(int), "get_document_frequencies");
    int i = 0;
    Array_list_ptr values = value_list(positional_index->positional_index);
    for (int j = 0; j < values->size; j++) {
        Positional_posting_list_ptr positional_posting_list = array_list_get(values, j);
        df[i] = size_of_positional_posting_list(positional_posting_list);
        i++;
    }
    free_array_list(values, NULL);
    return df;
}

int *get_document_sizes(Positional_index_ptr positional_index, int document_size) {
    int* sizes = malloc_(document_size * sizeof(int), "get_document_sizes");
    Array_list_ptr values = value_list(positional_index->positional_index);
    for (int j = 0; j < values->size; j++) {
        Positional_posting_list_ptr positional_posting_list = array_list_get(values, j);
        for (int k = 0; k < size_of_positional_posting_list(positional_posting_list); k++){
            Positional_posting_ptr positional_posting = get_positional_posting(positional_posting_list, k);
            int doc_id = positional_posting->doc_id;
            sizes[doc_id] += size_of_positional_posting(positional_posting);
        }
    }
    free_array_list(values, NULL);
    return sizes;
}

/**
 * Calculates and updates the frequency counts of the terms in each category node.
 * @param positional_index Positional index object.
 * @param documents Document collection.
 */
void set_category_counts(Positional_index_ptr positional_index, Array_list_ptr documents) {
    Array_list_ptr keys = key_list(positional_index->positional_index);
    for (int j = 0; j < keys->size; j++) {
        int *item = array_list_get(keys, j);
        Positional_posting_list_ptr positional_posting_list = hash_map_get(positional_index->positional_index, item);
        for (int k = 0; k < size_of_positional_posting_list(positional_posting_list); k++){
            Positional_posting_ptr positional_posting = get_positional_posting(positional_posting_list, k);
            int doc_id = positional_posting->doc_id;
            Document_ptr document = array_list_get(documents, doc_id);
            add_counts(document->category, *item, size_of_positional_posting(positional_posting));
        }
    }
    free_array_list(keys, NULL);
}
