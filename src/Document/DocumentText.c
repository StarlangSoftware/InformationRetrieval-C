//
// Created by Olcay Taner YILDIZ on 4.09.2023.
//

#include <Memory/Memory.h>
#include "DocumentText.h"
#include "../Index/TermOccurrence.h"

Hash_set_ptr construct_distinct_word_list(const Corpus* corpus, Term_type term_type) {
    Hash_set_ptr words = create_hash_set((unsigned int (*)(const void *, int)) hash_function_string,
                                         (int (*)(const void *, const void *)) compare_string);
    for (int i = 0; i < corpus_sentence_count(corpus); i++){
        Sentence_ptr sentence = corpus_get_sentence(corpus, i);
        for (int j = 0; j < sentence_word_count(sentence); j++){
            switch (term_type) {
                case TOKEN:
                    hash_set_insert(words, sentence_get_word(sentence, j));
                    break;
                case PHRASE:
                    if (j < sentence_word_count(sentence) - 1){
                        String_ptr st = create_string4(sentence_get_word(sentence, j), " ", sentence_get_word(sentence, j + 1));
                        hash_set_insert(words, st->s);
                        free_(st);
                    }
            }
        }
    }
    return words;
}

Array_list_ptr construct_term_list(const Corpus* corpus, int doc_id, Term_type term_type){
    Array_list_ptr terms = create_array_list();
    int size = 0;
    for (int i = 0; i < corpus_sentence_count(corpus); i++){
        Sentence_ptr sentence = corpus_get_sentence(corpus, i);
        for (int j = 0; j < sentence_word_count(sentence); j++){
            switch (term_type) {
                case TOKEN:
                    array_list_add(terms, create_term_occurrence(clone_string(sentence_get_word(sentence, j)), doc_id, size));
                    size++;
                    break;
                case PHRASE:
                    if (j < sentence_word_count(sentence) - 1){
                        String_ptr st = create_string4(sentence_get_word(sentence, j), " ", sentence_get_word(sentence, j + 1));
                        char* tmp = str_copy(tmp, st->s);
                        array_list_add(terms, create_term_occurrence(tmp, doc_id, size));
                        free_string_ptr(st);
                        size++;
                    }
            }
        }
    }
    return terms;
}