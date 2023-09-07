//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_PARAMETER_H
#define INFORMATIONRETRIEVAL_PARAMETER_H

#include <stdbool.h>
#include "IndexType.h"
#include "DocumentType.h"

struct parameter{
    Index_type index_type;
    bool indexes_from_file;
    bool normalize_document;
    bool phrase_index;
    bool positional_index;
    bool n_gram_index;
    bool limit_number_of_documents_loaded;
    int document_limit;
    int word_limit;
    Document_type document_type;
    int representative_count;
};

typedef struct parameter Parameter;

typedef Parameter *Parameter_ptr;

Parameter_ptr create_parameter();

void free_parameter(Parameter_ptr parameter);

#endif //INFORMATIONRETRIEVAL_PARAMETER_H
