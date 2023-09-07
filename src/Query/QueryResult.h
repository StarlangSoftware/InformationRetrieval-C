//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_QUERYRESULT_H
#define INFORMATIONRETRIEVAL_QUERYRESULT_H

#include <ArrayList.h>
#include "QueryResultItem.h"

struct query_result{
    Array_list_ptr items;
};

typedef struct query_result Query_result;

typedef Query_result *Query_result_ptr;

Query_result_ptr create_query_result();

void free_query_result(Query_result_ptr query_result);

void add_to_query_result(Query_result_ptr query_result, int doc_id, double score);

void add_to_query_result2(Query_result_ptr query_result, int doc_id);

int size_of_query_result(const Query_result* query_result);

void get_best(Query_result_ptr query_result, int K);

Query_result_ptr intersection_fast_search(const Query_result* first, const Query_result* second);

Query_result_ptr intersection_binary_search(const Query_result* first, const Query_result* second);

Query_result_ptr intersection_linear_search(const Query_result* first, const Query_result* second);

#endif //INFORMATIONRETRIEVAL_QUERYRESULT_H
