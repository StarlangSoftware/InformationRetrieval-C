//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_SEARCHPARAMETER_H
#define INFORMATIONRETRIEVAL_SEARCHPARAMETER_H

#include <stdbool.h>
#include "RetrievalType.h"
#include "../Document/DocumentWeighting.h"
#include "../Index/TermWeighting.h"
#include "CategoryDeterminationType.h"
#include "FocusType.h"

struct search_parameter{
    Retrieval_type retrieval_type;
    Document_weighting document_weighting;
    Term_weighting term_weighting;
    int documents_retrieved;
    Category_determination_type category_determination_type;
    Focus_type focus_type;
    bool search_attributes;
};

typedef struct search_parameter Search_parameter;

typedef Search_parameter *Search_parameter_ptr;

Search_parameter_ptr create_search_parameter();

void free_search_parameter(Search_parameter_ptr search_parameter);

#endif //INFORMATIONRETRIEVAL_SEARCHPARAMETER_H
