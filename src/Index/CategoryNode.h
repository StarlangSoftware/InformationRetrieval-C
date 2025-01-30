//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_CATEGORYNODE_H
#define INFORMATIONRETRIEVAL_CATEGORYNODE_H

#include <ArrayList.h>
#include <CounterHashMap.h>
#include <Dictionary/Dictionary.h>
#include "StringUtils.h"
#include "../Query/Query.h"

struct category_node{
    Array_list_ptr children;
    struct category_node* parent;
    Counter_hash_map_ptr counts;
    Array_list_ptr category_words;
};

typedef struct category_node Category_node;

typedef Category_node* Category_node_ptr;

Category_node_ptr create_category_node(const char* name, Category_node_ptr parent);

void free_category_node(Category_node_ptr category_node);

void add_child(Category_node_ptr category_node, Category_node_ptr child);

char* get_name(const Category_node* category_node);

Category_node_ptr get_child(const Category_node* category_node, const char* name);

void add_counts(Category_node_ptr category_node, int term_id, int count);

String_ptr category_node_to_string(const Category_node* category_node);

bool is_descendant(const Category_node* category_node, const Category_node* ancestor);

void set_representative_count_node(Category_node_ptr category_node, int representative_count);

void get_categories_with_keyword(Category_node_ptr category_node, Query_ptr query, Array_list_ptr result);

void get_categories_with_cosine(Category_node_ptr category_node, Query_ptr query, Dictionary_ptr dictionary, Array_list_ptr result);

#endif //INFORMATIONRETRIEVAL_CATEGORYNODE_H
