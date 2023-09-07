//
// Created by Olcay Taner YILDIZ on 7.09.2023.
//

#include <stdio.h>
#include "../src/Document/Parameter.h"
#include "../src/Document/MemoryCollection.h"

void test_incidence_matrix_small(){
    Parameter_ptr parameter = create_parameter();
    parameter->index_type = INCIDENCE_MATRIX;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection2", parameter);
    if (collection_size(collection) != 2){
        printf("Error in collection size %d\n", collection_size(collection));
    }
    if (vocabulary_size(collection) != 26){
        printf("Error in vocabulary size %d\n", vocabulary_size(collection));
    }
    free_memory_collection(collection);
}

void test_incidence_matrix_single_query(Memory_collection_ptr collection, char* s, int size){
    Query_ptr query = create_query(s);
    Search_parameter_ptr search_parameter = create_search_parameter();
    search_parameter->retrieval_type = BOOLEAN;
    Query_result_ptr result = search_collection(collection, query, search_parameter);
    if (size_of_query_result(result) != size){
        printf("Error in query %s result size %d\n", s, size_of_query_result(result));
    }
    free_query(query);
    free_query_result(result);
    free_search_parameter(search_parameter);
}

void test_incidence_matrix_query(){
    Parameter_ptr parameter = create_parameter();
    parameter->index_type = INCIDENCE_MATRIX;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection2", parameter);
    test_incidence_matrix_single_query(collection, "Brutus", 2);
    test_incidence_matrix_single_query(collection, "Brutus Caesar", 2);
    test_incidence_matrix_single_query(collection, "enact", 1);
    test_incidence_matrix_single_query(collection, "noble", 1);
    test_incidence_matrix_single_query(collection, "a", 0);
}

void test_inverted_matrix_single_query(Memory_collection_ptr collection, char* s, int size){
    Query_ptr query = create_query(s);
    Search_parameter_ptr search_parameter = create_search_parameter();
    search_parameter->retrieval_type = BOOLEAN;
    Query_result_ptr result = search_collection(collection, query, search_parameter);
    if (size_of_query_result(result) != size){
        printf("Error in query %s result size %d\n", s, size_of_query_result(result));
    }
    free_query(query);
    free_query_result(result);
    free_search_parameter(search_parameter);
}

void test_inverted_index_boolean_query(){
    Parameter_ptr parameter = create_parameter();
    parameter->index_type = INVERTED_INDEX;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection2", parameter);
    test_inverted_matrix_single_query(collection, "Brutus", 2);
    test_inverted_matrix_single_query(collection, "Brutus Caesar", 2);
    test_inverted_matrix_single_query(collection, "enact", 1);
    test_inverted_matrix_single_query(collection, "noble", 1);
    test_inverted_matrix_single_query(collection, "a", 0);
}

void test_positional_index_single_query(Memory_collection_ptr collection, char* s, int size){
    Query_ptr query = create_query(s);
    Search_parameter_ptr search_parameter = create_search_parameter();
    search_parameter->retrieval_type = POSITIONAL;
    Query_result_ptr result = search_collection(collection, query, search_parameter);
    if (size_of_query_result(result) != size){
        printf("Error in query result %s size %d\n", s, size_of_query_result(result));
    }
    free_query(query);
    free_query_result(result);
    free_search_parameter(search_parameter);
}

void test_positional_index_boolean_query(){
    Parameter_ptr parameter = create_parameter();
    parameter->index_type = INVERTED_INDEX;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection2", parameter);
    test_positional_index_single_query(collection, "Julius Caesar", 2);
    test_positional_index_single_query(collection, "I was killed", 1);
    test_positional_index_single_query(collection, "The noble Brutus", 1);
    test_positional_index_single_query(collection, "a", 0);
}

void test_load_indexes_from_file_small(){
    Parameter_ptr parameter = create_parameter();
    parameter->indexes_from_file = true;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection2", parameter);
    if (collection_size(collection) != 2){
        printf("Error in collection size %d\n", collection_size(collection));
    }
    if (vocabulary_size(collection) != 26){
        printf("Error in vocabulary size %d\n", vocabulary_size(collection));
    }
    free_memory_collection(collection);
}

void test_limit_number_of_documents_small(){
    Parameter_ptr parameter = create_parameter();
    parameter->n_gram_index = false;
    parameter->limit_number_of_documents_loaded = true;
    parameter->document_limit = 1;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection2", parameter);
    if (collection_size(collection) != 1){
        printf("Error in collection size %d\n", collection_size(collection));
    }
    if (vocabulary_size(collection) != 15){
        printf("Error in vocabulary size %d\n", vocabulary_size(collection));
    }
    free_memory_collection(collection);
}

void test_categorical_collection(){
    Parameter_ptr parameter = create_parameter();
    parameter->document_type = CATEGORICAL;
    parameter->indexes_from_file = true;
    parameter->phrase_index = false;
    parameter->n_gram_index = false;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection3", parameter);
    if (collection_size(collection) != 1000){
        printf("Error in collection size %d\n", collection_size(collection));
    }
    if (vocabulary_size(collection) != 2283){
        printf("Error in vocabulary size %d\n", vocabulary_size(collection));
    }
    free_memory_collection(collection);
}

void test_attribute_single_query(Memory_collection_ptr collection, char* s, int size) {
    Query_ptr query = create_query(s);
    Search_parameter_ptr search_parameter = create_search_parameter();
    search_parameter->retrieval_type = RANKED;
    search_parameter->documents_retrieved = 400;
    search_parameter->search_attributes = true;
    Query_result_ptr result = search_collection(collection, query, search_parameter);
    if (size_of_query_result(result) != size){
        printf("Error in query %s result size %d\n", s, size_of_query_result(result));
    }
    free_query(query);
    free_query_result(result);
    free_search_parameter(search_parameter);
}

void test_attribute_query(){
    Parameter_ptr parameter = create_parameter();
    parameter->document_type = CATEGORICAL;
    parameter->indexes_from_file = true;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection3", parameter);
    test_attribute_single_query(collection, "Çift yönlü", 10);
    test_attribute_single_query(collection, "Müzikli", 4);
    test_attribute_single_query(collection, "Çift Yönlü Alüminyum Bebek Arabası", 2);
}

void test_categorical_single_query(Memory_collection_ptr collection, char* s, int size) {
    Query_ptr query = create_query(s);
    Search_parameter_ptr search_parameter = create_search_parameter();
    search_parameter->retrieval_type = BOOLEAN;
    search_parameter->focus_type = CATEGORY;
    Query_result_ptr result = search_collection(collection, query, search_parameter);
    if (size_of_query_result(result) != size){
        printf("Error in query %s result size %d\n", s, size_of_query_result(result));
    }
    free_query(query);
    free_query_result(result);
    free_search_parameter(search_parameter);
}

void test_categorical_query(){
    Parameter_ptr parameter = create_parameter();
    parameter->document_type = CATEGORICAL;
    parameter->indexes_from_file = true;
    Memory_collection_ptr collection = create_memory_collection("../../testCollection3", parameter);
    test_categorical_single_query(collection, "Çift Yönlü Bebek Arabası", 10);
    test_categorical_single_query(collection, "Terlik", 5);
}

int main(){
    test_incidence_matrix_small();
    test_incidence_matrix_query();
    test_inverted_index_boolean_query();
    test_positional_index_boolean_query();
    test_load_indexes_from_file_small();
    test_limit_number_of_documents_small();
    test_categorical_collection();
    test_attribute_query();
    test_categorical_query();
}