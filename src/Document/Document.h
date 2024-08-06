//
// Created by Olcay Taner YILDIZ on 3.09.2023.
//

#ifndef INFORMATIONRETRIEVAL_DOCUMENT_H
#define INFORMATIONRETRIEVAL_DOCUMENT_H

#include "DocumentType.h"
#include "../Index/CategoryNode.h"
#include "../Index/CategoryTree.h"
#include "Corpus.h"

struct document{
    char* absolute_file_name;
    char* file_name;
    int doc_id;
    int size;
    Document_type document_type;
    Category_node_ptr category;
};

typedef struct document Document;

typedef Document* Document_ptr;

Document_ptr create_document(Document_type document_type,
                             const char* absolute_file_name,
                             const char* file_name,
                             int doc_id);

void free_document(Document_ptr document);

void load_category(Document_ptr document, Category_tree_ptr category_tree);

void set_category(Document_ptr document, Category_tree_ptr category_tree, char* category);

Corpus_ptr load_document(Document_ptr document);

#endif //INFORMATIONRETRIEVAL_DOCUMENT_H
