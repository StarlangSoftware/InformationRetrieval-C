//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#include <math.h>
#include <Memory/Memory.h>
#include "VectorSpaceModel.h"

/**
 * Constructor for the VectorSpaceModel class. Calculates the normalized tf-idf vector of a single document.
 * @param term_frequencies Term frequencies in the document
 * @param document_frequencies Document frequencies of terms.
 * @param document_size Number of documents in the collection
 * @param term_weighting Term weighting scheme applied in term frequency calculation.
 * @param document_weighting Document weighting scheme applied in document frequency calculation.
 */
Vector_space_model_ptr create_vector_space_model(const int *term_frequencies,
                                                 const int *document_frequencies,
                                                 int document_size,
                                                 Term_weighting term_weighting,
                                                 Document_weighting document_weighting) {
    Vector_space_model_ptr result = malloc_(sizeof(Vector_space_model), "create_vector_space_model");
    result->document_size = document_size;
    result->model = malloc_(document_size * sizeof(double), "create_vector_space_model");
    double sum = 0.0;
    for (int i = 0; i < result->document_size; i++){
        result->model[i] = weighting(term_frequencies[i], document_frequencies[i], document_size, term_weighting, document_weighting);
        sum += result->model[i] * result->model[i];
    }
    for (int i = 0; i < document_size; i++){
        result->model[i] /= sqrt(sum);
    }
    return result;
}

void free_vector_space_model(Vector_space_model_ptr vector_space_model) {
    free_(vector_space_model->model);
    free_(vector_space_model);
}

/**
 * Returns the tf-idf value for a column at position index
 * @param vector_space_model Current vector space model.
 * @param index Position of the column
 * @return tf-idf value for a column at position index
 */
double get_weight(const Vector_space_model* vector_space_model, int index) {
    return vector_space_model->model[index];
}

/**
 * Calculates the cosine similarity between this document vector and the given second document vector.
 * @param first Document vector of the first document.
 * @param second Document vector of the second document.
 * @return Cosine similarity between this document vector and the given second document vector.
 */
double cosine_similarity(const Vector_space_model *first, const Vector_space_model *second) {
    double sum = 0.0;
    for (int i = 0; i < first->document_size; i++){
        sum += first->model[i] * second->model[i];
    }
    return sum;
}

/**
 * Calculates tf-idf value of a single word (column) of the document vector.
 * @param term_frequency Term frequency of this word in the document
 * @param document_frequency Document frequency of this word.
 * @param document_size Number of documents in the collection
 * @param term_weighting Term weighting scheme applied in term frequency calculation.
 * @param document_weighting Document weighting scheme applied in document frequency calculation.
 * @return tf-idf value of a single word (column) of the document vector.
 */
double weighting(double term_frequency,
                 double document_frequency,
                 int document_size,
                 Term_weighting term_weighting,
                 Document_weighting document_weighting) {
    double multiplier1 = 1, multiplier2 = 1;
    switch (term_weighting){
        case   NATURAL:
            multiplier1 = term_frequency;
            break;
        case LOGARITHM:
            if (term_frequency > 0)
                multiplier1 = 1 + log(term_frequency);
            else
                multiplier1 = 0;
            break;
        case     BOOLE:
            if (term_frequency > 0){
                multiplier1 = 1;
            } else {
                multiplier1 = 0;
            }
    }
    switch (document_weighting){
        case   NO_IDF:
            multiplier2 = 1;
            break;
        case 	   IDF:
            multiplier2 = log(document_size / (document_frequency + 0.0));
            break;
        case PROBABILISTIC_IDF:
            if (document_size > 2 * document_frequency){
                multiplier2 = log((document_size - document_frequency) / (document_frequency + 0.0));
            } else {
                multiplier2 = 0.0;
            }
            break;
    }
    return multiplier1 * multiplier2;
}
