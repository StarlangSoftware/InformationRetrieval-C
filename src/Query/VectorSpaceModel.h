//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_VECTORSPACEMODEL_H
#define INFORMATIONRETRIEVAL_VECTORSPACEMODEL_H

#include "../Index/TermWeighting.h"
#include "../Document/DocumentWeighting.h"

struct vector_space_model{
    double* model;
    int document_size;
};

typedef struct vector_space_model Vector_space_model;

typedef Vector_space_model *Vector_space_model_ptr;

Vector_space_model_ptr create_vector_space_model(int* term_frequencies,
                                                 int* document_frequencies,
                                                 int document_size,
                                                 Term_weighting term_weighting,
                                                 Document_weighting document_weighting);

void free_vector_space_model(Vector_space_model_ptr vector_space_model);

double get_weight(const Vector_space_model* vector_space_model, int index);

double cosine_similarity(const Vector_space_model* first, const Vector_space_model* second);

double weighting(double term_frequency,
                 double document_frequency,
                 int document_size,
                 Term_weighting term_weighting,
                 Document_weighting document_weighting);

#endif //INFORMATIONRETRIEVAL_VECTORSPACEMODEL_H
