//
// Created by Olcay Taner YILDIZ on 1.09.2023.
//

#include <Memory/Memory.h>
#include "IncidenceMatrix.h"
#include "TermOccurrence.h"
#include "TermDictionary.h"

/**
 * Empty constructor for the incidence matrix representation. Initializes the incidence matrix according to the
 * given dictionary and document size.
 * @param dictionary_size Number of words in the dictionary (number of distinct words in the collection)
 * @param document_size Number of documents in the collection
 */
Incidence_matrix_ptr create_incidence_matrix(int dictionary_size, int document_size) {
    Incidence_matrix_ptr result = malloc_(sizeof(Incidence_matrix), "create_incidence_matrix_1");
    result->dictionary_size = dictionary_size;
    result->document_size = document_size;
    result->incidence_matrix = malloc_(dictionary_size * sizeof(bool*), "create_incidence_matrix_2");
    for (int i = 0; i < dictionary_size; i++){
        result->incidence_matrix[i] = calloc_(document_size, sizeof(bool),"create_incidence_matrix_3");
        for (int j = 0; j < document_size; j++){
            result->incidence_matrix[i][j] = false;
        }
    }
    return result;
}

/**
 * Constructs an incidence matrix from a list of sorted tokens in the given terms array.
 * @param dictionary Term dictionary
 * @param terms List of tokens in the memory collection.
 * @param document_size Number of documents in the collection
 */
Incidence_matrix_ptr create_incidence_matrix2(Array_list_ptr terms, Dictionary_ptr dictionary, int document_size) {
    Incidence_matrix_ptr result = create_incidence_matrix(size(dictionary), document_size);
    if (terms->size > 0){
        Term_occurrence_ptr term = array_list_get(terms, 0);
        int i = 1;
        int term_index = get_term_dictionary_word_index(dictionary, term->term);
        set(result, term_index, term->doc_id);
        while (i < terms->size){
            term = array_list_get(terms, i);
            term_index = get_term_dictionary_word_index(dictionary, term->term);
            set(result, term_index, term->doc_id);
            i++;
        }
    }
    return result;
}

/**
 * Sets the given cell in the incidence matrix to true.
 * @param incidence_matrix Incidence matrix object.
 * @param row Row no of the cell
 * @param col Column no of the cell
 */
void set(Incidence_matrix_ptr incidence_matrix, int row, int col) {
    incidence_matrix->incidence_matrix[row][col] = true;
}

void free_incidence_matrix(Incidence_matrix_ptr incidence_matrix) {
    for (int i = 0; i < incidence_matrix->dictionary_size; i++){
        free_(incidence_matrix->incidence_matrix[i]);
    }
    free_(incidence_matrix->incidence_matrix);
    free_(incidence_matrix);
}

/**
 * Searches a given query in the document collection using incidence matrix boolean search.
 * @param incidence_matrix Incidence matrix object.
 * @param query Query string
 * @param dictionary Term dictionary
 * @return The result of the query obtained by doing incidence matrix boolean search in the collection.
 */
Query_result_ptr search_incidence_matrix(Incidence_matrix_ptr incidence_matrix, Query_ptr query, Dictionary_ptr dictionary) {
    Query_result_ptr result = create_query_result();
    bool* result_row = malloc_(incidence_matrix->document_size * sizeof(bool), "search_incidence_matrix");
    for (int i = 0; i < incidence_matrix->document_size; i++){
        result_row[i] = true;
    }
    for (int i = 0; i < size_of_query(query); i++){
        int term_index = get_term_dictionary_word_index(dictionary, get_term(query, i));
        if (term_index != -1){
            for (int j = 0; j < incidence_matrix->document_size; j++){
                result_row[j] = result_row[j] && incidence_matrix->incidence_matrix[term_index][j];
            }
        } else {
            free_(result_row);
            return result;
        }
    }
    for (int i = 0; i < incidence_matrix->document_size; i++){
        if (result_row[i]){
            add_to_query_result2(result, i);
        }
    }
    free_(result_row);
    return result;
}
