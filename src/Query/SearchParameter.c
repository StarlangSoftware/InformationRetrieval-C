//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#include <stdlib.h>
#include <Memory/Memory.h>
#include "SearchParameter.h"

Search_parameter_ptr create_search_parameter() {
    Search_parameter_ptr result = malloc_(sizeof(Search_parameter), "create_search_parameter");
    result->retrieval_type = RANKED;
    result->document_weighting = NO_IDF;
    result->term_weighting = NATURAL;
    result->documents_retrieved = 1;
    result->category_determination_type = KEYWORD;
    result->focus_type = OVERALL;
    result->search_attributes = false;
    return result;
}

void free_search_parameter(Search_parameter_ptr search_parameter) {
    free_(search_parameter);
}
