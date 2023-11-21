//
// Created by Olcay Taner YILDIZ on 1.09.2023.
//

#include <stdlib.h>
#include "IncidenceMatrix.h"
#include "TermOccurrence.h"
#include "TermDictionary.h"

Incidence_matrix_ptr create_incidence_matrix(int dictionary_size, int document_size) {
    Incidence_matrix_ptr result = malloc(sizeof(Incidence_matrix));
    result->dictionary_size = dictionary_size;
    result->document_size = document_size;
    result->incidence_matrix = malloc(dictionary_size * sizeof(bool*));
    for (int i = 0; i < dictionary_size; i++){
        result->incidence_matrix[i] = calloc(document_size, sizeof(bool));
        for (int j = 0; j < document_size; j++){
            result->incidence_matrix[i][j] = false;
        }
    }
    return result;
}

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

void set(Incidence_matrix_ptr incidence_matrix, int row, int col) {
    incidence_matrix->incidence_matrix[row][col] = true;
}

void free_incidence_matrix(Incidence_matrix_ptr incidence_matrix) {
    for (int i = 0; i < incidence_matrix->dictionary_size; i++){
        free(incidence_matrix->incidence_matrix[i]);
    }
    free(incidence_matrix->incidence_matrix);
    free(incidence_matrix);
}

Query_result_ptr search_incidence_matrix(Incidence_matrix_ptr incidence_matrix, Query_ptr query, Dictionary_ptr dictionary) {
    Query_result_ptr result = create_query_result();
    bool* result_row = malloc(incidence_matrix->document_size * sizeof(bool));
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
            return result;
        }
    }
    for (int i = 0; i < incidence_matrix->document_size; i++){
        if (result_row[i]){
            add_to_query_result2(result, i);
        }
    }
    free(result_row);
    return result;
}
