//
// Created by Olcay Taner YILDIZ on 5.09.2023.
//

#include <FileUtils.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "MemoryCollection.h"
#include "Document.h"
#include "../Index/TermDictionary.h"
#include "../Index/TermOccurrence.h"
#include "DocumentText.h"

void load_attribute_list(Memory_collection_ptr memory_collection) {
    char name[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    memory_collection->attribute_list = create_hash_set((unsigned int (*)(const void *, int)) hash_function_string,
                                                        (int (*)(const void *, const void *)) compare_string);
    sprintf(name, "%s-attributelist.txt", memory_collection->name);
    FILE* input_file = fopen(name, "r");
    if (input_file == NULL){
        return;
    }
    char* input = fgets(line, MAX_LINE_LENGTH, input_file);
    while (input != NULL){
        if (strlen(line) != 0){
            line[strcspn(line, "\n")] = 0;
            char* attribute = str_copy(attribute, line);
            hash_set_insert(memory_collection->attribute_list, attribute);
        }
        input = fgets(line, MAX_LINE_LENGTH, input_file);
    }
    fclose(input_file);
}

int collection_size(Memory_collection_ptr memory_collection) {
    return memory_collection->documents->size;
}

int vocabulary_size(Memory_collection_ptr memory_collection) {
    return memory_collection->dictionary->words->size;
}

void load_categories(Memory_collection_ptr memory_collection) {
    char name[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    memory_collection->category_tree = create_category_tree(memory_collection->name);
    sprintf(name, "%s-categories.txt", memory_collection->name);
    FILE* input_file = fopen(name, "r");
    char* input = fgets(line, MAX_LINE_LENGTH, input_file);
    while (input != NULL){
        if (strlen(line) != 0){
            line[strcspn(line, "\n")] = 0;
            Array_list_ptr items = str_split(line, '\t');
            int doc_id = atoi(array_list_get(items, 0));
            if (items->size > 1){
                Document_ptr document = array_list_get(memory_collection->documents, doc_id);
                set_category(document, memory_collection->category_tree, array_list_get(items, 1));
            }
            free_array_list(items, free);
        }
        input = fgets(line, MAX_LINE_LENGTH, input_file);
    }
    fclose(input_file);
}

void construct_n_gram_index(Memory_collection_ptr memory_collection) {
    Array_list_ptr terms = construct_terms_from_dictionary(memory_collection->dictionary, 2);
    memory_collection->bi_gram_dictionary = create_term_dictionary3(terms);
    memory_collection->bi_gram_index = create_inverted_index2(memory_collection->bi_gram_dictionary, terms);
    free_array_list(terms, (void (*)(void *)) free_term_occurrence);
    terms = construct_terms_from_dictionary(memory_collection->dictionary, 3);
    memory_collection->tri_gram_dictionary = create_term_dictionary3(terms);
    memory_collection->tri_gram_index = create_inverted_index2(memory_collection->tri_gram_dictionary, terms);
    free_array_list(terms, (void (*)(void *)) free_term_occurrence);
}

Memory_collection_ptr create_memory_collection(char *directory_name, Parameter_ptr parameter) {
    Memory_collection_ptr result = malloc(sizeof(Memory_collection));
    result->parameter = parameter;
    result->name = str_copy(result->name, directory_name);
    load_attribute_list(result);
    read_documents(result, directory_name, parameter);
    if (parameter->document_type == CATEGORICAL){
        load_categories(result);
    } else {
        result->category_tree = NULL;
    }
    result->index_type = parameter->index_type;
    if (parameter->indexes_from_file){
        load_indexes_from_file(result, directory_name);
    } else {
        construct_indexes_in_memory(result);
    }
    if (parameter->document_type == CATEGORICAL){
        set_category_counts(result->positional_index, result->documents);
        set_representative_count_tree(result->category_tree, parameter->representative_count);
    }
    return result;
}

void read_documents(Memory_collection_ptr memory_collection, char *directory_name, Parameter_ptr parameter) {
    int j = 0;
    DIR *directory = opendir(directory_name);
    struct dirent *directory_entry;
    memory_collection->documents = create_array_list();
    directory_entry = readdir(directory);
    while (directory_entry != NULL){
        if (ends_with(directory_entry->d_name, ".txt")){
            if (!parameter->limit_number_of_documents_loaded || j < parameter->document_limit){
                char name[MAX_LINE_LENGTH];
                sprintf(name, "%s/%s", directory_name, directory_entry->d_name);
                Document_ptr document = create_document(parameter->document_type,
                                                        name,
                                                        directory_entry->d_name, j);
                array_list_add(memory_collection->documents, document);
                j++;
            }
        }
        directory_entry = readdir(directory);
    }
    closedir(directory);
}

void free_memory_collection(Memory_collection_ptr memory_collection) {
    free_dictionary(memory_collection->dictionary);
    if (memory_collection->index_type != INCIDENCE_MATRIX){
        free_inverted_index(memory_collection->inverted_index);
        if (memory_collection->positional_index != NULL){
            free_positional_index(memory_collection->positional_index);
        }
        if (memory_collection->phrase_dictionary != NULL){
            free_dictionary(memory_collection->phrase_dictionary);
            free_inverted_index(memory_collection->phrase_index);
        }
        if (memory_collection->phrase_positional_index != NULL){
            free_positional_index(memory_collection->phrase_positional_index);
        }
        if (memory_collection->bi_gram_dictionary != NULL){
            free_dictionary(memory_collection->bi_gram_dictionary);
            free_dictionary(memory_collection->tri_gram_dictionary);
            free_inverted_index(memory_collection->bi_gram_index);
            free_inverted_index(memory_collection->tri_gram_index);
        }
    } else {
        free_incidence_matrix(memory_collection->incidence_matrix);
    }
    if (memory_collection->category_tree != NULL){
        free_category_tree(memory_collection->category_tree);
    }
    free_parameter(memory_collection->parameter);
    free_array_list(memory_collection->documents, (void (*)(void *)) free_document);
    free_hash_set(memory_collection->attribute_list, free);
    free(memory_collection);
}

void load_indexes_from_file(Memory_collection_ptr memory_collection, char *directory) {
    memory_collection->dictionary = create_term_dictionary2(directory);
    memory_collection->inverted_index = create_inverted_index(directory);
    if (memory_collection->parameter->positional_index){
        memory_collection->positional_index = create_positional_index(directory);
        int* sizes = get_document_sizes(memory_collection->positional_index, memory_collection->documents->size);
        for (int i = 0; i < memory_collection->documents->size; i++){
            Document_ptr doc = array_list_get(memory_collection->documents, i);
            doc->size = sizes[doc->doc_id];
        }
        free(sizes);
    } else {
        memory_collection->positional_index = NULL;
    }
    if (memory_collection->parameter->phrase_index){
        char name[MAX_LINE_LENGTH];
        sprintf(name, "%s-phrase", directory);
        memory_collection->phrase_dictionary = create_term_dictionary2(name);
        memory_collection->phrase_index = create_inverted_index(name);
        if (memory_collection->parameter->positional_index){
            memory_collection->phrase_positional_index = create_positional_index(name);
        } else {
            memory_collection->phrase_positional_index = NULL;
        }
    } else {
        memory_collection->phrase_dictionary = NULL;
        memory_collection->phrase_index = NULL;
        memory_collection->phrase_positional_index = NULL;
    }
    if (memory_collection->parameter->n_gram_index){
        char name[MAX_LINE_LENGTH];
        sprintf(name, "%s-biGram", directory);
        memory_collection->bi_gram_dictionary = create_term_dictionary2(name);
        memory_collection->bi_gram_index = create_inverted_index(name);
        sprintf(name, "%s-triGram", directory);
        memory_collection->tri_gram_dictionary = create_term_dictionary2(name);
        memory_collection->tri_gram_index = create_inverted_index(name);
    } else {
        memory_collection->bi_gram_dictionary = NULL;
        memory_collection->bi_gram_index = NULL;
        memory_collection->tri_gram_dictionary = NULL;
        memory_collection->tri_gram_index = NULL;
    }
}

Array_list_ptr construct_terms(Memory_collection_ptr memory_collection, Term_type term_type) {
    Array_list_ptr terms = create_array_list();
    Array_list_ptr doc_terms;
    for (int i = 0; i < memory_collection->documents->size; i++){
        Document_ptr doc = array_list_get(memory_collection->documents, i);
        Corpus_ptr document_txt = load_document(doc);
        doc_terms = construct_term_list(document_txt, doc->doc_id, term_type);
        array_list_add_all(terms, doc_terms);
        free_array_list(doc_terms, NULL);
    }
    array_list_sort(terms, (int (*)(const void *, const void *)) compare_term_occurrence2);
    return terms;
}

void construct_indexes_in_memory(Memory_collection_ptr memory_collection) {
    Array_list_ptr terms = construct_terms(memory_collection, TOKEN);
    memory_collection->dictionary = create_term_dictionary3(terms);
    switch (memory_collection->index_type) {
        case INCIDENCE_MATRIX:
            memory_collection->incidence_matrix = create_incidence_matrix2(terms, memory_collection->dictionary, memory_collection->documents->size);
            break;
        case INVERTED_INDEX:
            memory_collection->inverted_index = create_inverted_index2(memory_collection->dictionary, terms);
            if (memory_collection->parameter->positional_index){
                memory_collection->positional_index = create_positional_index2(memory_collection->dictionary, terms);
            } else {
                memory_collection->positional_index = NULL;
            }
            free_array_list(terms, (void (*)(void *)) free_term_occurrence);
            if (memory_collection->parameter->phrase_index){
                terms = construct_terms(memory_collection, PHRASE);
                memory_collection->phrase_dictionary = create_term_dictionary3(terms);
                memory_collection->phrase_index = create_inverted_index2(memory_collection->phrase_dictionary, terms);
                if (memory_collection->parameter->positional_index){
                    memory_collection->phrase_positional_index = create_positional_index2(memory_collection->phrase_dictionary, terms);
                } else {
                    memory_collection->phrase_positional_index = NULL;
                }
            } else {
                memory_collection->phrase_dictionary = NULL;
                memory_collection->phrase_index = NULL;
                memory_collection->phrase_positional_index = NULL;
            }
            if (memory_collection->parameter->n_gram_index){
                construct_n_gram_index(memory_collection);
            } else {
                memory_collection->bi_gram_dictionary = NULL;
                memory_collection->bi_gram_index = NULL;
                memory_collection->tri_gram_dictionary = NULL;
                memory_collection->tri_gram_index = NULL;
            }
            if (memory_collection->parameter->document_type == CATEGORICAL){
                memory_collection->category_tree = create_category_tree(memory_collection->name);
                for (int i = 0; i < memory_collection->documents->size; i++){
                    Document_ptr doc = array_list_get(memory_collection->documents, i);
                    load_category(doc, memory_collection->category_tree);
                }
            } else {
                memory_collection->category_tree = NULL;
            }
            break;
    }
}

void save_categories(Memory_collection_ptr memory_collection) {
    FILE *output_file;
    char name[MAX_LINE_LENGTH];
    sprintf(name, "%s-categories.txt", memory_collection->name);
    output_file = fopen(name, "w");
    for (int i = 0; i < memory_collection->documents->size; i++){
        Document_ptr document = array_list_get(memory_collection->documents, i);
        String_ptr st = category_node_to_string(document->category);
        fprintf(output_file, "%d\t%s\n", document->doc_id, st->s);
        free_string_ptr(st);
    }
    fclose(output_file);
}

void save_memory_collection(Memory_collection_ptr memory_collection) {
    if (memory_collection->index_type == INVERTED_INDEX){
        save_term_dictionary(memory_collection->dictionary, memory_collection->name);
        save_inverted_index(memory_collection->inverted_index, memory_collection->name);
        if (memory_collection->parameter->positional_index){
            save_positional_index(memory_collection->positional_index, memory_collection->name);
        }
        if (memory_collection->parameter->phrase_index){
            char name[MAX_LINE_LENGTH];
            sprintf(name, "%s-phrase", memory_collection->name);
            save_term_dictionary(memory_collection->phrase_dictionary, name);
            save_inverted_index(memory_collection->phrase_index, name);
            if (memory_collection->parameter->positional_index){
                save_positional_index(memory_collection->phrase_positional_index, name);
            }
        }
        if (memory_collection->parameter->n_gram_index){
            char name[MAX_LINE_LENGTH];
            sprintf(name, "%s-biGram", memory_collection->name);
            save_term_dictionary(memory_collection->bi_gram_dictionary, name);
            save_inverted_index(memory_collection->bi_gram_index, name);
            sprintf(name, "%s-triGram", memory_collection->name);
            save_term_dictionary(memory_collection->tri_gram_dictionary, name);
            save_inverted_index(memory_collection->tri_gram_index, name);
        }
    }
    if (memory_collection->parameter->document_type == CATEGORICAL){
        save_categories(memory_collection);
    }
}

Query_result_ptr
filter_according_to_categories(Memory_collection_ptr memory_collection, Query_result_ptr current_result,
                               Array_list_ptr categories) {
    Query_result_ptr filtered_result = create_query_result();
    Array_list_ptr items = current_result->items;
    for (int i = 0; i < items->size; i++){
        Query_result_item_ptr query_result_item = array_list_get(items, i);
        Document_ptr doc = array_list_get(memory_collection->documents, query_result_item->doc_id);
        Category_node_ptr category_node = doc->category;
        for (int j = 0; j < categories->size; j++){
            Category_node_ptr possible_ancestor = array_list_get(categories, j);
            if (is_descendant(category_node, possible_ancestor)){
                add_to_query_result(filtered_result, query_result_item->doc_id, query_result_item->score);
                break;
            }
        }
    }
    return filtered_result;
}

Query_result_ptr
search_with_inverted_index(Memory_collection_ptr memory_collection, Query_ptr query, Search_parameter_ptr parameter) {
    Query_result_ptr result;
    switch (parameter->retrieval_type) {
        case BOOLEAN:
            return search_inverted_index(memory_collection->inverted_index, query, memory_collection->dictionary);
        case POSITIONAL:
            return positional_search(memory_collection->positional_index, query, memory_collection->dictionary);
        case RANKED:
            result = ranked_search(memory_collection->positional_index,
                                   query,
                                   memory_collection->dictionary,
                                   memory_collection->documents,
                                   parameter);
            get_best(result, parameter->documents_retrieved);
            return result;
    }
    return create_query_result();
}

Query_result_ptr
attribute_search(Memory_collection_ptr memory_collection, Query_ptr query, Search_parameter_ptr parameter) {
    Query_ptr term_attributes = create_query2();
    Query_ptr phrase_attributes = create_query2();
    Query_result_ptr term_result = create_query_result();
    Query_result_ptr phrase_result = create_query_result();
    Query_result_ptr attribute_result, filtered_result;
    Query_ptr filtered_query = filter_attributes(query, memory_collection->attribute_list, term_attributes, phrase_attributes);
    if (size_of_query(term_attributes) > 0){
        term_result = search_inverted_index(memory_collection->inverted_index, term_attributes, memory_collection->dictionary);
    }
    if (size_of_query(phrase_attributes) > 0){
        phrase_result = search_inverted_index(memory_collection->phrase_index, phrase_attributes, memory_collection->phrase_dictionary);
    }
    if (size_of_query(term_attributes) == 0){
        attribute_result = phrase_result;
    } else {
        if (size_of_query(phrase_attributes) == 0){
            attribute_result = term_result;
        } else {
            attribute_result = intersection_fast_search(term_result, phrase_result);
        }
    }
    if (size_of_query(filtered_query) == 0){
        return attribute_result;
    } else {
        if (parameter->retrieval_type != RANKED){
            filtered_result = search_with_inverted_index(memory_collection, filtered_query, parameter);
            return intersection_fast_search(filtered_result, attribute_result);
        } else {
            filtered_result = ranked_search(memory_collection->positional_index,
                                            filtered_query,
                                            memory_collection->dictionary,memory_collection->documents,
                                            parameter);
            Query_result_ptr tmp_result;
            if (size_of_query_result(attribute_result) < 10){
                tmp_result = intersection_linear_search(filtered_result, attribute_result);
            } else {
                tmp_result = intersection_binary_search(filtered_result, attribute_result);
            }
            free_query_result(filtered_result);
            get_best(tmp_result, parameter->documents_retrieved);
            return tmp_result;
        }
    }
}

Query_result_ptr
search_collection(Memory_collection_ptr memory_collection, Query_ptr query, Search_parameter_ptr search_parameter) {
    Query_result_ptr current_result;
    if (search_parameter->focus_type == CATEGORY){
        if (search_parameter->search_attributes){
            current_result = attribute_search(memory_collection, query, search_parameter);
        } else {
            current_result = search_with_inverted_index(memory_collection, query, search_parameter);
        }
        Array_list_ptr categories = get_categories(memory_collection->category_tree, query, memory_collection->dictionary, search_parameter->category_determination_type);
        return filter_according_to_categories(memory_collection, current_result, categories);
    } else {
        switch (memory_collection->index_type) {
            case INCIDENCE_MATRIX:
                return search_incidence_matrix(memory_collection->incidence_matrix, query, memory_collection->dictionary);
            case INVERTED_INDEX:
                if (search_parameter->search_attributes){
                    return attribute_search(memory_collection, query, search_parameter);
                } else {
                    return search_with_inverted_index(memory_collection, query, search_parameter);
                }
        }
    }
    return create_query_result();
}
