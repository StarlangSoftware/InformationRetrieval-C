//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#include <stdlib.h>
#include <Dictionary/Word.h>
#include <Memory/Memory.h>
#include "CategoryTree.h"

Category_tree_ptr create_category_tree(char *root_name) {
    Category_tree_ptr result = malloc_(sizeof(Category_tree), "create_category_tree");
    result->root = create_category_node(root_name, NULL);
    return result;
}

Category_node_ptr add_category_hierarchy(Category_tree_ptr category_tree, char *hierarchy) {
    Array_list_ptr categories = split_with_char(hierarchy, "%");
    Category_node_ptr current = category_tree->root;
    for (int i = 0; i < categories->size; i++){
        String_ptr category = array_list_get(categories, i);
        Category_node_ptr node = get_child(current, category->s);
        if (node == NULL){
            node = create_category_node(category->s, current);
        }
        current = node;
    }
    free_array_list(categories, (void (*)(void *)) free_string_ptr);
    return current;
}

void set_representative_count_tree(Category_tree_ptr category_tree, int representative_count) {
    set_representative_count_node(category_tree->root, representative_count);
}

void free_category_tree(Category_tree_ptr category_tree) {
    free_category_node(category_tree->root);
    free_(category_tree);
}

Array_list_ptr get_categories(Category_tree_ptr category_tree, Query_ptr query, Dictionary_ptr dictionary,
                              Category_determination_type category_determination_type) {
    Array_list_ptr result = create_array_list();
    switch (category_determination_type) {
        case KEYWORD:
            get_categories_with_keyword(category_tree->root, query, result);
            break;
        case COSINE:
            get_categories_with_cosine(category_tree->root, query, dictionary, result);
            break;
    }
    return result;
}
