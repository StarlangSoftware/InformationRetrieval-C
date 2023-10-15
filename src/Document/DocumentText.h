//
// Created by Olcay Taner YILDIZ on 4.09.2023.
//

#ifndef INFORMATIONRETRIEVAL_DOCUMENTTEXT_H
#define INFORMATIONRETRIEVAL_DOCUMENTTEXT_H

#include <HashMap/HashSet.h>
#include <Corpus.h>
#include "../Index/TermType.h"

Hash_set_ptr construct_distinct_word_list(const Corpus* corpus, Term_type term_type);

Array_list_ptr construct_term_list(const Corpus* corpus, int doc_id, Term_type term_type);

#endif //INFORMATIONRETRIEVAL_DOCUMENTTEXT_H
