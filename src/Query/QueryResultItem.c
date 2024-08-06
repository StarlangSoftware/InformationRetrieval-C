//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#include <stdlib.h>
#include <Memory/Memory.h>
#include "QueryResultItem.h"

Query_result_item_ptr create_query_result_item2() {
    Query_result_item_ptr result = malloc_(sizeof(Query_result_item), "create_query_result_item2");
    return result;
}

void free_query_result_item(Query_result_item_ptr query_result_item) {
    free_(query_result_item);
}

/**
 * Constructor for the QueryResultItem class. Sets the document id and score of a single query result.
 * @param doc_id Id of the document that satisfies the query.
 * @param score Score of the document for the query.
 */
Query_result_item_ptr create_query_result_item(int doc_id, double score) {
    Query_result_item_ptr result = malloc_(sizeof(Query_result_item), "create_query_result_item");
    result->doc_id = doc_id;
    result->score = score;
    return result;
}

/**
 * Compares two query result items according to their scores.
 * @param first the first query result item to be compared.
 * @param second the second query result item to be compared.
 * @return -1 if the score of the first item is smaller than the score of the second item; 1 if the score of the
 * first item is larger than the score of the second item; 0 otherwise.
 */
int compare_query_result_item(const Query_result_item *first, const Query_result_item *second) {
    if (first->score - second->score < 0){
        return -1;
    } else {
        if (first->score - second->score > 0){
            return 1;
        } else {
            return 0;
        }
    }
}
