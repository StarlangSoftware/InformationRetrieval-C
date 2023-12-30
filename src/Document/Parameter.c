//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#include <stdlib.h>
#include <Memory/Memory.h>
#include "Parameter.h"

Parameter_ptr create_parameter() {
    Parameter_ptr parameter = malloc_(sizeof(Parameter), "create_parameter");
    parameter->index_type = INVERTED_INDEX;
    parameter->indexes_from_file = false;
    parameter->normalize_document = false;
    parameter->phrase_index = true;
    parameter->positional_index = true;
    parameter->n_gram_index = true;
    parameter->limit_number_of_documents_loaded = false;
    parameter->document_limit = 1000;
    parameter->word_limit = 10000;
    parameter->document_type = NORMAL;
    parameter->representative_count = 10;
    return parameter;
}

void free_parameter(Parameter_ptr parameter) {
    free_(parameter);
}
