//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#include <stdlib.h>
#include <Dictionary/Word.h>
#include "Query.h"

Query_ptr create_query(char *query) {
    Query_ptr result = malloc(sizeof(Query));
    result->terms = create_array_list();
    Array_list_ptr terms = str_split(query, ' ');
    for (int i = 0; i < terms->size; i++){
        array_list_add(result->terms, create_word(array_list_get(terms, i)));
    }
    return result;
}

Query_ptr create_query2() {
    Query_ptr result = malloc(sizeof(Query));
    result->terms = create_array_list();
    return result;
}

void free_query(Query_ptr query) {
    free_array_list(query->terms, (void (*)(void *)) free_word);
    free(query);
}

Word_ptr get_term(Query_ptr query, int index) {
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
            String_ptr pair = create_string4(((Word_ptr) array_list_get(query->terms, i))->name, " ", ((Word_ptr) array_list_get(query->terms, i + 1))->name);
            if (hash_set_contains(attribute_list, pair->s)){
                array_list_add(phrase_attributes->terms, create_word(pair->s));
                i += 2;
                continue;
            }
            free_string_ptr(pair);
            //regex
        }
        if (hash_set_contains(attribute_list, ((Word_ptr) array_list_get(query->terms, i))->name)){
            array_list_add(term_attributes->terms, create_word(((Word_ptr) array_list_get(query->terms, i))->name));
        } else {
            array_list_add(filtered_query->terms, create_word(((Word_ptr) array_list_get(query->terms, i))->name));
        }
        i++;
    }
    return filtered_query;
}
