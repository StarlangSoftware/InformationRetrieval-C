//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#include <stdlib.h>
#include <Dictionary/Word.h>
#include <string.h>
#include <RegularExpression.h>
#include "Query.h"

Query_ptr create_query(char *query) {
    Query_ptr result = malloc(sizeof(Query));
    result->terms = create_array_list();
    Array_list_ptr terms = str_split(query, ' ');
    for (int i = 0; i < terms->size; i++){
        array_list_add(result->terms, array_list_get(terms, i));
    }
    free_array_list(terms, NULL);
    return result;
}

Query_ptr create_query2() {
    Query_ptr result = malloc(sizeof(Query));
    result->terms = create_array_list();
    return result;
}

void free_query(Query_ptr query) {
    free_array_list(query->terms, (void (*)(void *)) free);
    free(query);
}

char* get_term(Query_ptr query, int index) {
    return array_list_get(query->terms, index);
}

int size_of_query(Query_ptr query) {
    return query->terms->size;
}

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
