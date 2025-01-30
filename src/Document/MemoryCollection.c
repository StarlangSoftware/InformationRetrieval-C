//
// Created by Olcay Taner YILDIZ on 5.09.2023.
//

#include <FileUtils.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <Memory/Memory.h>
#include "MemoryCollection.h"
#include "Document.h"
#include "../Index/TermDictionary.h"
#include "../Index/TermOccurrence.h"
#include "DocumentText.h"

/**
 * Loads the attribute list from attribute index file. Attributes are single or bi-word phrases representing the
 * important features of products in the collection. Each line of the attribute file contains either single or a two
 * word expression.
 */
void load_attribute_list(Memory_collection_ptr memory_collection) {
    char name[MAX_LINE_LENGTH];
    sprintf(name, "%s-attributelist.txt", memory_collection->name);
    memory_collection->attribute_list = read_hash_set(name);
}

/**
 * Returns size of the document collection.
 * @return Size of the document collection.
 */
int collection_size(const Memory_collection* memory_collection) {
    return memory_collection->documents->size;
}

/**
 * Returns size of the term dictionary.
 * @return Size of the term dictionary.
 */
int vocabulary_size(const Memory_collection* memory_collection) {
    return memory_collection->dictionary->words->size;
}

/**
 * Loads the category tree for the categorical collections from category index file. Each line of the category index
 * file stores the index of the category and the category name with its hierarchy. Hierarchy string is obtained by
 * concatenating the names of all nodes in the path from root node to a leaf node separated with '%'.
 */
void load_categories(Memory_collection_ptr memory_collection) {
    char name[MAX_LINE_LENGTH];
    memory_collection->category_tree = create_category_tree(memory_collection->name);
    sprintf(name, "%s-categories.txt", memory_collection->name);
    Array_list_ptr lines = read_lines(name);
    for (int i = 0; i < lines->size; i++){
        char* line = array_list_get(lines, i);
        if (strlen(line) != 0){
            Array_list_ptr items = str_split(line, '\t');
            int doc_id = atoi(array_list_get(items, 0));
            if (items->size > 1){
                Document_ptr document = array_list_get(memory_collection->documents, doc_id);
                set_category(document, memory_collection->category_tree, array_list_get(items, 1));
            }
            free_array_list(items, free_);
        }
    }
    free_array_list(lines, free_);
}

/**
 * Constructs bi-gram and tri-gram indexes in memory.
 */
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

/**
 * Constructor for the MemoryCollection class. In small collections, dictionary and indexes are kept in memory.
 * Memory collection also supports categorical documents.
 * @param directory_name Directory where the document collection resides.
 * @param parameter Search parameter
 */
Memory_collection_ptr create_memory_collection(const char *directory_name, Parameter_ptr parameter) {
    Memory_collection_ptr result = malloc_(sizeof(Memory_collection), "create_memory_collection");
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

void read_documents(Memory_collection_ptr memory_collection, const char *directory_name, const Parameter* parameter) {
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
    free_term_dictionary(memory_collection->dictionary);
    if (memory_collection->index_type != INCIDENCE_MATRIX){
        free_inverted_index(memory_collection->inverted_index);
        if (memory_collection->positional_index != NULL){
            free_positional_index(memory_collection->positional_index);
        }
        if (memory_collection->phrase_dictionary != NULL){
            free_term_dictionary(memory_collection->phrase_dictionary);
            free_inverted_index(memory_collection->phrase_index);
        }
        if (memory_collection->phrase_positional_index != NULL){
            free_positional_index(memory_collection->phrase_positional_index);
        }
        if (memory_collection->bi_gram_dictionary != NULL){
            free_term_dictionary(memory_collection->bi_gram_dictionary);
            free_term_dictionary(memory_collection->tri_gram_dictionary);
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
    free_hash_set(memory_collection->attribute_list, free_);
    free_(memory_collection->name);
    free_(memory_collection);
}

/**
 * The method loads the term dictionary, inverted index, positional index, phrase and N-Gram indexes from dictionary
 * and index files to the memory.
 * @param memory_collection Collection object
 * @param directory Directory where the document collection resides.
 */
void load_indexes_from_file(Memory_collection_ptr memory_collection, const char *directory) {
    memory_collection->dictionary = create_term_dictionary2(directory);
    memory_collection->inverted_index = create_inverted_index(directory);
    if (memory_collection->parameter->positional_index){
        memory_collection->positional_index = create_positional_index(directory);
        int* sizes = get_document_sizes(memory_collection->positional_index, memory_collection->documents->size);
        for (int i = 0; i < memory_collection->documents->size; i++){
            Document_ptr doc = array_list_get(memory_collection->documents, i);
            doc->size = sizes[doc->doc_id];
        }
        free_(sizes);
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

/**
 * Given the document collection, creates an array list of terms. If term type is TOKEN, the terms are single
 * word, if the term type is PHRASE, the terms are bi-words. Each document is loaded into memory and
 * word list is created. Since the dictionary can be kept in memory, all operations can be done in memory.
 * @param memory_collection Collection object
 * @param term_type If term type is TOKEN, the terms are single word, if the term type is PHRASE, the terms are
 *                 bi-words.
 * @return Array list of terms occurring in the document collection.
 */
Array_list_ptr construct_terms(const Memory_collection* memory_collection, Term_type term_type) {
    Array_list_ptr terms = create_array_list();
    Array_list_ptr doc_terms;
    for (int i = 0; i < memory_collection->documents->size; i++){
        Document_ptr doc = array_list_get(memory_collection->documents, i);
        Corpus_ptr document_txt = load_document(doc);
        doc_terms = construct_term_list(document_txt, doc->doc_id, term_type);
        array_list_add_all(terms, doc_terms);
        free_array_list(doc_terms, NULL);
        free_(document_txt->file_name);
        free_array_list(document_txt->sentences, NULL);
        free_array_list(document_txt->paragraphs, (void (*)(void *)) free_paragraph);
        free_counter_hash_map(document_txt->word_list);
        free_(document_txt);
    }
    array_list_sort(terms, (int (*)(const void *, const void *)) compare_term_occurrence);
    return terms;
}

/**
 * The method constructs the term dictionary, inverted index, positional index, phrase and N-Gram indexes in memory.
 * @param memory_collection Collection object
 */
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
    free_array_list(terms, (void (*)(void *)) free_term_occurrence);
}

/**
 * The method saves the category tree for the categorical collections.
 * @param memory_collection Collection object
 */
void save_categories(const Memory_collection* memory_collection) {
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

/**
 * The method saves the term dictionary, inverted index, positional index, phrase and N-Gram indexes to the dictionary
 * and index files. If the collection is a categorical collection, categories are also saved to the category
 * files.
 * @param memory_collection Collection object
 */
void save_memory_collection(const Memory_collection* memory_collection) {
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

/**
 * Filters current search result according to the predicted categories from the query string. For every search
 * result, if it is in one of the predicated categories, is added to the filtered end result. Otherwise, it is
 * omitted in the end result.
 * @param memory_collection Collection object
 * @param current_result Current search result before filtering.
 * @param categories Predicted categories that match the query string.
 * @return Filtered query result
 */
Query_result_ptr
filter_according_to_categories(const Memory_collection* memory_collection, const Query_result* current_result,
                               const Array_list* categories) {
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

/**
 * The method searches given query string in the document collection using the inverted index according to the
 * given search parameter. If the search is (i) boolean, inverted index is used (ii) positional, positional
 * inverted index is used, (iii) ranked, positional inverted index is used with a ranking algorithm at the end.
 * @param memory_collection Collection object
 * @param query Query string
 * @param parameter Search parameter for the query
 * @return The intermediate result of the query obtained by doing inverted index based search in the collection.
 */
Query_result_ptr
search_with_inverted_index(const Memory_collection* memory_collection, Query_ptr query, const Search_parameter* parameter) {
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

/**
 * The method searches given query string in the document collection using the attribute list according to the
 * given search parameter. First, the original query is filtered by removing phrase attributes, shortcuts and single
 * word attributes. At this stage, we get the word and phrase attributes in the original query and the remaining
 * words in the original query as two separate queries. Second, both single word and phrase attributes in the
 * original query are searched in the document collection. Third, these intermediate query results are then
 * intersected. Fourth, we put this results into either (i) an inverted index (ii) or a ranked based positional
 * filtering with the filtered query to get the end result.
 * @param memory_collection Collection object
 * @param query Query string
 * @param parameter Search parameter for the query
 * @return The intermediate result of the query obtained by doing attribute list based search in the collection.
 */
Query_result_ptr
attribute_search(const Memory_collection* memory_collection, Query_ptr query, const Search_parameter* parameter) {
    Query_ptr term_attributes = create_query2();
    Query_ptr phrase_attributes = create_query2();
    Query_result_ptr term_result = NULL;
    Query_result_ptr phrase_result = NULL;
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
            free_query_result(term_result);
            free_query_result(phrase_result);
        }
    }
    free_query(term_attributes);
    free_query(phrase_attributes);
    if (size_of_query(filtered_query) == 0){
        free_query(filtered_query);
        return attribute_result;
    } else {
        if (parameter->retrieval_type != RANKED){
            filtered_result = search_with_inverted_index(memory_collection, filtered_query, parameter);
            free_query(filtered_query);
            Query_result_ptr result = intersection_fast_search(filtered_result, attribute_result);
            free_query_result(filtered_result);
            free_query_result(attribute_result);
            return result;
        } else {
            filtered_result = ranked_search(memory_collection->positional_index,
                                            filtered_query,
                                            memory_collection->dictionary,memory_collection->documents,
                                            parameter);
            free_query(filtered_query);
            Query_result_ptr tmp_result;
            if (size_of_query_result(attribute_result) < 10){
                tmp_result = intersection_linear_search(filtered_result, attribute_result);
            } else {
                tmp_result = intersection_binary_search(filtered_result, attribute_result);
            }
            free_query_result(filtered_result);
            free_query_result(attribute_result);
            get_best(tmp_result, parameter->documents_retrieved);
            return tmp_result;
        }
    }
}

/**
 * Searches a document collection for a given query according to the given search parameters. The documents are
 * searched using (i) incidence matrix if the index type is incidence matrix, (ii) attribute list if search
 * attributes option is selected, (iii) inverted index if the index type is inverted index and no attribute
 * search is done. After the initial search, if there is a categorical focus, it filters the results
 * according to the predicted categories from the query string.
 * @param memory_collection Collection object
 * @param query Query string
 * @param search_parameter Search parameter for the query
 * @return The result of the query obtained by doing search in the collection.
 */
Query_result_ptr
search_collection(const Memory_collection* memory_collection, Query_ptr query, const Search_parameter* search_parameter) {
    Query_result_ptr current_result;
    if (search_parameter->focus_type == CATEGORY){
        if (search_parameter->search_attributes){
            current_result = attribute_search(memory_collection, query, search_parameter);
        } else {
            current_result = search_with_inverted_index(memory_collection, query, search_parameter);
        }
        Array_list_ptr categories = get_categories(memory_collection->category_tree, query, memory_collection->dictionary, search_parameter->category_determination_type);
        Query_result_ptr result = filter_according_to_categories(memory_collection, current_result, categories);
        free_array_list(categories, NULL);
        free_query_result(current_result);
        return result;
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
