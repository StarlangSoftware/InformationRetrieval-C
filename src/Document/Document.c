//
// Created by Olcay Taner YILDIZ on 3.09.2023.
//

#include <Corpus.h>
#include <TurkishSplitter.h>
#include <Memory/Memory.h>
#include "Document.h"

/**
 * Constructor for the Document class. Sets the attributes.
 * @param document_type Type of the document. Can be normal for normal documents, categorical for categorical
 *                     documents.
 * @param absolute_file_name Absolute file name of the document
 * @param file_name Relative file name of the document.
 * @param doc_id Id of the document
 */
Document_ptr create_document(Document_type document_type, const char *absolute_file_name, const char *file_name, int doc_id) {
    Document_ptr result = malloc_(sizeof(Document), "create_document");
    result->doc_id = doc_id;
    result->absolute_file_name = str_copy(result->absolute_file_name, absolute_file_name);
    result->file_name = str_copy(result->file_name, file_name);
    result->document_type = document_type;
    return result;
}

/**
 * Loads the category of the document and adds it to the category tree. Category information is stored in the first
 * line of the document.
 * @param document Current document
 * @param category_tree Category tree to which new product will be added.
 */
void load_category(Document_ptr document, Category_tree_ptr category_tree) {
    if (document->document_type == CATEGORICAL){
        Corpus_ptr corpus = create_corpus2(document->absolute_file_name);
        if (corpus_sentence_count(corpus) >= 2){
            Sentence_ptr sentence = corpus_get_sentence(corpus, 0);
            String_ptr s = to_words(sentence);
            document->category = add_category_hierarchy(category_tree, s->s);
            free_string_ptr(s);
        }
        free_corpus(corpus);
    }
}

/**
 * Mutator for the category attribute.
 * @param document Current document
 * @param category_tree Category tree to which new category will be added.
 * @param category New category that will be added
 */
void set_category(Document_ptr document, Category_tree_ptr category_tree, char *category) {
    document->category = add_category_hierarchy(category_tree, category);
}

/**
 * Loads the document from input stream. For normal documents, it reads as a corpus. For categorical documents, the
 * first line contains categorical information, second line contains name of the product, third line contains
 * detailed info about the product.
 * @param document Current document
 * @return Loaded document text.
 */
Corpus_ptr load_document(Document_ptr document) {
    Corpus_ptr document_text = NULL;
    Corpus_ptr corpus;
    switch (document->document_type) {
        case NORMAL:
            document_text = create_corpus4(document->absolute_file_name, sentence_split_turkish);
            document->size = corpus_word_count(document_text);
            break;
        case CATEGORICAL:
            corpus = create_corpus2(document->absolute_file_name);
            if (corpus_sentence_count(corpus) >= 2){
                document_text = create_corpus();
                Sentence_ptr sentence = corpus_get_sentence(corpus, 1);
                String_ptr s = to_words(sentence);
                Array_list_ptr sentences = sentence_split_turkish(s->s);
                for (int i = 0; i < sentences->size; i++){
                    corpus_add_sentence(document_text, array_list_get(sentences, i));
                }
                document->size = corpus_number_of_words(document_text);
                free_string_ptr(s);
            }
            free_corpus(corpus);
            break;
    }
    return document_text;
}

/**
 * Frees memory allocated to the document.
 * @param document Document to be deallocated.
 */
void free_document(Document_ptr document) {
    free_(document->absolute_file_name);
    free_(document->file_name);
    free_(document);
}
