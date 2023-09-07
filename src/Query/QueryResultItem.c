//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#include <stdlib.h>
#include "QueryResultItem.h"

Query_result_item_ptr create_query_result_item2() {
    Query_result_item_ptr result = malloc(sizeof(Query_result_item));
    return result;
}

void free_query_result_item(Query_result_item_ptr query_result_item) {
    free(query_result_item);
}

Query_result_item_ptr create_query_result_item(int doc_id, double score) {
    Query_result_item_ptr result = malloc(sizeof(Query_result_item));
    result->doc_id = doc_id;
    result->score = score;
    return result;
}

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
