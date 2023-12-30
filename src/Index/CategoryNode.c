//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#include <Dictionary/Word.h>
#include <string.h>
#include "CategoryNode.h"
#include "Term.h"
#include "TermDictionary.h"
#include "Memory/Memory.h"

Category_node_ptr create_category_node(char *name, Category_node_ptr parent) {
    Category_node_ptr result = malloc_(sizeof(Category_node), "create_category_node");
    result->category_words = str_split(name, ' ');
    result->children = create_array_list();
    result->counts = create_counter_hash_map((unsigned int (*)(const void *, int)) hash_function_int,
                                             (int (*)(const void *, const void *)) compare_int);
    result->parent = parent;
    if (parent != NULL){
        add_child(parent, result);
    }
    return result;
}

void add_child(Category_node_ptr category_node, Category_node_ptr child) {
    array_list_add(category_node->children, child);
}

void free_category_node(Category_node_ptr category_node) {
    free_array_list(category_node->category_words, (void (*)(void *)) free_);
    free_array_list(category_node->children, (void (*)(void *)) free_category_node);
    free_counter_hash_map2(category_node->counts, free_);
    free_(category_node);
}

char *get_name(const Category_node* category_node) {
    unsigned long size = 1 + strlen((char*) array_list_get(category_node->category_words, 0));
    for (int i = 0; i < category_node->category_words->size; i++){
        size += 1 + strlen((char*) array_list_get(category_node->category_words, i));
    }
    char* result = malloc_(size * sizeof(char), "get_name");
    strcpy(result, (char*) array_list_get(category_node->category_words, 0));
    for (int i = 1; i < category_node->category_words->size; i++){
        strcat(result, " ");
        strcat(result, (char*) array_list_get(category_node->category_words, i));
    }
    return result;
}

Category_node_ptr get_child(const Category_node* category_node, const char *name) {
    for (int i = 0; i < category_node->children->size; i++){
        Category_node_ptr child = array_list_get(category_node->children, i);
        char* child_name = get_name(child);
        if (strcmp(child_name, name) == 0){
            free_(child_name);
            return child;
        }
        free_(child_name);
    }
    return NULL;
}

void add_counts(Category_node_ptr category_node, int term_id, int count) {
    Category_node_ptr current = category_node;
    while (current->parent != NULL){
        if (count_counter_hash_map(current->counts, &term_id) > 0){
            put_counter_hash_map_n_times(current->counts, &term_id, count);
        } else {
            int* id = malloc_(sizeof(int), "add_counts");
            *id = term_id;
            put_counter_hash_map_n_times(current->counts, id, count);
        }
        current = current->parent;
    }
}

String_ptr category_node_to_string(const Category_node *category_node) {
    if (category_node->parent != NULL){
        String_ptr s;
        char* name = get_name(category_node);
        if (category_node->parent->parent != NULL){
            s = category_node_to_string(category_node->parent);
            string_append_char(s, '%');
            string_append(s, name);
        } else {
            s = create_string2(name);
        }
        free_(name);
        return s;
    }
    return create_string();
}

bool is_descendant(const Category_node *category_node, const Category_node *ancestor) {
    if (category_node == ancestor){
        return true;
    }
    if (category_node->parent == NULL){
        return false;
    }
    return is_descendant(category_node->parent, ancestor);
}

void set_representative_count_node(Category_node_ptr category_node, int representative_count) {
    if (representative_count <= category_node->counts->map->count){
        Array_list_ptr top_list = top_N_counter_hash_map(category_node->counts, representative_count);
        for (int i = 0; i < top_list->size; i++){
            Hash_node_ptr hash_node = array_list_get(top_list, i);
            put_counter_hash_map_n_times(category_node->counts, hash_node->key, *(int*)hash_node->value);
        }
        free_array_list(top_list, NULL);
    }
}

void get_categories_with_keyword(Category_node_ptr category_node, Query_ptr query, Array_list_ptr result) {
    double category_score = 0;
    for (int i = 0; i < size_of_query(query); i++){
        char* word = array_list_get(query->terms, i);
        int index = array_list_index_of(category_node->category_words, word,
                            (int (*)(const void *, const void *)) compare_string);
        if (index != -1){
            category_score++;
        }
    }
    if (category_score > 0){
        array_list_add(result, category_node);
    }
    for (int i = 0; i < category_node->children->size; i++){
        Category_node_ptr child = array_list_get(category_node->children, i);
        get_categories_with_keyword(child, query, result);
    }
}

void get_categories_with_cosine(Category_node_ptr category_node, Query_ptr query, Dictionary_ptr dictionary,
                                Array_list_ptr result) {
    double category_score = 0;
    for (int i = 0; i < size_of_query(query); i++){
        char* word = array_list_get(query->terms, i);
        Term_ptr term = get_word_term(dictionary, word);
        if (term != NULL){
            category_score += count_counter_hash_map(category_node->counts, &(term->term_id));
        }
    }
    if (category_score > 0){
        array_list_add(result, category_node);
    }
    for (int i = 0; i < category_node->children->size; i++){
        Category_node_ptr child = array_list_get(category_node->children, i);
        get_categories_with_cosine(child, query, dictionary, result);
    }
}
