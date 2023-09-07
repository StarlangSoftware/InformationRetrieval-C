//
// Created by Olcay Taner YILDIZ on 27.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_CATEGORYTREE_H
#define INFORMATIONRETRIEVAL_CATEGORYTREE_H

#include <Dictionary/Dictionary.h>
#include "CategoryNode.h"
#include "../Query/Query.h"
#include "../Query/CategoryDeterminationType.h"

struct category_tree{
    Category_node_ptr root;
};

typedef struct category_tree Category_tree;

typedef Category_tree* Category_tree_ptr;

Category_tree_ptr create_category_tree(char* root_name);

void free_category_tree(Category_tree_ptr category_tree);

Category_node_ptr add_category_hierarchy(Category_tree_ptr category_tree, char* hierarchy);

void set_representative_count_tree(Category_tree_ptr category_tree, int representative_count);

Array_list_ptr get_categories(Category_tree_ptr category_tree,
                              Query_ptr query,
                              Dictionary_ptr dictionary,
                              Category_determination_type category_determination_type);

#endif //INFORMATIONRETRIEVAL_CATEGORYTREE_H
