//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#include <Dictionary/Word.h>
#include <string.h>
#include <RegularExpression.h>
#include <Memory/Memory.h>
#include "Query.h"

/**
 * Another constructor of the Query class. Splits the query into multiple words and put them into the terms array.
 * @param query Query string
 */
Query_ptr create_query(char *query) {
    Query_ptr result = malloc_(sizeof(Query), "create_query");
    result->terms = create_array_list();
    Array_list_ptr terms = str_split(query, ' ');
    for (int i = 0; i < terms->size; i++){
        array_list_add(result->terms, array_list_get(terms, i));
    }
    free_array_list(terms, NULL);
    return result;
}

/**
 * Constructor of the Query class. Initializes the terms array.
 */
Query_ptr create_query2() {
    Query_ptr result = malloc_(sizeof(Query), "create_query2");
    result->terms = create_array_list();
    return result;
}

void free_query(Query_ptr query) {
    free_array_list(query->terms, (void (*)(void *)) free_);
    free_(query);
}

/**
 * Accessor for the terms array. Returns the term at position index.
 * @param query Current query object.
 * @param index Position of the term in the terms array.
 * @return The term at position index.
 */
char* get_term(Query_ptr query, int index) {
    return array_list_get(query->terms, index);
}

/**
 * Returns the size of the query, i.e. number of words in the query.
 * @param query Current query object.
 * @return Size of the query, i.e. number of words in the query.
 */
int size_of_query(Query_ptr query) {
    return query->terms->size;
}

/**
 * Filters the original query by removing phrase attributes, shortcuts and single word attributes.
 * @param query Current query object.
 * @param attribute_list Hash set containing all attributes (phrase and single word)
 * @param term_attributes New query that will accumulate single word attributes from the original query.
 * @param phrase_attributes New query that will accumulate phrase attributes from the original query.
 * @return Filtered query after removing single word and phrase attributes from the original query.
 */
Query_ptr filter_attributes(Query_ptr query,
                            Hash_set_ptr attribute_list,
                            Query_ptr term_attributes,
                            Query_ptr phrase_attributes) {
    int i = 0;
    Query_ptr filtered_query = create_query2();
    while (i < query->terms->size){
        if (i < query->terms->size - 1){
            String_ptr pair = create_string4(array_list_get(query->terms, i), " ", array_list_get(query->terms, i + 1));
            if (hash_set_contains(attribute_list, pair->s)){
                char* tmp = str_copy(tmp, pair->s);
                array_list_add(phrase_attributes->terms, tmp);
                i += 2;
                free_string_ptr(pair);
                continue;
            }
            free_string_ptr(pair);
            bool found = false;
            for (int j = 0; j < 6; j++){
                if (strcmp(shortcuts[j], array_list_get(query->terms, i + 1)) == 0){
                    found = true;
                    break;
                }
            }
            if (found){
                Regular_expression_ptr re1 = create_regular_expression("[\\-\\+]?\\d+");
                Regular_expression_ptr re2 = create_regular_expression("[\\+\\-]?\\d*\\.\\d*");
                if (full_matches(re1, array_list_get(query->terms, i)) || full_matches(re2, array_list_get(query->terms, i))){
                    char* tmp = str_copy(tmp, pair->s);
                    array_list_add(phrase_attributes->terms, tmp);
                    i += 2;
                    continue;
                }
                free_regular_expression(re1);
                free_regular_expression(re2);
            }
        }
        char* tmp = str_copy(tmp, array_list_get(query->terms, i));
        if (hash_set_contains(attribute_list, tmp)){
            array_list_add(term_attributes->terms, tmp);
        } else {
            array_list_add(filtered_query->terms, tmp);
        }
        i++;
    }
    return filtered_query;
}
