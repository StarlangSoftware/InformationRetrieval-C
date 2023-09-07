//
// Created by Olcay Taner YILDIZ on 1.09.2023.
//

#ifndef INFORMATIONRETRIEVAL_INCIDENCEMATRIX_H
#define INFORMATIONRETRIEVAL_INCIDENCEMATRIX_H

#include <stdbool.h>
#include <ArrayList.h>
#include <Dictionary/Dictionary.h>
#include "../Query/QueryResult.h"
#include "../Query/Query.h"

struct incidence_matrix{
    bool** incidence_matrix;
    int dictionary_size;
    int document_size;
};

typedef struct incidence_matrix Incidence_matrix;

typedef struct incidence_matrix *Incidence_matrix_ptr;

Incidence_matrix_ptr create_incidence_matrix(int dictionary_size, int document_size);

Incidence_matrix_ptr create_incidence_matrix2(Array_list_ptr terms, Dictionary_ptr dictionary, int document_size);

void free_incidence_matrix(Incidence_matrix_ptr incidence_matrix);

void set(Incidence_matrix_ptr incidence_matrix, int row, int col);

Query_result_ptr search_incidence_matrix(Incidence_matrix_ptr incidence_matrix, Query_ptr query, Dictionary_ptr dictionary);

#endif //INFORMATIONRETRIEVAL_INCIDENCEMATRIX_H
